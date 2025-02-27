/*
 *  File: elf.h
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
 * $Id: elf.h,v 1.4 2006-04-18 23:11:50 cmn Exp $
 */

#ifndef _SDC_ELF_H
#define _SDC_ELF_H


/* ELF Header */
typedef struct {
  uint8_t e_ident[16];        /* Magic number and other info */
  uint16_t e_type;         	  /* Object file type */
  uint16_t e_machine;      	  /* Architecture */
  uint32_t e_version;         /* Object file version */
  uint32_t e_entry;           /* Entry point virtual address */
  uint32_t e_phoff;        	  /* Program header table file offset */
  uint32_t e_shoff;        	  /* Section header table file offset */
  uint32_t e_flags;        	  /* Processor-specific flags */
  uint16_t e_ehsize;       	  /* ELF header size in bytes */
  uint16_t e_phentsize;       /* Program header table entry size */
  uint16_t e_phnum;        	  /* Program header table entry count */
  uint16_t e_shentsize;       /* Section header table entry size */
  uint16_t e_shnum;        	  /* Section header table entry count */
  uint16_t e_shstrndx;     	  /* Section header string table index */
} Elf32_Ehdr;


/* ELF Header */
typedef struct {
  uint8_t e_ident[16];        /* Magic number and other info */
  uint16_t e_type;         	  /* Object file type */
  uint16_t e_machine;      	  /* Architecture */
  uint32_t e_version;         /* Object file version */
  uint64_t e_entry;           /* Entry point virtual address */
  uint64_t e_phoff;        	  /* Program header table file offset */
  uint64_t e_shoff;        	  /* Section header table file offset */
  uint32_t e_flags;        	  /* Processor-specific flags */
  uint16_t e_ehsize;       	  /* ELF header size in bytes */
  uint16_t e_phentsize;       /* Program header table entry size */
  uint16_t e_phnum;        	  /* Program header table entry count */
  uint16_t e_shentsize;       /* Section header table entry size */
  uint16_t e_shnum;        	  /* Section header table entry count */
  uint16_t e_shstrndx;     	  /* Section header string table index */
} Elf64_Ehdr;


#define EI_CLASS	4
#define ET_EXEC 	2
#define ELFCLASS32	1
#define ELFCLASS64	2


/* elf.c */
extern int64_t elf_offset(const char *);
extern const char *elferr(void);

#endif /* _SDC_ELF_H */
