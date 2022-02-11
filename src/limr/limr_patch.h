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
#ifndef __LIMR_PATCH__
#define __LIMR_PATCH__ 1
#include <gio/gio.h>
#include <private.h>

#if __cplusplus
extern "C" {
#endif // __cplusplus

#define LLIBRARY "__LIMR_LIBRARY"

G_GNUC_INTERNAL
GInputStream*
limr_patch_get_stdin ();
G_GNUC_INTERNAL
GOutputStream*
limr_patch_get_stdout ();
G_GNUC_INTERNAL
GOutputStream*
limr_patch_get_stderr ();

G_GNUC_INTERNAL
const char*
_limr_preader (lua_State* L, lua_Reader reader, gpointer target, size_t* length, GError** error);
G_GNUC_INTERNAL
int
_limr_loadx (lua_State* L, lua_Reader reader, void* dt, const gchar* chunkname, const gchar* mode);
G_GNUC_INTERNAL
int
_limr_load (lua_State* L, lua_Reader reader, void* dt, const gchar* chunkname);
G_GNUC_INTERNAL
GBytes*
_limr_run_sketch (lua_State* L, GError** error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_PATCH__
