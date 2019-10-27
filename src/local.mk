# Make ng-coreutils programs.                             -*-Makefile-*-
# This is included by the top-level Makefile.am.

## Copyright (C) 1990-2019 Free Software Foundation, Inc.

## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <https://www.gnu.org/licenses/>.

AM_CFLAGS = -Wall -Wextra -Wno-pointer-sign -funsigned-char -std=gnu99 -D_GNU_SOURCE=1 -pipe $(WERROR_CFLAGS)

bin_PROGRAMS = src/pc

noinst_HEADERS = \
  src/system.h

CLEANFILES += $(SCRIPTS)

noinst_LIBRARIES += src/libver.a
nodist_src_libver_a_SOURCES = src/version.c src/version.h

# Tell the linker to omit references to unused shared libraries.
AM_LDFLAGS = $(IGNORE_UNUSED_LIBRARIES_CFLAGS) -export-dynamic

# Sometimes, the expansion of $(LIBINTL) includes -lc which may
# include modules defining variables like 'optind', so libgnu.a
# must precede $(LIBINTL) in order to ensure we use GNU getopt.
# But libgnu.a must also follow $(LIBINTL), since libintl uses
# replacement functions defined in libgnu.a.
LDADD = src/libver.a src/thread/libthread.a lib/libgnu.a $(LIBINTL) lib/libgnu.a

# Get the release year from lib/version-etc.c.
RELEASE_YEAR = \
  `sed -n '/.*COPYRIGHT_YEAR = \([0-9][0-9][0-9][0-9]\) };/s//\1/p' \
    $(top_srcdir)/lib/version-etc.c`

# '-d' option is necessary here to create parser.h
src/parse.c: src/parse.y
	$(AM_V_GEN)bison -d -y -Wnone -o $@ $^
src/lex.c: src/lex.l
	$(AM_V_GEN)flex -o $@ $^
DISTCLEANFILES += src/lex.c src/parse.c src/parse.h

src_pc_LDADD   = $(LDADD)
src_pc_CFLAGS  = -DCOPYRIGHT_YEAR=\"$(RELEASE_YEAR)\" $(AM_CFLAGS) $(AM_CPPFLAGS)
# src/parser.c must be first here
src_pc_SOURCES = src/parse.c src/lex.c src/stack.c src/pc.c src/main.c src/real.c src/util.c # src/parser/parser.c src/parser/util.c src/parser/execute.c

BUILT_SOURCES += src/version.c
src/version.c: Makefile
	$(AM_V_GEN)rm -f $@
	$(AM_V_at)${MKDIR_P} src
	$(AM_V_at)printf '\nconst char *Version = "$(PACKAGE_VERSION)";\n' > $@t
	$(AM_V_at)chmod a-w $@t
	$(AM_V_at)mv $@t $@

BUILT_SOURCES += src/version.h
src/version.h: Makefile
	$(AM_V_GEN)rm -f $@
	$(AM_V_at)${MKDIR_P} src
	$(AM_V_at)printf '\n' > $@t
	$(AM_V_at)printf '#ifndef _VERSION_H\n' >> $@t
	$(AM_V_at)printf '#define _VERSION_H 1\n\n' >> $@t
	$(AM_V_at)printf 'extern const char *Version;\n\n' >> $@t
	$(AM_V_at)printf '#endif /* _VERSION_H */\n' >> $@t
	$(AM_V_at)chmod a-w $@t
	$(AM_V_at)mv $@t $@

DISTCLEANFILES += src/version.c src/version.h
MAINTAINERCLEANFILES += $(BUILT_SOURCES)

INSTALL = install -c
