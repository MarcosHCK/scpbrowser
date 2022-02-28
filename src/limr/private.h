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
#ifndef __LIBLIMR_PRIVATE__
#define __LIBLIMR_PRIVATE__ 1
#include <gio/gio.h>
#include <gmodule.h>

#ifndef __LIBLIMR_INSIDE__
  #error "Include 'liblimr.h' instead"
#endif // !__LIBLIMR_INSIDE__

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

G_GNUC_INTERNAL
gboolean
lua_istrue (lua_State* L, int idx);
G_GNUC_INTERNAL
const gchar*
g_file_peek_uri (GFile* file);

#define LIBRARY           "__LIMR_LIBRARY"
#define MACROS            "__LIMR_MACROS"
#define MACROS_PRELOAD    "__LIMR_MACROS_PRELOAD"
#define MACROS_SEARCHERS  "__LIMR_MACROS_SEARCHERS"

#define SKETCH_TABLE      "__LIMR_SKETCH_TABLE"
#define SKETCH_SLICES     "__LIMR_SKETCH_SLICES"

#define lua_istrue(L,idx) (lua_istrue ((L),(idx)))
#define lua_isfalse(L,idx) (!lua_istrue ((L),(idx)))

G_MODULE_EXPORT
int
luaopen_liblimr (lua_State* L);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIBLIMR_PRIVATE__
