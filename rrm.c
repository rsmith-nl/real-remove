/* rrm.c
 * Overwrites files with random data and unlinks them.
 * Copyright © 2008,2014 R.F. Smith <rsmith@xs4all.nl>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFSIZE 1048576

#ifndef NULL
#define NULL (void*)0
#endif

off_t   filesize(char *name);
char   *newname(char *from);

#ifndef NDEBUG
/* __FUNCTION__ and varags macros are a GCC feature. */
#ifdef __GNUC__
#undef DEBUG
#define DEBUG(a...) fprintf(stderr,"%s, line %i, %s(): ",\
__FILE__,__LINE__,__FUNCTION__); fprintf(stderr, ## a); fprintf(stderr, "\n")
#else
#undef DEBUG
#define DEBUG(a...) fprintf(stderr, ## a); fprintf(stderr, "\n")
#endif                                  /* __GNUC__ */
#else
#undef DEBUG
#define DEBUG(a...) (void)0
#endif                                  /* NDEBUG */


int
main(int argc, char *argv[])
{
    int     t, rnd;
    ssize_t rv;
    int     f;
    long    size = 0;
    char   *buf;
    char   *n;

    if (argc == 1) {
        fprintf(stderr, "%s version %s\n", PACKAGE, VERSION);
        fprintf(stderr, "usage: rrm file ...\n");
        return 0;
    }
    buf = calloc(1, BUFSIZE);
    if (buf == NULL) {
        perror("Cannot allocate memory: ");
        return 1;
    }
    rnd = open("/dev/random", O_RDONLY);
    if (rnd == -1) {
        fprintf(stderr, "Cannot open random device.");
        return 2;
    }
    for (t = 1; t < argc; t++) {
        size = filesize(argv[t]);
        if (size == -1) {
            fprintf(stderr,
                    "Cannot establish the size of %s. ",
                    argv[t]);
            perror(NULL);
            continue;
        }
        DEBUG("file %s is %ld bytes.\n", argv[t], size);
        f = open(argv[t], O_RDWR);
        if (f == -1) {
            fprintf(stderr, "Cannot open %s. ", argv[t]);
            perror(NULL);
            continue;
        }
        do {
            rv = read(rnd, (void *)buf, BUFSIZE);
            if (rv != BUFSIZE) {
                fprintf(stderr, "Not enough random data.");
                close(f);
            }
            write(f, buf, BUFSIZE);
            size -= BUFSIZE;
        } while (size > BUFSIZE);
        rv = read(rnd, (void *)buf, (size_t)size);
        if (rv != size) {
            fprintf(stderr, "Not enough random data.");
        } else {
            write(f, buf, (size_t)size);
        }
        close(f);
        n = newname(argv[t]);
        rv = rename(argv[t], n);
        if (rv) {
            fprintf(stderr, "Cannot rename %s. ", argv[t]);
            perror(NULL);
            n = argv[t];
        } else {
            DEBUG("renamed \"%s\" to \"%s\"\n", argv[t], n);
        }
        if (unlink(n) == -1) {
            fprintf(stderr, "Cannot unlink %s. ", n);
            perror(NULL);
        }
    }
    free(buf);
    return 0;
}


off_t
filesize(char *name)
{
    struct stat st;

    if (stat(name, &st) == 0)
        return st.st_size;

    return -1;
}

char   *
newname(char *from)
{
    char   *name;
    char    newchar;
    size_t  l, t;
    static char buf[1024];

    srandomdev();
    bzero(buf, 1024);
    if (strlen(from) > 1023) {
        return NULL;
    }
    strncpy(buf, from, 1023);
    name = strrchr(buf, '/');
    if (name == NULL) {
        name = buf;
    } else {
        name++;
    }
    l = strlen(name);
    if (l == 0) {
        return NULL;
    }
    for (t = 0; t < l; t++) {
        do {
            newchar = (char)(random() / 16777215);
        } while (isalpha(newchar) == 0);
        name[t] = newchar;
    }
    return buf;
}

/* EOF rrm.c */
