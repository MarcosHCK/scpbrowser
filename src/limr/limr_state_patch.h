/* Copyright 2021-2025 MarcosHCK
 * This file is part of liblimr.
 *
 * liblimr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblimr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liblimr.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __LIMR_STATE_PATCH__
#define __LIMR_STATE_PATCH__ 1
#include <gio/gio.h>

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

G_GNUC_INTERNAL
int
limr_patch_ref_string (lua_State* L);
G_GNUC_INTERNAL
lua_State*
limr_patch_create_vm (GError** error);
G_GNUC_INTERNAL
int
limr_patch_add_string (lua_State* L, const gchar* string_, gssize length);
G_GNUC_INTERNAL
int
limr_patch_compile (lua_State* L, const gchar* source, gssize length, GError** error);
G_GNUC_INTERNAL
int
limr_patch_execute (lua_State* L, GOutputStream* stream, GCancellable* cancellable, GError** error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_STATE_PATCH__
