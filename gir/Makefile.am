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

include $(INTROSPECTION_MAKEFILE)

#
# Source headers
#

GIR_NAMESPACE=Scp
GIR_VERSION=${PACKAGE_VERSION_MAYOR}.${PACKAGE_VERSION_MINOR}

GIR_PKG=\
	gio-2.0 \
	glib-2.0 \
	gmodule-2.0 \
	gobject-2.0 \
	gtk+-3.0 \
	webkit2gtk-4.0 \
	$(VOID)

GIR_INC=\
	Gio-2.0 \
	GLib-2.0 \
	GModule-2.0 \
	GObject-2.0 \
	Gtk-3.0 \
	$(VOID)

#
# .gir file
#

Scp-${GIR_VERSION}.gir: sources.list ${top_srcdir}/src/libscpbrowser.la
	${INTROSPECTION_SCANNER} \
		--output $@ \
		--nsversion "${GIR_VERSION}" \
		--library "${top_srcdir}/src/libscpbrowser.la" \
		--warn-all \
		--namespace Scp \
		--cflags-begin \
			-I${top_srcdir}/src/ \
		--cflags-end \
		--libtool "$(LIBTOOL)" \
		--sources-top-dirs=${top_srcdir} \
		--include-first-in-src="config.h" \
		$(addprefix --pkg , ${GIR_PKG}) \
		$(addprefix --include , ${GIR_INC}) \
		--filelist sources.list \
		$(VOID)

#
# *.typelib file
#

TYPELIB_FILES=\
	Scp-${GIR_VERSION}.typelib \
	$(VOID)

$(TYPELIB_FILES): $(TYPELIB_FILES:.typelib=.gir)
	$(INTROSPECTION_COMPILER) --output $@ $<

#
# GObject introspection data
#

gir_DATA=\
	$(TYPELIB_FILES) \
	$(VOID)
