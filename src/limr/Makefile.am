# Copyright 2021-2025 MarcosHCK
# This file is part of scpbrowser.
#
# scpbrowser is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# scpbrowser is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with scpbrowser. If not, see <http://www.gnu.org/licenses/>.
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
  limr_state_patch.h \
  limr_vm_data.h \
  $(VOID)

#
# Binaries and libraries
#

pkglib_LTLIBRARIES=liblimr.la

liblimr_la_SOURCES=\
  limr_state_patch.c \
  limr_state.vala \
  limr_vm_data.c \
  $(VOID)
liblimr_la_CFLAGS=\
  $(GIO_CFLAGS) \
  $(GLIB_CFLAGS) \
  $(GMODULE_CFLAGS) \
  $(GOBJECT_CFLAGS) \
  $(LUA_CFLAGS) \
  -DG_LOG_DOMAIN=\"LibLimr\" \
  -DABSTOPBUILDDIR=\"${abs_top_builddir}\" \
  -DGAPPNAME=\"org.hck.${PACKAGE_NAME}\" \
  -DGRESNAME=\"/org/hck/${PACKAGE_NAME}\" \
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
  -D DEBUG=${DEBUG} \
  -D DEVELOPER=${DEVELOPER} \
  $(VOID)
