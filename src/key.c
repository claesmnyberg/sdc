/*
 *  File: key.c
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
 * $Id: key.c,v 1.6 2007-02-22 12:23:50 cmn Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "print.h"
#ifdef WINDOWS
#include "win32getpass.h"
#endif


/*
 * Read key from user
 */
char *
key_user(int decrypt)
{
    char buf1[1024];
    char buf2[1024];
    char *key;

#ifdef WINDOWS
	/* Read decryption key */
	if (decrypt) {
		key = win32getpass(NULL);
	}
	else {
		key = win32getpass("Enter Decryption Key");
		snprintf(buf1, sizeof(buf1), "%s", key);

		key = win32getpass("Verify Decryption Key");
		snprintf(buf2, sizeof(buf2), "%s", key);
	
		if (strcmp(buf1, buf2)) {
			err("Keys mismatch\n");
			return(NULL);
		}
	}
#else

    int attempts;
    for (attempts=0; ;attempts++) {

        memset(buf1,0x00, sizeof(buf1));
        if ( (key = getpass("Encryption key: ")) == NULL) {
            err_errno("Failed to read encryption key");
            return(NULL);
        }
		if (decrypt)
			return(key);

        snprintf(buf1, sizeof(buf1), "%s", key);

        memset(buf2,0x00, sizeof(buf1));
        if ( (key = getpass("Verify key: ")) == NULL) {
            err_errno("Failed to read encryption key");
            return(NULL);
        }
        snprintf(buf2, sizeof(buf2), "%s", key);

        if (!strcmp(buf1, buf2))
            break;

        printf("*** Keys mismatch\n\n");

        if (attempts == 3) {
            err("Failed to read key\n");
			return(NULL);
		}
    }

    memset(key, 0x00, strlen(key));
    key = strdup(buf1);
    memset(buf1, 0x00, sizeof(buf1));
    memset(buf2, 0x00, sizeof(buf2));
#endif

    return(key);
}

/*
 * Read key from file
 */
char *
key_file(char *file)
{
    char buf[1024];
    FILE *f;
    char *key;

    key = NULL;

    if ( (f = fopen(file, "rb")) == NULL) {
        err_errno("Failed to open key file");
        return(NULL);
    }

    memset(buf, 0x00, sizeof(buf));
    if (fgets(buf, sizeof(buf), f) == NULL) {
        err_errno("Failed to read key from keyfile");
        goto finished;
    }

    /* Delete newline character(s) */
    if (strlen(buf) > 0) {
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';
    }
    if (strlen(buf) > 0) {
        if (buf[strlen(buf)-1] == '\r')
            buf[strlen(buf)-1] = '\0';
    }

    key = strdup(buf);
    finished:

    if (fclose(f) < 0)
        warn_errno("Failed to close key file");

    return(key);
}


