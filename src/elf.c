/*
 *  File: elf.c
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
 * $Id: elf.c,v 1.7 2006-04-18 23:09:13 cmn Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#ifdef LINUX
#include <stdint.h>
#endif
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "elf.h"


/* Error buffer */
static char errbuf[2048];

/*
 * Return last error as a string
 */
const char *
elferr(void)
{
	return(errbuf);
}

/*
 * Find the offset to the end of the ELF binary.
 * Returns the offset on success, -1 on error.
 */
int64_t
elf_offset(const char *elf_file)
{
	Elf32_Ehdr *eh;
	Elf64_Ehdr *eh64;
	struct stat sb;
	int64_t elf_size;
	void *map;
	int fd;

	elf_size = 0;

	if ( (fd = open(elf_file, O_RDONLY)) < 0) {
		snprintf(errbuf, sizeof(errbuf), "open(): %s", 
			strerror(errno));
		return(-1);
	}

	if (fstat(fd, &sb) < 0) {
		snprintf(errbuf, sizeof(errbuf), "fstat(): %s", 
			strerror(errno));
		return(-1);
	}

	if ( (map = mmap(0, sb.st_size, PROT_READ, 
			MAP_PRIVATE, fd, 0)) == NULL) {
		snprintf(errbuf, sizeof(errbuf), "mmap(): %s", 
			strerror(errno));
		return(-1);
	}

	eh = (Elf32_Ehdr *)map;

	/* Check for ELF tag */
	if (memcmp(eh->e_ident, "\x7f\x45\x4c\x46", 4)) {
		snprintf(errbuf, sizeof(errbuf), "Not an ELF file"); 
		return(-1);
	}


	if (eh->e_ident[EI_CLASS] == ELFCLASS32) {
		/* End of sections */
		if (eh->e_shoff > eh->e_phoff)
			elf_size = eh->e_shoff + (eh->e_shnum*eh->e_shentsize);

		/* End of programs */
		if (eh->e_phoff > eh->e_shoff) 
			elf_size = eh->e_phoff + (eh->e_phnum*eh->e_phentsize);
	}
	else if (eh->e_ident[EI_CLASS] == ELFCLASS64) {
		eh64 = (Elf64_Ehdr *)eh;

		/* End of sections */
		if (eh64->e_shoff > eh64->e_phoff) 
			elf_size = eh64->e_shoff + (eh64->e_shnum*eh64->e_shentsize);

		/* End of programs */
		if (eh64->e_phoff > eh64->e_shoff) 
			elf_size = eh64->e_phoff + (eh64->e_phnum*eh64->e_phentsize);
	}
	else {
		snprintf(errbuf, sizeof(errbuf), "Unrecognized ELF class (not 32 or 64 bit)");
		return(-1);
	}

	if (munmap(map, sb.st_size) < 0) {
		snprintf(errbuf, sizeof(errbuf), "munmap(): %s", 
			strerror(errno));
		return(-1);
	}

	if (close(fd) < 0) {
		snprintf(errbuf, sizeof(errbuf), "close(): %s", 
			strerror(errno));
		return(-1);
	}

	return(elf_size);
}
