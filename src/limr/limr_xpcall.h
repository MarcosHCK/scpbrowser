/* Copyright 2021-2025 MarcosHCK
 * This file is part of libLimr.
 *
 * libLimr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libLimr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libLimr.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __LIMR_XPCALL__
#define __LIMR_XPCALL__ 1
#include <glib.h>

#define LIMR_XPCALL_ERROR (limr_xpcall_error_quark ())

typedef enum
{
  LIMR_XPCALL_ERROR_FAILED,
  LIMR_XPCALL_ERROR_RUN,
  LIMR_XPCALL_ERROR_RECURSIVE,
  LIMR_XPCALL_ERROR_MEMORY,
} LimrXpcallError;

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

GQuark
limr_xpcall_error_quark (void) G_GNUC_CONST;

G_GNUC_INTERNAL
int
_limr_throwrap (lua_State* L);
G_GNUC_INTERNAL
int
_limr_xpcall (lua_State* L, int nargs, int nresults, GError** error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_XPCALL__
