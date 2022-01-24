# Copyright 2021-2023 MarcosHCK
# This file is part of halckulator.
#
# halckulator is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# halckulator is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with halckulator. If not, see <http://www.gnu.org/licenses/>.
#

#
# Some variables
#

PACKAGE_NAME=@PACKAGE_NAME@
PACKAGE_RELEASE=@PACKAGE_RELEASE@
PACKAGE_VERSION_MAYOR=@PACKAGE_VERSION_MAYOR@
PACKAGE_VERSION_MINOR=@PACKAGE_VERSION_MINOR@
PACKAGE_VERSION_MICRO=@PACKAGE_VERSION_MICRO@
PACKAGE_VERSION_BUILD=@PACKAGE_VERSION_BUILD@

EXTRA_DIST=
VOID=

#
# Resources
#

RESOURCES=\
	$(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies resources.gresources.xml) \
	$(VOID)

EXTRA_DIST+=\
	$(RESOURCES) \
	$(VOID)
	
CLEANFILES=\
	resources.c \
	resources.h \
	$(VOID)

#
# *.gresources.xml to *.c and *.h make target
#

resources.c: resources.gresources.xml $(RESOURCES)
	$(GLIB_COMPILE_RESOURCES) --target=$@ --generate-source $<

resources.h: resources.gresources.xml $(RESOURCES)
	$(GLIB_COMPILE_RESOURCES) --target=$@ --generate-header $<

#
# Binaries and libraries
#

noinst_LTLIBRARIES=libbundle.la

noinst_DATA=\
	resources.h \
	$(VOID)

libbundle_la_SOURCES=\
	resources.c \
	$(VOID)
libbundle_la_CFLAGS=\
	$(GIO_CFLAGS) \
	$(VOID)
libbundle_la_LIBADD=\
	$(GIO_LIBS) \
	$(VOID)
libbundle_la_LDFLAGS=\
	-static \
	$(VOID)
