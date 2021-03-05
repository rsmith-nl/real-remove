/* file: rrm.c
 * vim:fileencoding=utf-8:fdm=marker:ft=c
 * Overwrites files with random data and unlinks them.
 *
 * Copyright © 2008-2021 R.F. Smith <rsmith@xs4all.nl>.
 * SPDX-License-Identifier: 2BSD
 * Created: 2008-11-09T12:02:44+01:00
 * Last modified: 2021-03-05T23:03:10+0100
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFSIZE 1048576

#ifndef VERSION
#define VERSION "unknown"
#endif

off_t   filesize(char *name);
char *  newname(char *from);

int
main(int argc, char *argv[])
{
    int     t;
    ssize_t rv;
    int     f;
    long    size = 0;
    char   *buf;
    char   *n;

    if (argc == 1) {
        fprintf(stderr, "rrm version %s\n", VERSION);
        fprintf(stderr, "usage: rrm file ...\n");
        return 0;
    }
    buf = malloc(BUFSIZE);
    if (buf == NULL) {
        perror("Cannot allocate memory: ");
        return 1;
    }
    for (t = 1; t < argc; t++) {
        size = filesize(argv[t]);
        if (size == -1) {
            perror(argv[t]);
            continue;
        }
        f = open(argv[t], O_RDWR);
        if (f == -1) {
            perror(argv[t]);
            continue;
        }
        while (size > BUFSIZE) {
            arc4random_buf((void *)buf, BUFSIZE);
            if (write(f, buf, BUFSIZE) == -1) {
                perror(argv[t]);
                break;
            } else {
                size -= BUFSIZE;
            }
        }
        if (write(f, buf, (size_t)size) == -1) {
            perror(argv[t]);
        }
        close(f);
        n = newname(argv[t]);
        if (n == NULL) {
            n = argv[t];
        } else {
            rv = rename(argv[t], n);
            if (rv) {
                perror(argv[t]);
                n = argv[t];
            }
        }
        if (unlink(n) == -1) {
            perror(argv[t]);
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

char *
newname(char *from)
{
    char   *name;
    char    newchar;
    size_t  l, t;
    int     rnd;
    static char buf[1024];

    arc4random_buf((void *)buf, 1023);
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
    rnd = open("/dev/random", O_RDONLY);
    if (rnd == -1) {
        perror(NULL);
        return NULL;
    }
    for (t = 0; t < l; t++) {
        do {
            read(rnd, &newchar, 1);
        } while (isalpha(newchar) == 0);
        name[t] = newchar;
    }
    close(rnd);
    return buf;
}

/* EOF rrm.c */
