/*
 *  File: sdc.c
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
 * $Id: sdc.c,v 1.18 2007-02-22 12:23:50 cmn Exp $
 */

#ifdef WINDOWS
#include <windows.h>
#include <winsock.h>
#include <stdint.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "sdc.h"
#include "targets.h"

#ifndef DEFAULT_TARGET_ARCH
#define DEFAULT_TARGET_ARCH "linux_x86"
#endif


/* Local routines */
static char *sdc_file_name(char *, struct target_arch *);
static struct target_arch *target_resolve(char *);
static int file_encrypt(FILE *, FILE *, struct target_arch *, char *);
static struct target_arch *target_resolve(char *);


/*
 * Create the name of the encrypted file
 * by extracting the filename and adding the suffix.
 * (we do not use basename to simplify portability)
 */
char *
sdc_file_name(char *file, struct target_arch *tarch)
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
	
	snprintf(filename, sizeof(filename), "%s%s", pt, tarch->suffix);
	return(strdup(filename));
}


/*
 * Resolve target architecture by name.
 * Returns a pointer to the binary on success,
 * NULL if target was not found.
 */
struct target_arch *
target_resolve(char *name)
{
	struct target_arch *tgt;
	char buf[1024];
	char *pt;
	size_t i;

	tgt = targets;
	for (i=0; tgt[i].name != NULL; i++) {
		snprintf(buf, sizeof(buf), "%s", tgt[i].name);	

		if ( (pt = strstr(buf, "(")) != NULL) {
			*pt-- = '\0';
			while (isspace((int)*pt))
				*pt-- = '\0';
		}

		while ( (pt = strstr(buf, " ")) != NULL) 
			*pt = '_';
		
		if (!strncasecmp(buf, name, strlen(buf)))
			return(&tgt[i]);
	}

	/* Could not find target */
	if (tgt[i].name == NULL)
		return(NULL);
	
	return(tgt);
}


/*
 * Encrypt file.
 * Overwrites any existing file.
 * Returns 0 on success, -1 on error.
 */
int
file_encrypt(FILE *in, FILE *out,
	struct target_arch *tarch, char *key)
{
	struct bfish_key *bk;
	struct sdc_hdr hdr;
	struct sdc_hdr hdr_tmp;
	unsigned char buf[8192];
	int ret;
	size_t n;

	ret = -1;
	bk = NULL;

	if (strlen(key) > 56) {
		warn("Truncating encryption key to maximum size (56 bytes)\n");
		memset(&key[56], 0x00, strlen(key)-56);
	}
	
	if ( (bk = bfish_keyinit((unsigned char *)key, strlen(key))) == NULL) {
		err("Failed to init Blowfish key\n");
		goto finished;
	}

	/* Zero out key */
	memset(key, 0x00, strlen(key));

	/* Write target binary */
	verbose(2, "Writing target binary of %u bytes\n", tarch->proglen);
	if (fwrite(tarch->prog, tarch->proglen, 1, out) != 1) {
		err_errno("Failed to write self decrypting code to target file");
		goto finished;
	}

	/* Build file header */
	random_bytes((unsigned char *)&hdr, sizeof(hdr));	
	
	/* Copy header to work with */
	memcpy(&hdr_tmp, &hdr, sizeof(hdr));
	
	/* Encrypt and write header */
	hdr.magic1 = my_htonl(hdr.magic1);
	hdr.magic2 = hdr.magic1;
	verbose(2, "Using 0x%08x as magic number\n", my_ntohl(hdr.magic1));
	hdr.pad_prefix = my_htons(hdr.pad_prefix);

	bfish_cfb_encrypt((unsigned char *)&hdr + sizeof(hdr_tmp.iv), 
		sizeof(hdr)-sizeof(hdr_tmp.iv), hdr_tmp.iv, 8, bk);
	
	if (fwrite(&hdr, 1, sizeof(hdr), out) != sizeof(hdr)) {
		err_errno("Failed to write sdc header to target file");
		goto finished;
	}

	/* Write prefix pad */
	{
		unsigned char *pad = malloc(hdr_tmp.pad_prefix);
		if (pad == NULL) {
			err_errno("Failed to allocate memory for prefix pad");
			goto finished;
		}
		random_bytes(pad, hdr_tmp.pad_prefix);
		bfish_cfb_encrypt(pad, hdr_tmp.pad_prefix, hdr_tmp.iv, 8, bk);
		verbose(1, "Writing %u bytes of random bytes as prefix\n", hdr_tmp.pad_prefix);
		
		if (fwrite(pad, 1, hdr_tmp.pad_prefix, out) != hdr_tmp.pad_prefix) {
			err_errno("Failed to write pad to target file");
			free(pad);
			goto finished;
		}
		free(pad);
	}

