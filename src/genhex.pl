#!/usr/bin/env perl

#
#  File: genhex.pl
#  Copyright (c) 2006 Claes M. Nyberg <cmn@signedness.org>
#  All rights reserved, all wrongs reversed.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. The name of author may not be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
#  THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# $Id: genhex.pl,v 1.4 2006-05-01 15:51:05 cmn Exp $
#

#
# Genereate hex string from file.
# Copyright (C) Claes M Nyberg 2006 <cmn@signedness.org>
# $Id: genhex.pl,v 1.4 2006-05-01 15:51:05 cmn Exp $
#

use strict;

sub usage()
{
	print("Usage: $0 <file> <len>\n");
	exit(1);
}


# Convert file into hex-string
# $_[0] - The file to translate
# $_[1] - The maximum number of characters per line
sub genhex($$)
{
	my $c;
	my $count = 0;

	open(IN, '<', $_[0])	or
		die("Failed to open $_[0]: $!");
	
	# Binary mode
	binmode(IN);

	while (sysread(IN, $c, 1)) {
		$count++;

		# Special case, number of characters per line is 1
		(((($count % int($_[1])) == 1) and ($count > 1)) or 
			((int($_[1]) == 1)) and ($count > 1)) and
				print("\n");

		# New line
		((($count % int($_[1])) == 1) or (int($_[1]) == 1)) and
			 print("\"");

		printf("\\x%02x", ord($c));

		# Last character in line
		(($count % int($_[1])) == 0) and
			print("\"");
	}

	(($count % int($_[1])) != 0) and
		print("\"");
	close(IN);
}


sub main(@)
{
	my $file = shift;
	my $len = shift;

	((not defined($file)) or (not defined($len))) and 
		usage();
	genhex($file, $len);	
}


main(@ARGV);
