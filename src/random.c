/*
 *  File: random.c
 *  Copyright (c) 2006 Claes M. Nyberg <cmn@signedness.org>
 *  All rights reserved, all wrongs reversed.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 *  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: random.c,v 1.2 2006-04-14 22:54:19 cmn Exp $
 */

#ifdef WINDOWS
#include <windows.h>
#else
#include <fcntl.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>


/*
 * Generate len bytes of random bytes and store it in buf. 
 * Code influed by lib/libkern/random.c (NetBSD 1.6.1).
 */ 
void
random_bytes(unsigned char *buf, size_t len)
{
    static unsigned long randseed = 1;
	struct timeval tv;
    long x; 
    size_t i;
    long hi; 
    long lo;
    long t;

    /* First time throught, create seed */
    if (randseed == 1) {
#ifdef WINDOWS
		tv.tv_sec = time(NULL);
		tv.tv_usec = GetTickCount() ^ getpid();
#else
		int fd;
		if ( (fd = open("/dev/random", O_RDONLY)) > 0) {
			if (read(fd, &randseed, sizeof(randseed)) != sizeof(randseed))
				fprintf(stderr, "** Warning: Failed to read all randomness\n");
			close(fd);
		}
		tv.tv_sec = time(NULL); /* In case gettimeofday fails */
		gettimeofday(&tv, NULL);
		tv.tv_usec ^= getpid();
#endif
		randseed ^= tv.tv_sec ^ tv.tv_usec;
	}

    for (i=0; i<len; i++) {

        /*
         * Compute x[n + 1] = (7^5 * x[n]) mod (2^31 - 1).
         * From "Random number generators: good ones are hard to find",
         * Park and Miller, Communications of the ACM, vol. 31, no. 10,
         * October 1988, p. 1195.
         */
        x = randseed;
        hi = x / 127773;
        lo = x % 127773;
        t = 16807 * lo - 2836 * hi;
        if (t <= 0)
            t += 0x7fffffff;
        randseed = t;

        buf[i] = (unsigned char)t;
    }
}

