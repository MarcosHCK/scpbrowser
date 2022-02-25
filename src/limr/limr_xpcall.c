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
#include <config.h>
#include <limr_xpcall.h>

G_GNUC_INTERNAL G_DEFINE_QUARK
(limr-xpcall-error-quark,
 limr_xpcall_error);

G_GNUC_NORETURN
G_GNUC_INTERNAL
void
_limr_throwgerror (lua_State* L, GError* error)
{
  lua_pushfstring
  (L,
   "%s: %i: %s",
   g_quark_to_string (error->domain),
   error->code,
   error->message);
  g_error_free (error);
  lua_error (L);
for (;;);
}

G_GNUC_INTERNAL
int
_limr_throwrap (lua_State* L)
{
  const gchar* err = NULL;

  err = lua_tostring (L, 1);
  if G_UNLIKELY (err == NULL)
  {
    if(luaL_callmeta (L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING)
      err = lua_tostring (L, -1);
    else
      err = lua_pushfstring (L, "(error object is a %s value)", lua_typename(L, 1));
  }

  luaL_traceback (L, L, err, 1);
return 1;
}

G_GNUC_INTERNAL
int
_limr_xpcall (lua_State* L, int nargs, int nresults, GError** error)
{
  int top = lua_gettop (L),
      lower = top - nargs - 1;
  int result = 0;

  lua_pushcfunction (L, _limr_throwrap);
  lua_insert (L, 1);

  result =
  lua_pcall (L, nargs, nresults, 1);
  lua_remove (L, 1);

  switch (result)
  {
  case LUA_ERRRUN:
    g_set_error
    (error,
     LIMR_XPCALL_ERROR,
     LIMR_XPCALL_ERROR_RUN,
     "%s",
     lua_tostring (L, -1));
    lua_pop (L, 1);
#if DEBUG == 1
    g_assert (lua_gettop (L) == lower);
#endif // DEBUG
    return -1;
  case LUA_ERRERR:
    g_set_error
    (error,
     LIMR_XPCALL_ERROR,
     LIMR_XPCALL_ERROR_RECURSIVE,
     "recursive error: %s",
     lua_tostring (L, -1));
    lua_pop (L, 1);
#if DEBUG == 1
    g_assert (lua_gettop (L) == lower);
#endif // DEBUG
    return -1;
  case LUA_ERRMEM:
    g_set_error_literal
    (error,
     LIMR_XPCALL_ERROR,
     LIMR_XPCALL_ERROR_MEMORY,
     "Lua thread out of memory");
    g_warning ("Lua thread out of memory");
    lua_settop (L, lower);
#if DEBUG == 1
    g_assert (lua_gettop (L) == lower);
#endif // DEBUG
    return -1;
  default:
    g_assert (result == LUA_OK);
#if DEBUG == 1
    if (nresults != LUA_MULTRET)
    g_assert (lua_gettop (L) == lower + nresults);
#endif // DEBUG
    return result;
  }
return -1;
}
