
#
# Project default Makefile
# $Id: Makefile,v 1.15 2007-02-22 12:29:57 cmn Exp $
#

#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
SHELL		= /bin/sh
HOME		= ${PWD}
CC			= gcc
CC_RC		= mingw32-windres
RC          = win32getpass.rc
WIN32_CC    = mingw32-gcc
CFLAGS		= -Wall -static -O -fomit-frame-pointer -pipe -s

SELF_OBJS   = selfdecrypt.o print.o bf_keyinit.o bf_decrypt.o bf_encrypt.o \
			  bf_cfb.o key.o
SELF_PROG   = selfdecrypt
SELF_LIBS   =

SDC_OBJS    = sdc.o print.o random.o bf_keyinit.o bf_encrypt.o bf_cfb.o \
              key.o
SDC_PROG	= sdc
SDC_MAN     = sdc.1
SDC_LIBC    =

ELF_OBJS    = elf.o 
PECOFF_OBJS = pecoff.o
#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

none: 
	@echo
	@echo "make <target>"
	@echo
	@echo "SDC generator targets:"
	@echo "  linux_x86"
	@echo "  windows_x86"
	@echo "  openbsd_x86"
	@echo "  openbsd_arm"
	@echo "  openbsd_sparc64"
	@echo "  freebsd_x86"
	@echo "  netbsd_x86"
	@echo "  solaris_sparc"
	@echo
	@echo "Self decrypt targets:"
	@echo "  selfdecrypt_linux_x86"
	@echo "  selfdecrypt_windows_x86"
	@echo "  selfdecrypt_openbsd_x86"
	@echo "  selfdecrypt_openbsd_arm"
	@echo "  selfdecrypt_openbsd_sparc64"
	@echo "  selfdecrypt_netbsd_x86"
	@echo "  selfdecrypt_freebsd_x86"
	@echo "  selfdecrypt_solaris_sparc"
	@echo

all: none

sdc: ${SDC_OBJS}
	@${CC} ${CFLAGS} -o ${SDC_PROG} ${SDC_OBJS} ${SDC_LIBS}

selfdecrypt: ${SELF_OBJS}
	@${CC} ${CFLAGS} -o ${SELF_PROG} ${SELF_OBJS} ${SELF_LIBS}

# Linux x86
selfdecrypt_linux_x86::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='${CFLAGS} -DLINUX -DLINUX_X86' selfdecrypt > /dev/null
	@echo "[+] Generating linux_x86.h"
	@echo "#ifndef _SDC_LINUX_X86"          > linux_x86.h
	@echo "#define _SDC_LINUX_X86"         >> linux_x86.h
	@echo                                  >> linux_x86.h
	@echo "static char linux_x86_elf[] = " >> linux_x86.h
	@./genhex.pl ${SELF_PROG} 15            >> linux_x86.h
	@echo ";"                              >> linux_x86.h
	@echo                                  >> linux_x86.h
	@echo "#endif"                         >> linux_x86.h

linux_x86::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='${CFLAGS} \
		-DLINUX -DLINUX_X86	-DDEFAULT_TARGET_ARCH=\"linux_x86\"' sdc > /dev/null

# Windows x86
selfdecrypt_windows_x86::
	@echo "[+] Compiling ${SELF_PROG}.exe"
	${CC_RC} -o ${RC}.o ${RC}
	@make SELF_OBJS='${SELF_OBJS} ${PECOFF_OBJS} win32getpass.o ${RC}.o' \
		CC=${WIN32_CC} CFLAGS='${CFLAGS} -DWINDOWS -DWINDOWS_X86' \
		SELF_PROG='${SELF_PROG}.exe' selfdecrypt 
	@echo "[+] Generating windows_x86.h"
	@echo "#ifndef _SDC_WINDOWS_X86"          > windows_x86.h
	@echo "#define _SDC_WINDOWS_X86"         >> windows_x86.h
	@echo                                  >> windows_x86.h
	@echo "static char windows_x86_pecoff[] = " >> windows_x86.h
	@./genhex.pl ${SELF_PROG}.exe 15            >> windows_x86.h
	@echo ";"                              >> windows_x86.h
	@echo                                  >> windows_x86.h
	@echo "#endif"                         >> windows_x86.h

