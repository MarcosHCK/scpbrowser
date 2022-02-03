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
AC_PREREQ([2.69])

m4_define([p_VERSION], [0.1.0.0])
m4_define([p_RELEASE], [alpha])

AC_INIT([scpbrowser], [p_VERSION], [scpbrowser.bugs@hcktm.com], [scpbrowser], [https://www.hcktm.com/scpbrowser/])
AC_CONFIG_SRCDIR([src/scp_main.c])
AC_LANG([C])

#
# Configure aclocal path
#
AC_CONFIG_MACRO_DIRS([m4])

#
# Setup default cross-compiling
# environment mechanics
#
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

#
# Automake
#
AM_INIT_AUTOMAKE

#
# Extract mayor, minor and micro version
# numbers from version string
# Also define release type
#
m4_define([vernum_pattern],[\(@<:@0-9@:>@*\)])

AC_SUBST([PACKAGE_VERSION_MAYOR])
PACKAGE_VERSION_MAYOR=$(echo "$PACKAGE_VERSION" | sed -e 's/vernum_pattern\.vernum_pattern\.vernum_pattern\.vernum_pattern/\1/')
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MAYOR], ["$PACKAGE_VERSION_MAYOR"], [Version mayor number])
AC_SUBST([PACKAGE_VERSION_MINOR])
PACKAGE_VERSION_MINOR=$(echo "$PACKAGE_VERSION" | sed -e 's/vernum_pattern\.vernum_pattern\.vernum_pattern\.vernum_pattern/\2/')
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MINOR], ["$PACKAGE_VERSION_MINOR"], [Version minor number])
AC_SUBST([PACKAGE_VERSION_MICRO])
PACKAGE_VERSION_MICRO=$(echo "$PACKAGE_VERSION" | sed -e 's/vernum_pattern\.vernum_pattern\.vernum_pattern\.vernum_pattern/\3/')
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MICRO], ["$PACKAGE_VERSION_MICRO"], [Version micro number])
AC_SUBST([PACKAGE_VERSION_BUILD])
PACKAGE_VERSION_BUILD=$(echo "$PACKAGE_VERSION" | sed -e 's/vernum_pattern\.vernum_pattern\.vernum_pattern\.vernum_pattern/\4/')
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_BUILD], ["$PACKAGE_VERSION_BUILD"], [Version build number])

AC_DEFINE([PACKAGE_RELEASE], ["p_RELEASE"], [Release tag])
AC_SUBST([PACKAGE_RELEASE], [p_RELEASE])

#
#  Checks for programs we'll
# need to build
#

LT_PREREQ([2.4.6])
LT_INIT

AM_PROG_AS
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_VALAC

PKG_PROG_PKG_CONFIG
GOBJECT_INTROSPECTION_REQUIRE([1.68.0])

AC_CHECK_PROG([GLIB_GENMARSHAL], [glib-genmarshal], [glib-genmarshal], [no])
if test "x$GLIB_GENMARSHAL" = "xno"; then
  AC_MSG_FAILURE([glib-genmarshal not found our your system])
fi
AC_CHECK_PROG([GLIB_COMPILE_RESOURCES], [glib-compile-resources], [glib-compile-resources], [no])
if test "x$GLIB_COMPILE_RESOURCES" = "xno"; then
  AC_MSG_FAILURE([glib-compile-resources not found our your system])
fi
AC_CHECK_PROG([GLIB_COMPILE_SCHEMAS], [glib-compile-schemas], [glib-compile-schemas], [no])
if test "x$GLIB_COMPILE_SCHEMAS" = "xno"; then
  AC_MSG_FAILURE([glib-compile-schemas not found our your system])
fi
AC_CHECK_PROG([GLIB_MKENUMS], [glib-mkenums], [glib-mkenums], [no])
if test "x$GLIB_MKENUMS" = "xno"; then
  AC_MSG_FAILURE([glib-mkenums not found our your system])
fi

#
# Gettext stuff
#

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.19.8])
AM_CONDITIONAL([USE_NLS], [test "x${USE_NLS}" = "xyes"])

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

#
# Custom build options
#

debugging="no"
developer="no"

AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [Enable debug object output @<:@default=no@:>@])],
              [if test "x$enableval" != "xno"; then
                debugging="yes"
               fi
              ])

if test "x$debugging" = "xno"; then
  AC_DEFINE([DEBUG], [0], [Debugging disabled])
  AC_SUBST([DEBUG], [0])
else
  AC_DEFINE([DEBUG], [1], [Debugging enabled])
  AC_SUBST([DEBUG], [1])
fi

AC_ARG_ENABLE([developer],
              [AS_HELP_STRING([--enable-developer], [Enable developer features @<:@default=no@:>@])],
              [if test "x$enableval" != "xno"; then
                developer="yes"
               fi
              ])

if test "x$developer" = "xno"; then
  AC_DEFINE([DEVELOPER], [0], [Developer feature disabled])
  AC_SUBST([DEVELOPER], [0])
else
  AC_DEFINE([DEVELOPER], [1], [Developer feature enabled])
  AC_SUBST([DEVELOPER], [1])
fi

#
# Check for libraries using pkg-config
# Note: keep sorted alphabetically 
#

PKG_CHECK_MODULES([GIO], [gio-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GMODULE], [gmodule-2.0])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])
PKG_CHECK_MODULES([GTK], [gtk+-3.0])
PKG_CHECK_MODULES([LIBSASS], [libsass])
PKG_CHECK_MODULES([LIBSOUP], [libsoup-2.4])
PKG_CHECK_MODULES([LUA], [luajit lua5.1 lua51])
PKG_CHECK_MODULES([WEBKIT2], [webkit2gtk-4.0])

#
# Check for libraries
#

#
# Checks for header files.
#

#
# Checks for typedefs, structures, and compiler characteristics.
#

AC_C_INLINE

#
# Checks for library functions.
#

#
# Prepare output
#

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
gir/Makefile
po/Makefile.in
settings/Makefile
src/extension/Makefile
src/limr/limr_io/Makefile
src/limr/vapi/Makefile
src/limr/Makefile
src/resources/Makefile
src/vapi/Makefile
src/web/Makefile
src/Makefile
Makefile
])

AC_OUTPUT
