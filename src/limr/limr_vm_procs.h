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
#ifndef __LIMR_VM_PROCS__
#define __LIMR_VM_PROCS__ 1
#include <glib.h>

#define IF_LUA_ERROR(L, code) \
  if(G_UNLIKELY \
    (code != LUA_OK \
      && lua_type(L, -1) != LUA_TSTRING)) \
  { \
    if (code == LUA_ERRMEM) \
    { \
      g_critical \
      ("(%s: %i): out of memory", \
       G_STRFUNC, \
       __LINE__); \
      g_assert_not_reached(); \
    } \
    else \
    { \
      lua_pushfstring \
      (L, \
       "(%s: %i): wtf?", \
       G_STRFUNC, \
       __LINE__); \
    } \
  } else \
  if(G_UNLIKELY(code != LUA_OK))
#define THROW_GERROR(L, error) \
  G_STMT_START { \
    lua_pushstring(L, ((error))->message); \
    g_error_free(((error))); \
    lua_error(L); \
  } G_STMT_END

#define LEXECUTOR "__LIMR_EXECUTOR"

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/*
 * Procedures
 *
 */

G_GNUC_INTERNAL
int
limr_proc_print (lua_State* L);
G_GNUC_INTERNAL
int
limr_proc_printerr (lua_State* L);

/*
 * Entry point
 *
 */

G_GNUC_INTERNAL
int
limr_procs_emit (lua_State* L, GError** error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_VM_PROCS__