	/* Read and write input file */
	do {
		n = fread(buf, 1, sizeof(buf), in);

		if (ferror(in)) {
			err_errno("Failed to read from input file");
			goto finished;
		}

		bfish_cfb_encrypt(buf, n, hdr_tmp.iv, 8, bk);
		
		if (fwrite(buf, 1, n, out) != n) {
			err_errno("Failed to write encrypted data to target file");
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
	
	if (out) {
		if (fclose(out) < 0)
			warn_errno("Failed to close output file");
	}

	if (bk)
		free(bk);
	return(ret);
}


void
usage(const char *pname) 
{
	struct target_arch *tgt;
	size_t i;
	
	printf("\n");
	printf("+------------------------------------------------------+\n");
	printf("     Self Decrypting File Generator, Version 1.2\n");
	printf(" Copyright (C) 2006 Claes M Nyberg <cmn@signedness.org>\n");
	printf("+------------------------------------------------------+\n");
	printf("\n");
	printf("Usage: %s [Option(s)] <file>\n", pname);
	printf("Options:\n");
	printf("  -h           - This help\n");
	printf("  -C <file>    - Check if file was generated by this program\n");
	printf("  -k <file>    - File with encryption key\n");
	printf("  -o <file>    - Output filename\n");
	printf("  -t <target>  - Target system\n");
	printf("  -v           - Be verbose, repeat to increase\n\n");
	printf("Targets:\n");

	tgt = targets;
	for (i=0; tgt[i].name != NULL; i++) {
		char buf[1024];
		char *pt;
		snprintf(buf, sizeof(buf), "%s", tgt[i].name);
		if ( (pt = strstr(buf, "(")) != NULL) {
			*pt-- = '\0';
			while(isspace((int)*pt))
				*pt-- = '\0';
		}


		/* Translate space character */
		while ( (pt = strstr(buf, " ")) != NULL)
			*pt = '_';
		
		printf("  %-16s - %s", buf, tgt[i].name);

		if (!strcasecmp(buf, DEFAULT_TARGET_ARCH))
			printf(" [default]");
		printf("\n");
	}
	
	printf("\n");
	exit(EXIT_FAILURE);
}

/*
 * Check if file is a self decrypting binary 
 * generated by this program.
 * Returns 1 if file was generated by this program, 
 * 0 otherwise and -1 on error.
 */
int
check_file(const char *file)
{
	struct target_arch *tgt;
	char *buf;
	FILE *in;
	size_t i;	

	if ( (in = fopen(file, "rb")) == NULL) {
		err_errno("Failed to open '%s'", file);
		return(-1);
	}

	tgt = targets;
	for (i=0; tgt[i].name != NULL; i++) {
		
		if ( (buf = malloc(tgt[i].proglen)) == NULL) {
			err_errno("Failed to allocate memory");
			return(-1);
		}

		rewind(in);
		
		if (fread(buf, tgt[i].proglen, 1, in) != 1) {
			free(buf);
			continue;
		}
		
		if (memcmp(buf, tgt[i].prog, tgt[i].proglen) == 0) {
			printf("%s [%s] was generated by this program\n", 
				file, tgt[i].name);
			fclose(in);
			free(buf);
			return(1);
		}

		free(buf);
	}

	warn("%s was not generated by this program\n", file);
	fclose(in);
	return(0);
}


int
main(int argc, char *argv[])
{
	int opt;
	extern unsigned int verbose_level;
	struct sdc_conf cfg;
	FILE *in;
	FILE *out;

	in = NULL;
	out = NULL;

	if (argv[1] == NULL) 
		usage(argv[0]);

	memset(&cfg, 0x00, sizeof(cfg));
	if ( (cfg.tarch = target_resolve(DEFAULT_TARGET_ARCH)) == NULL) {
		errx("Failed to resolve default target arch '%s'\n", 
			DEFAULT_TARGET_ARCH);
	}

	while ( (opt = getopt(argc, argv, "C:o:t:k:vh")) != -1) {
		
		switch (opt) {
			case 'C':
				check_file(optarg);
				exit(EXIT_SUCCESS);
				break;
				
			case 'o':
				cfg.file_out = optarg;
				break;

			case 'h':
				usage(argv[0]);
				break;

			case 'k':
				cfg.file_key = optarg;
				break;
			
			case 't':
				if ( (cfg.tarch = target_resolve(optarg)) == NULL)
					errx("Failed to resolve target architecture\n");
				break;
				
			case 'v':
				verbose_level++;
				break;

			default:
				usage(argv[0]);
		}
	}

	if (argv[optind] == NULL) 
		usage(argv[0]);

	cfg.file_in = argv[optind];

	if (cfg.file_out == NULL) {
		if ( (cfg.file_out = sdc_file_name(cfg.file_in, cfg.tarch)) == NULL)
			exit(EXIT_FAILURE);
	}
	
	/* Open the files */
	if ( (in = fopen(cfg.file_in, "rb")) == NULL) {
		err_errno("Failed to open input file '%s'", cfg.file_in);
		exit(EXIT_FAILURE);
	}

	if ( (out = fopen(cfg.file_out, "wb")) == NULL) {
		err_errno("Failed to open output file '%s'", cfg.file_out);
		exit(EXIT_FAILURE);
	}

#ifndef WINDOWS
	if (chmod(cfg.file_out, 0755) < 0)
		warn_errno("Failed to chmod target file");
#endif
		
	/* Read key from file */
	if (cfg.file_key != NULL) {
		if ( (cfg.key = key_file(cfg.file_key)) == NULL)
			exit(EXIT_FAILURE);
	}
	/* Read key from user */
	else {
		if ( (cfg.key = key_user(0)) == NULL)
			exit(EXIT_FAILURE);
	}

	printf("Writing self decrypting file %s for %s\n", 
		cfg.file_out, cfg.tarch->name);
	if (file_encrypt(in, out, cfg.tarch, cfg.key) < 0) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
