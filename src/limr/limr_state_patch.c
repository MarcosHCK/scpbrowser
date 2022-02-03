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
#include <config.h>
#include <limr_io.h>
#include <limr_stream.h>
#include <limr_state_patch.h>

#define LSTRINGS "__LIMR_STRINGS"
#define LSKETCH "__LIMR_SKETCH"

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

int
limr_patch_ref_string (lua_State* L)
{
  int idx, id;

  id = luaL_checkinteger (L, 1);
  idx = lua_upvalueindex (1);
  lua_pushinteger (L, id);
  lua_gettable (L, idx);
return 1;
}

G_GNUC_INTERNAL
lua_State*
limr_patch_create_vm (GError** error)
{
  lua_State* L =
  luaL_newstate ();

  luaL_openlibs (L);
  luaopen_limr_io (L);
  lua_setglobal (L, "io");

  lua_createtable (L, 0, 0);
  lua_pushvalue (L, -1);
  lua_setfield (L, LUA_REGISTRYINDEX, LSTRINGS);
  lua_pushcclosure (L, limr_patch_ref_string, 1);
  lua_setglobal (L, "ref_string");
  lua_settop (L, 0);
return L;
}

G_GNUC_INTERNAL
int
limr_patch_add_string (lua_State* L, const gchar* string_, gssize length)
{
  int elements;
  if (length < 0)
    length = strlen (string_);
  lua_getfield (L, LUA_REGISTRYINDEX, LSTRINGS);

  elements = lua_objlen (L, -1) + 1;
  lua_pushinteger (L, elements);
  lua_pushlstring (L, string_, length);
  lua_settable (L, -3);
  lua_pop (L, 1);
return elements;
}

G_GNUC_INTERNAL
int
limr_patch_compile (lua_State* L, const gchar* source, gssize length, GError** error)
{
  int result;

  result =
  luaL_loadbuffer (L, source, length, "=sketch");
  if (G_UNLIKELY (result != LUA_OK))
  {
    if(G_LIKELY
      (lua_type(L, -1) == LUA_TSTRING))
    {
      g_set_error
      (error,
       G_IO_ERROR,
       G_IO_ERROR_FAILED,
       "(%s: %i): %s",
       G_STRFUNC,
       __LINE__,
       lua_tostring(L, -1));
       lua_pop (L, 1);
    }
    else
    {
      g_set_error
      (error,
       G_IO_ERROR,
       G_IO_ERROR_FAILED,
       "(%s: %i): wtf",
       G_STRFUNC,
       __LINE__);
    }
  }
  else
  {
    g_assert (lua_type (L, -1) == LUA_TFUNCTION);
    lua_setfield (L, LUA_REGISTRYINDEX, LSKETCH);
  }
#if DEVELOPER == 1
  g_assert (lua_gettop (L) == 0);
#endif // DEVELOPER
return (result == LUA_OK) ? 0 : 1;
}

static int
limr_patch_throwrap(lua_State* L)
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

static int
limr_patch_boot (lua_State* L)
{
  GIOStream* stream = NULL;
  gpointer ostream = NULL;

  ostream = lua_touserdata (L, 1);
  stream = g_simple_io_stream_new (NULL, ostream);

  /*
   * Open temporary
   *
   */

  lua_getglobal (L, "io");

  lua_getfield (L, -1, "output");
  luaL_pushstream (L, stream);
  g_object_unref (stream);

  lua_call (L, 1, 0);
  lua_pop (L, 1);

  /*
   * Execute
   *
   */

  lua_getfield (L, LUA_REGISTRYINDEX, LSKETCH);
  lua_call (L, 0, 0);
return 0;
}

G_GNUC_INTERNAL
int
limr_patch_execute (lua_State* L, GOutputStream* stream, GCancellable* cancellable, GError** error)
{
  int top = lua_gettop (L);
  int result = 0;

  lua_pushcfunction (L, limr_patch_throwrap);
  lua_pushcfunction (L, limr_patch_boot);
  lua_pushlightuserdata (L, stream);

  result =
  lua_pcall (L, 1, 0, top);
  IF_LUA_ERROR (L, result)
  {
    g_set_error
    (error,
     G_IO_ERROR,
     G_IO_ERROR_FAILED,
     "%s",
     lua_tostring(L, -1));
    lua_pop (L, 1);
  }

  lua_pop (L, 1);
#if DEVELOPER == 1
    g_assert (lua_gettop (L) == top);
#endif // DEVELOPER
return result != 0;
}
