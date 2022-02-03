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
#ifndef __LIMR_STREAM__
#define __LIMR_STREAM__ 1
#include <gio/gio.h>

#define _IO_STREAM "__LIMR_IO_STREAM"

#define WRONG_OPERATION(L, opr) \
  G_STMT_START { \
    lua_pushfstring \
    (L, \
     "stream doesn't support '%s' operation", \
     ((opr))); \
    lua_error (L); \
  } G_STMT_END
#define PUSH_GERROR(L,error) \
  G_STMT_START { \
    lua_pushstring (L, ((error))->message); \
    g_error_free (((error))); \
  } G_STMT_END
#define THROW_GERROR(L, error) \
  G_STMT_START { \
    PUSH_GERROR (L, (error)); \
    lua_error (L); \
  } G_STMT_END

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

G_GNUC_INTERNAL
void
_limr_stream_create_meta (lua_State* L);
void
luaL_pushstream (lua_State* L, gpointer stream);
gpointer
luaL_tostream (lua_State* L, int idx);
gpointer
luaL_checkstream (lua_State* L, int idx);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_STREAM__
