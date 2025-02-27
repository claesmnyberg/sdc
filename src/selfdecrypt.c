/*
 *  File: selfdecrypt.c
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
 * $Id: selfdecrypt.c,v 1.10 2006-05-01 15:45:36 cmn Exp $
 */


#ifdef WINDOWS
#include <winsock.h>
#include <stdint.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "print.h"
#include "sdc.h"

#ifdef LINUX
#include <stdint.h>
#endif

#ifdef WINDOWS
#include <winsock.h>
#include "pecoff.h"
#else
#include "elf.h"
#endif


/* Local routines */
static char *output_filename(char *);
static int file_decrypt(char *, char *, char *);


/*
 * Chop of extension to get output file name
 * Returns a pointer to the file name on success,
 * NULL on error.
 */
char *
output_filename(char *file)
{
	char filename[FILENAME_MAX];
	char *pt;

    if (file == NULL) {
        err("%s: Got NULL pointer as argument\n", __FUNCTION__);
        return(NULL);
    }

    pt = &file[strlen(file)-1];

    /* Find filename */
    while (*pt != '/' && *pt != '\\' && pt != file)
        pt--;

    if ((*pt == '/') || (*pt == '\\'))
        pt++;

    if (*pt == '\0') {
        err("Bad source file name (%s)\n", file);
        return(NULL);
    }

	snprintf(filename, sizeof(filename), "%s", pt);

	/* Delete suffix */
	if ( (pt = strstr(filename, ".sdc")) == NULL) {
		err("Missing suffix '.sdc' in filename\n");
		return(NULL);
	}
	else
		*pt = '\0';

	return(strdup(filename));
}

/*
 * Decrypt attached file
 * Returns 0 on success, and -1 on error.
 */
int
file_decrypt(char *file_in, char *file_out, char *key)
{
	struct bfish_key *bk;
	struct stat sb;
	struct sdc_hdr hdr;
	char buf[8192];
	uint64_t off;
	size_t n;
	FILE *in;
	FILE *out;
	int ret;
	
	bk = NULL;
	in = NULL;
	out = NULL;
	ret = -1;

#ifdef WINDOWS
	if ( (off = pecoff_offset(file_in)) == -1) {
		err("Failed to get PE-COFF offset: %s\n", pecoff_error());
		goto finished;
	}
#else
	if ( (off = elf_offset(file_in)) == -1) {
		err("Failed to get ELF offset: %s\n", elferr());
		goto finished;
	}
#endif

	verbose(2, "Got file offset of %u bytes\n", off);

	if ( (in = fopen(file_in, "rb")) == NULL) {
		err_errno("Failed to open input file");
		goto finished;
	}

	if (fseek(in, off, SEEK_SET) == -1) {
		err_errno("Failed to set offset to encrypted file");
		goto finished;
	}

	/* Write decrypted file to standard output */
	if (!strcmp(file_out, "-"))
		out = stdout;
	else {
		if ( (out = fopen(file_out, "wb")) == NULL) {
			err_errno("Failed to open output file");
			goto finished;
		}
	}

    if (strlen(key) > 56) {
        warn("Truncating encryption key to maximum size (56 bytes)\n");
        memset(&key[56], 0x00, strlen(key)-56);
    }

	if ( (bk = bfish_keyinit((unsigned char *)key, strlen(key))) == NULL) {
		err("Failed to init Blowfish key\n");
		goto finished;
	}

	/* Read header */
	if (fread(&hdr, 1, sizeof(hdr), in) != sizeof(hdr)) {
		err_errno("Failed to read header from enrypted file");
		goto finished;
	}

	/* Decrypt header */
	bfish_cfb_decrypt((unsigned char *)&hdr + sizeof(hdr.iv), 
		sizeof(hdr)-sizeof(hdr.iv), hdr.iv, 8, bk); 

	hdr.magic1 = my_ntohl(hdr.magic1);
	hdr.magic2 = my_ntohl(hdr.magic2);
	hdr.pad_prefix = my_ntohs(hdr.pad_prefix);

	/* Bad key */
	verbose(2, "magic1 = 0x%08x, magic2 = 0x%08x\n", 
		hdr.magic1, hdr.magic2);
	
	if (hdr.magic1 != hdr.magic2) {
		err("Bad decryption key\n");
		goto finished;
	}

	/* Remove leading */
	{
		unsigned char *pad = malloc(hdr.pad_prefix);
		if (pad == NULL) {
			err_errno("Failed to allocate memory for pad");
			goto finished;
		}

		verbose(1, "Ignoring %u bytes of random prefix\n", hdr.pad_prefix);
		if (fread(pad, 1, hdr.pad_prefix, in) != hdr.pad_prefix) {
			err_errno("Failed to read pad from input file");
			free(pad);
			goto finished;
		}
		bfish_cfb_decrypt(pad, hdr.pad_prefix, hdr.iv, 8, bk);
		free(pad);
	}

    /* Read and decrypt input file */
    do {
        n = fread(buf, 1, sizeof(buf), in);
		if (ferror(in)) {
			err_errno("Failed to read from input file");
			goto finished;
		}
        bfish_cfb_decrypt((unsigned char *)buf, n, hdr.iv, 8, bk);

        if (fwrite(buf, 1, n, out) != n) {
            err_errno("Failed to write decrypted data to target file");
            goto finished;
        }
    } while (n == sizeof(buf));

    if (feof(in) == 0) {
        err_errno("Failed to read from input file");
        goto finished;
    }

	ret = 0;
    finished:

    if (in) {
        if (fclose(in) < 0)
            warn_errno("Failed to close input file");
    }

    if (out && (out != stdout) ) {
        if (fclose(out) < 0)
            warn_errno("Failed to close output file");
    }

    if (bk)
        free(bk);

	/* Delete empty output file on error */
	if (ret == -1) {

		if ( (stat(file_out, &sb) != -1) && (sb.st_size == 0)) {
#ifdef WINDOWS
			DeleteFile(file_out);
#else
			unlink(file_out);
#endif
		}
	}

	return(ret);	
}


