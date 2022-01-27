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

# Keep sync with settings/Makefile.am
schemasdir=${pkgdatadir}/schemas

# Keep sync with src/Makefile.am
girdir=${pkgdatadir}/gir/

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
  web \
  vapi \
  $(VOID)

#
# C headers
#

EXTRA_DIST+=\
  scp_browser_internl.h \
  scp_browser.h \
  $(VOID)

#
# Binaries and libraries
#

bin_PROGRAMS=scpbrowser
pkglib_LTLIBRARIES=libscpbrowser.la

scpbrowser_SOURCES=\
  scp_main.c \
  $(VOID)
scpbrowser_CFLAGS=\
  $(GIO_CFLAGS) \
  $(GLIB_CFLAGS) \
  $(GMODULE_CFLAGS) \
  $(GOBJECT_CFLAGS) \
  $(GTK_CFLAGS) \
  $(LIBSASS_CFLAGS) \
  $(WEBKIT2_CFLAGS) \
  -DLOCALEDIR=\"${localedir}\" \
  -DMODULESDIR=\"${modulesdir}\" \
  -DSCHEMASDIR=\"${schemasdir}\" \
  -DGAPPNAME=\"org.hck.${PACKAGE_NAME}\" \
  -DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\" \
  -DGEXTNAME=\"org-hck-${PACKAGE_NAME}\" \
  -DGRESNAME=\"/org/hck/${PACKAGE_NAME}\" \
  $(VOID)
scpbrowser_LDADD=\
  $(GIO_LIBS) \
  $(GLIB_LIBS) \
  $(GMODULE_LIBS) \
  $(GOBJECT_LIBS) \
  $(GTK_LIBS) \
  $(LIBSASS_LIBS) \
  $(WEBKIT2_LIBS) \
  libscpbrowser.la \
  web/libbundle.la \
  $(VOID)

libscpbrowser_la_SOURCES=\
  scp_application.vala \
  scp_browser_scss.c \
  scp_browser.c \
  scp_message.vala \
  scp_notebook_page.vala \
  scp_notebook_tab.vala \
  scp_window.vala \
  $(VOID)
libscpbrowser_la_CFLAGS=\
  $(GIO_CFLAGS) \
  $(GLIB_CFLAGS) \
  $(GMODULE_CFLAGS) \
  $(GOBJECT_CFLAGS) \
  $(GTK_CFLAGS) \
  $(LIBSASS_CFLAGS) \
  $(WEBKIT2_CFLAGS) \
  -DLOCALEDIR=\"${localedir}\" \
  -DMODULESDIR=\"${modulesdir}\" \
  -DSCHEMASDIR=\"${schemasdir}\" \
  -DGAPPNAME=\"org.hck.${PACKAGE_NAME}\" \
  -DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\" \
  -DGEXTNAME=\"org-hck-${PACKAGE_NAME}\" \
  -DGRESNAME=\"/org/hck/${PACKAGE_NAME}\" \
  -D__LIBSCPBROWSER_INSIDE__=1 \
  $(VOID)
libscpbrowser_la_LIBADD=\
  $(GIO_LIBS) \
  $(GLIB_LIBS) \
  $(GMODULE_LIBS) \
  $(GOBJECT_LIBS) \
  $(GTK_LIBS) \
  $(LIBSASS_LIBS) \
  $(WEBKIT2_LIBS) \
  resources/libbundle.la \
  $(VOID)
libscpbrowser_la_LDFLAGS=\
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
  --header libscpbrowser.h \
  --library libscpbrowser \
  --pkg config \
  --pkg scp \
  -D DEBUG=${DEBUG} \
  -D DEVELOPER=${DEVELOPER} \
  --gresources "${top_builddir}/src/resources/resources.gresources.xml" \
  $(VOID)