windows_x86::
	@echo "[+] Compiling ${SDC_PROG}.exe"
	${CC_RC} -o ${RC}.o ${RC}
	@make SDC_OBJS='${SDC_OBJS} win32getpass.o ${RC}.o' \
		CC=${WIN32_CC} CFLAGS='${CFLAGS} \
		-DWINDOWS -DWINDOWS_X86	-DDEFAULT_TARGET_ARCH=\"windows_x86\"' \
		SDC_PROG='${SDC_PROG}.exe' sdc 


# OpenBSD x86
selfdecrypt_openbsd_x86::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='${CFLAGS} -DOPENBSD -DOPENBSD_X86' selfdecrypt > /dev/null
	@echo "[+] Generating openbsd_x86.h"
	@echo "#ifndef _SDC_OPENBSD_X86"          > openbsd_x86.h
	@echo "#define _SDC_OPENBSD_X86"         >> openbsd_x86.h
	@echo                                  >> openbsd_x86.h
	@echo "static char openbsd_x86_elf[] = " >> openbsd_x86.h
	@./genhex.pl ${SELF_PROG} 15            >> openbsd_x86.h
	@echo ";"                              >> openbsd_x86.h
	@echo                                  >> openbsd_x86.h
	@echo "#endif"                         >> openbsd_x86.h

openbsd_x86::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='${CFLAGS} \
		-DOPENBSD -DOPENBSD_X86	-DDEFAULT_TARGET_ARCH=\"openbsd_x86\"' sdc > /dev/null

# OpenBSD arm
selfdecrypt_openbsd_arm::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='${CFLAGS} -DOPENBSD -DOPENBSD_ARM' selfdecrypt > /dev/null
	@echo "[+] Generating openbsd_arm.h"
	@echo "#ifndef _SDC_OPENBSD_ARM"          > openbsd_arm.h
	@echo "#define _SDC_OPENBSD_ARM"         >> openbsd_arm.h
	@echo                                  >> openbsd_arm.h
	@echo "static char openbsd_arm_elf[] = " >> openbsd_arm.h
	@./genhex.pl ${SELF_PROG} 15            >> openbsd_arm.h
	@echo ";"                              >> openbsd_arm.h
	@echo                                  >> openbsd_arm.h
	@echo "#endif"                         >> openbsd_arm.h

openbsd_arm::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='${CFLAGS} \
		-DOPENBSD -DOPENBSD_ARM	-DDEFAULT_TARGET_ARCH=\"openbsd_arm\"' sdc > /dev/null

# NetBSD x86
selfdecrypt_netbsd_x86::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='${CFLAGS} -DOPENBSD -DNETBSD_X86' selfdecrypt > /dev/null
	@echo "[+] Generating netbsd_x86.h"
	@echo "#ifndef _SDC_NETBSD_X86"          > netbsd_x86.h
	@echo "#define _SDC_NETBSD_X86"         >> netbsd_x86.h
	@echo                                  >> netbsd_x86.h
	@echo "static char netbsd_x86_elf[] = " >> netbsd_x86.h
	@./genhex.pl ${SELF_PROG} 15            >> netbsd_x86.h
	@echo ";"                              >> netbsd_x86.h
	@echo                                  >> netbsd_x86.h
	@echo "#endif"                         >> netbsd_x86.h

netbsd_x86::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='${CFLAGS} \
		-DNETBSD -DNETBSD_X86	-DDEFAULT_TARGET_ARCH=\"netbsd_x86\"' sdc > /dev/null


# OpenBSD sparc
selfdecrypt_openbsd_sparc64::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='${CFLAGS} -DOPENBSD -DOPENBSD_SPARC -DSDC_BIG_ENDIAN' \
		selfdecrypt > /dev/null
	@echo "[+] Generating openbsd_sparc64.h"
	@echo "#ifndef _SDC_OPENBSD_SPARC64"          > openbsd_sparc64.h
	@echo "#define _SDC_OPENBSD_SPARC64"         >> openbsd_sparc64.h
	@echo                                  >> openbsd_sparc64.h
	@echo "static char openbsd_sparc64_elf[] = " >> openbsd_sparc64.h
	@./genhex.pl ./${SELF_PROG} 15            >> openbsd_sparc64.h
	@echo ";"                              >> openbsd_sparc64.h
	@echo                                  >> openbsd_sparc64.h
	@echo "#endif"                         >> openbsd_sparc64.h

