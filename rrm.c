/* -*- c -*-
 * Time-stamp: <2008-11-09 16:42:44 rsmith>
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

#define BUFSIZE 4096

int main(int argc, char *argv[])
{
	int t;
	FILE *f;
	long size = 0;
	char *buf;
	size_t rv;

	if (argc==1) {
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
		rewind(f);
		do {
			fwrite(buf,BUFSIZE,1,f);
			size -= BUFSIZE;
		} while (size > BUFSIZE);
		fwrite(buf,size,1,f);
		fclose(f);
		if (unlink(argv[t])==-1) {
			fprintf(stderr, "Cannot unlink %s. ", argv[t]);
			perror(NULL);
		}
	}
	free(buf);
	return 0;
}
/* EOF rrm.c */
