# Copyright 2021-2025 MarcosHCK
# This file is part of libLimr.
#
# libLimr is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libLimr is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libLimr. If not, see <http://www.gnu.org/licenses/>.
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
# Subdirectories
#

SUBDIRS=\
  resources \
  $(VOID)

#
# C headers
#

EXTRA_DIST+=\
  vapi/config.deps \
  vapi/config.vapi \
  vapi/patch.deps \
  vapi/patch.vapi \
  vapi/stream.deps \
  vapi/stream.vapi \
  vapi/xpcall.deps \
  vapi/xpcall.vapi \
  liblimr.h \
  limr_patch.h \
  limr_stream.h \
  limr_xpcall.h \
  $(VOID)

#
# Binaries and libraries
#

noinst_LTLIBRARIES=liblimr.la

liblimr_la_SOURCES=\
  liblimr.c \
  limr_cpatch.c \
  limr_host.vala \
  limr_parser.vala \
  limr_patch.c \
  limr_stream.c \
  limr_xpcall.c \
  $(VOID)
liblimr_la_CFLAGS=\
  $(GIO_CFLAGS) \
  $(GLIB_CFLAGS) \
  $(GMODULE_CFLAGS) \
  $(GOBJECT_CFLAGS) \
  $(LUA_CFLAGS) \
  -DG_LOG_DOMAIN=\"Limr\" \
  -DABSTOPBUILDDIR=\"${abs_top_builddir}\" \
  -DGRESNAME=\"/org/hck/${PACKAGE_NAME}/limr\" \
  -D__LIBLIMR_INSIDE__=1 \
  $(VOID)
liblimr_la_LIBADD=\
  $(GIO_LIBS) \
  $(GLIB_LIBS) \
  $(GMODULE_LIBS) \
  $(GOBJECT_LIBS) \
  $(LUA_LIBS) \
  resources/libbundle.la \
  $(VOID)
liblimr_la_LDFLAGS=\
  -rpath ${pkglibdir} \
	-release ${PACKAGE_RELEASE} \
	-version-info ${PACKAGE_VERSION_MAYOR}:${PACKAGE_VERSION_MINOR}:${PACKAGE_VERSION_MICRO} \
	$(VOID)

#
# Vala
#

AM_VALAFLAGS=\
  --vapidir=vapi/ \
  --vapi-comments \
  --hide-internal \
  --abi-stability \
  --header liblimr.h \
  --library liblimr \
  --pkg config \
  --pkg patch \
  --pkg xpcall \
  -D DEBUG=${DEBUG} \
  -D DEVELOPER=${DEVELOPER} \
  $(VOID)