void
usage(const char *pname)
{

    printf("+------------------------------------------------------+\n");
    printf("  Self Decrypting File (Version 1.0) Generated by SDC\n");
	printf("        http://www.signedness.org/tools/\n");
    printf(" Copyright (C) 2006 Claes M Nyberg <cmn@signedness.org>\n");
    printf("+------------------------------------------------------+\n");

	printf("Usage: %s [<filename>] [Option(s)]\n", pname);
	printf("Option(s):\n");
	printf("  -k <file>   - Read key from file\n");
	printf("  -v          - Be verbose, repeat to increase\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	char *pname;
	char *key;
	char *file_out;
	int opt;
	extern unsigned int verbose_level;

	key = NULL;
	file_out = NULL;
	pname = argv[0];


	/* Output filename, "-" for standard out */
	if ( (argv[1] && (argv[1][0] != '-')) || (argv[1] && !strcmp(argv[1], "-")) ) {
		file_out = argv[1];
		argc--;
		argv++;
	}


	while ( (opt = getopt(argc, argv, "k:v")) != -1) {
		switch(opt) {
			case 'k':
				if ( (key = key_file(optarg)) == NULL)
					exit(EXIT_FAILURE);
				break;

			case 'v':
				verbose_level++;
				break;

			default:
				usage(pname);
		}
	}

	if (file_out == NULL) {
		if ( (file_out = output_filename(pname)) == NULL) 
			errx("Failed to resolve output filename\n");	
	}

	if (key == NULL) {
		if ( (key = key_user(1)) == NULL)
			errx("Failed to read decryption key\n");
	}

	printf("Decrypting to %s\n", 
		strcmp(file_out, "-") == 0 ? "standard out": file_out);
	
	if (file_decrypt(pname, file_out, key) < 0)
		exit(EXIT_FAILURE);

	exit(EXIT_SUCCESS);
}
