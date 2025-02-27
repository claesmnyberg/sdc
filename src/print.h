/*
 * print.h - Header file for commonly used print routines
 *
 *  Copyright (c) 2004 Claes M. Nyberg <md0claes@mdstud.chalmers.se>
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
 * $Id: print.h,v 1.2 2006-04-14 21:13:43 cmn Exp $
 */

#ifndef _CMN_PRINT_H
#define _CMN_PRINT_H

extern void verbose(unsigned int level, char *, ...);
extern void warn(const char *, ...);
extern void warnx(const char *, ...);
extern void warn_errno(const char *, ...);
extern void warn_errnox(const char *, ...);
extern void err(const char *, ...);
extern void errx(const char *, ...);
extern void err_errno(const char *, ...);
extern void err_errnox(const char *, ...);
#ifdef WINDOWS
extern const char *winerror(void);
#endif

#endif /* _CMN_PRINT_H */
