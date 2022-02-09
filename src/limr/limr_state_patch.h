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
#ifndef __LIMR_STATE_PATCH__
#define __LIMR_STATE_PATCH__ 1
#include <glib.h>

#define LENVIRON "__LIMR_ENVIRON"

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

G_GNUC_INTERNAL
int
_limr_vm_init (lua_State* L);
G_GNUC_INTERNAL
int
_limr_vm_setfenv (lua_State* L, int idx);
G_GNUC_NORETURN
G_GNUC_INTERNAL
void
_limr_vm_throwgerror (lua_State* L, GError* error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_STATE_PATCH__
