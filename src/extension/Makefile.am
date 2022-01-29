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
# C headers
#

EXTRA_DIST+=\
  $(VOID)

#
# Binaries and libraries
#

pkglib_LTLIBRARIES=libscpbrowser-extension.la

libscpbrowser_extension_la_SOURCES=\
  scp_extension.c \
  $(VOID)
libscpbrowser_extension_la_CFLAGS=\
  $(GIO_CFLAGS) \
  $(GLIB_CFLAGS) \
  $(GMODULE_CFLAGS) \
  $(GOBJECT_CFLAGS) \
  $(GTK_CFLAGS) \
  $(WEBKIT2_CFLAGS) \
  -DLOCALEDIR=\"${localedir}\" \
  -DMODULESDIR=\"${modulesdir}\" \
  -DSCHEMASDIR=\"${schemasdir}\" \
  -DPKGLIBDIR=\"${pkglibdir}\" \
  -DGAPPNAME=\"org.hck.${PACKAGE_NAME}\" \
  -DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\" \
  -DGEXTNAME=\"org-hck-${PACKAGE_NAME}\" \
  -DGRESNAME=\"/org/hck/${PACKAGE_NAME}\" \
  -D__LIBSCPBROWSER_INSIDE__=1 \
  $(VOID)
libscpbrowser_extension_la_LIBADD=\
  $(GIO_LIBS) \
  $(GLIB_LIBS) \
  $(GMODULE_LIBS) \
  $(GOBJECT_LIBS) \
  $(GTK_LIBS) \
  $(WEBKIT2_LIBS) \
  $(VOID)
libscpbrowser_extension_la_LDFLAGS=\
	-rpath ${pkglibdir} \
	-release ${PACKAGE_RELEASE} \
	-version-info ${PACKAGE_VERSION_MAYOR}:${PACKAGE_VERSION_MINOR}:${PACKAGE_VERSION_MICRO} \
	$(VOID)
