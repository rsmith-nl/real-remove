/* file: rrm.c
 * vim:fileencoding=utf-8:fdm=marker:ft=c
 * Overwrites files with random data and unlinks them.
 *
 * Copyright © 2008 R.F. Smith <rsmith@xs4all.nl>.
 * SPDX-License-Identifier: 2BSD
 * Created: 2008-11-09T12:02:44+01:00
 * Last modified: 2023-03-24T19:14:33+0100
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <ctype.h>

#ifndef VERSION
#define VERSION "unknown"
#endif

off_t   filesize(char *name);
char *  newname(char *from);
void    report(char *func, char *path);

int
main(int argc, char *argv[])
{
    int     t;
    ssize_t rv;
    int     f;
    off_t    size = 0;
    char   *buf;
    char   *n;

    if (argc == 1) {
        fprintf(stderr, "rrm version %s\n", VERSION);
        fprintf(stderr, "usage: rrm file ...\n");
        return 0;
    }
    for (t = 1; t < argc; t++) {
        size = filesize(argv[t]);
        if (size == -1) {
            report("filesize", argv[t]);
            continue;
        }

        f = open(argv[t], O_RDWR);
        if (f == -1) {
            report("open(2)", argv[t]);
            continue;
        }
        buf = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, f, 0);
        if (buf == MAP_FAILED) {
            report("mmap(2)", argv[t]);
            close(f);
            continue;
        }
        arc4random_buf((void *)buf, size);
        munmap(buf, size);
        close(f);
        n = newname(argv[t]);
        if (n == NULL) {
            n = argv[t];
        } else {
            rv = rename(argv[t], n);
            if (rv) {
                report("rename(2)", argv[t]);
                n = argv[t];
            }
        }
        if (unlink(n) == -1) {
            report("unlink(2)", argv[t]);
        }
    }
    return 0;
}


void
report(char *func, char *path)
{
    fprintf(stderr, "%s failed for “%s”: %s\n", func, path, strerror(errno));
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
        fprintf(stderr, "opening /dev/random failed.");
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
