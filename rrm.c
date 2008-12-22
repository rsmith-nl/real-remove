/* -*- c -*-
 * Time-stamp: <2008-12-22 13:55:28 rsmith>
 * 
 * rrm.c
 * Overwrites files with zeros and unlinks them.
 * Copyright © 2008 R.F. Smith <rsmith@xs4all.nl>. All rights reserved.
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define BUFSIZE 4096

#ifndef NULL
#define NULL (void*)0
#endif

char *newname(char *from);

#ifndef NDEBUG
/* __FUNCTION__ and varags macros are a GCC feature. */
#  ifdef __GNUC__
#    undef DEBUG
#    define DEBUG(a...) fprintf(stderr,"%s, line %i, %s(): ",\
__FILE__,__LINE__,__FUNCTION__); fprintf(stderr, ## a); fprintf(stderr, "\n")
#  endif        /* __GNUC__ */
#else
#  undef DEBUG
#  define DEBUG(a...) (void)0
#endif          /* NDEBUG */


int main(int argc, char *argv[])
{
	int t, rv;
	FILE *f;
	long size = 0;
	char *buf;
	char *n;

	if (argc==1) {
		fprintf(stderr, "%s version %s\n", PACKAGE, VERSION);
		fprintf(stderr, "usage: rrm file ...\n");
		return 0;
	}
	
	buf = calloc(1,BUFSIZE);
	if (buf==NULL) {
		perror("Cannot allocate memory: ");
		return 1;
	}

	for (t=1; t<argc; t++) {
		f = fopen(argv[t],"r+");
		if (f==NULL) {
			fprintf(stderr, "Cannot open %s. ", argv[t]);
			perror(NULL);
			continue;
		}
		if (fseek(f, 0, SEEK_END)==-1) {
			perror("Cannot seek to end-of-file: ");
			fclose(f);
			continue;
		}
		size = ftell(f);
		if (size==-1) {
			fprintf(stderr, 
				"Cannot establish the size of %s. ", 
				argv[t]);
			perror(NULL);
			fclose(f);
			continue;		
		}
		DEBUG("file %s is %ld bytes.\n", argv[t], size);
		rewind(f);
		do {
			fwrite(buf,BUFSIZE,1,f);
			size -= BUFSIZE;
		} while (size > BUFSIZE);
		fwrite(buf,size,1,f);
		fclose(f);
		n = newname(argv[t]);
		rv = rename(argv[t], n);
		if (rv) {
			fprintf(stderr, "Cannot rename %s. ", argv[t]);
			perror(NULL);
			n = argv[t];
		}
		DEBUG("renamed %s to %s\n", argv[t], n);
		if (unlink(n)==-1) {
			fprintf(stderr, "Cannot unlink %s. ", n);
			perror(NULL);
		}
	}
	free(buf);
	return 0;
}
/* EOF rrm.c */

char *newname(char *from) {
	char *name;
	size_t l, t;
	static char buf[1024];

	bzero(buf,1024);
	if (strlen(from)>1023) {
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
	for (t=0; t<l; t++) {
		name[t] = (char)(random() & 127);
	}
	return buf;
}

