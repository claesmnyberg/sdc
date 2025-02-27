/*
 *  File: sdc.h
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
 * $Id: sdc.h,v 1.4 2006-04-19 09:41:28 cmn Exp $
 */

#ifndef _SDC_H
#define _SDC_H

#ifdef WINDOWS
#include <windows.h>
#include <stdint.h>
#endif

#ifdef LINUX
#include <stdint.h>
#endif
#include <sys/types.h>
#include "print.h"
#include "bfish.h"
#include "key.h"
#include "random.h"

/* We use our own macros to avoid linking the winsock dll on windows 
 * since that is just another dependency */
#ifndef SDC_BIG_ENDIAN
#define my_ntohs(s) ((((s) >> 8) & 0xff) | ( ((s) << 8) & 0xff00))
#define my_htons(s) my_ntohs(s)
#define my_ntohl(l) \
    ((my_htons( ((l) >> 16) & 0xffff)) | (my_ntohs(((l) & 0xffff)) << 16))
#define my_htonl(l) my_ntohl(l)

#else
#define my_ntohs(s) (s)
#define my_htons(s) (s)
#define my_ntohl(s) (s)
#define my_htonl(s) (s)
#endif

struct target_arch {
	char *name;
	char *suffix;
	char *prog;
	size_t proglen;
};

struct sdc_conf {
	
	/* Input filename */
	char *file_in;

	/* Output filename */
	char *file_out; 

	/* Keyfile */
	char *file_key;
	
	/* Output architecture binary */
	struct target_arch *tarch;

	/* Output architecture name */
	char *target_arch_name;

	/* The password to use when encrypting */
	char *key;
};


/*
 * The SDC header prepended to the encrypted file.
 */
struct sdc_hdr {

	/* Initial vector */
	uint8_t iv[8];

	/* Everything from this point is encrypted */
	
	/* Magic random numbers used to verify the decryption key. 
	 * If magic1 equals magic2, we assume that the key is correct */
	uint32_t magic1;	
	uint32_t magic2;

	/* Pad before file */
	uint16_t pad_prefix;

	uint16_t unused;	
	
} __attribute__((packed));

#endif /* _SDC_H */
