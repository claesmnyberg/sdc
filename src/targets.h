/*
 *  File: targets.h
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
 * $Id: targets.h,v 1.10 2006-05-07 20:34:47 cmn Exp $
 */

#ifndef _SDC_TARGETS_H
#define _SDC_TARGETS_H

#include "targets/linux_x86.h"
#include "targets/openbsd_x86.h"
#include "targets/openbsd_arm.h"
#include "targets/openbsd_sparc64.h"
#include "targets/freebsd_x86.h"
#include "targets/netbsd_x86.h"
#include "targets/solaris_sparc.h"
#include "targets/windows_x86.h"

struct target_arch targets[] =
{
	{ "Linux x86 (ELF)", ".sdc", linux_x86_elf, sizeof(linux_x86_elf)-1 },
	{ "Windows x86 (PE-COFF)", ".sdc.exe", windows_x86_pecoff, sizeof(windows_x86_pecoff)-1 },
	{ "OpenBSD x86 (ELF)", ".sdc", openbsd_x86_elf, sizeof(openbsd_x86_elf)-1 },
	{ "OpenBSD arm (ELF)", ".sdc", openbsd_arm_elf, sizeof(openbsd_arm_elf)-1 },
	{ "OpenBSD sparc64 (ELF)", ".sdc", openbsd_sparc64_elf, sizeof(openbsd_sparc64_elf)-1 },
	{ "FreeBSD x86 (ELF)", ".sdc", freebsd_x86_elf, sizeof(freebsd_x86_elf)-1 },
	{ "NetBSD x86 (ELF)", ".sdc", netbsd_x86_elf, sizeof(netbsd_x86_elf)-1 },
	{ "Solaris sparc (ELF)", ".sdc", solaris_sparc_elf, sizeof(solaris_sparc_elf)-1 },
	
	/* NULL terminate */
	{ NULL, NULL, NULL, 0 }
};

#endif /* _SDC_TARGETS_H */