openbsd_sparc64::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='${CFLAGS} -DSDC_BIG_ENDIAN\
		-DOPENBSD -DOPENBSD_SPARC -DDEFAULT_TARGET_ARCH=\"openbsd_sparc64\"' sdc > /dev/null


# FreeBSD x86
selfdecrypt_freebsd_x86::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='${CFLAGS} -DFREEBSD -DFREEBSD_X86' selfdecrypt > /dev/null
	@echo "[+] Generating freebsd_x86.h"
	@echo "#ifndef _SDC_FREEBSD_X86"          > freebsd_x86.h
	@echo "#define _SDC_FREEBSD_X86"         >> freebsd_x86.h
	@echo                                  >> freebsd_x86.h
	@echo "static char freebsd_x86_elf[] = " >> freebsd_x86.h
	@./genhex.pl ${SELF_PROG} 15            >> freebsd_x86.h
	@echo ";"                              >> freebsd_x86.h
	@echo                                  >> freebsd_x86.h
	@echo "#endif"                         >> freebsd_x86.h

freebsd_x86::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='${CFLAGS} \
		-DSOLARIS -DSOLARIS_X86	-DDEFAULT_TARGET_ARCH=\"freebsd_x86\"' sdc > /dev/null

# Solaris Sparc (No static binaries)
selfdecrypt_solaris_sparc::
	@echo "[+] Compiling ${SELF_PROG}"
	@make SELF_OBJS='${SELF_OBJS} ${ELF_OBJS}' \
		CFLAGS='-Wall -O -fomit-frame-pointer -pipe -s\
		-DSOLARIS -DSOLARIS_SPARC -DSDC_BIG_ENDIAN' selfdecrypt > /dev/null
	@echo "[+] Generating solaris_sparc.h"
	@echo "#ifndef _SDC_SOLARIS_SPARC"          > solaris_sparc.h
	@echo "#define _SDC_SOLARIS_SPARC"         >> solaris_sparc.h
	@echo                                  >> solaris_sparc.h
	@echo "static char solaris_sparc_elf[] = " >> solaris_sparc.h
	@./genhex.pl ${SELF_PROG} 15            >> solaris_sparc.h
	@echo ";"                              >> solaris_sparc.h
	@echo                                  >> solaris_sparc.h
	@echo "#endif"                         >> solaris_sparc.h

solaris_sparc::
	@echo "[+] Compiling ${SDC_PROG}"
	@make SDC_OBJS='${SDC_OBJS}' CFLAGS='-Wall -O -fomit-frame-pointer -pipe -s  \
		-DFREEBSD -DSOLARIS_SPARC -DSDC_BIG_ENDIAN -DDEFAULT_TARGET_ARCH=\"solaris_sparc\"' sdc > /dev/null

install::
	cp -f ${SDC_PROG} /usr/bin/${SDC_PROG}
	cp -f ${SDC_MAN} /usr/share/man/man1/${SDC_MAN}
	chmod 755 /usr/bin/${SDC_PROG}

uninstall::
	rm -f /usr/bin/${SDC_PROG}
	rm -f /usr/share/man/man1/${SDC_MAN}


clean:
	rm -f ${SDC_OBJS} ${SELF_OBJS} *.o
	rm -f ${ELF_OBJS} ${PECOFF_OBJS}
	rm -f ${SDC_PROG} ${SELF_PROG} ${SDC_PROG}.exe ${SELF_PROG}.exe *.core
	rm -f linux_x86.h
	rm -f windows_x86.h
	rm -f openbsd_x86.h
	rm -f openbsd_sparc64.h
	rm -f freebsd_x86.h
	rm -f solaris_sparc.h

