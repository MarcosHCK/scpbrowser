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
#include <limr_state_patch.h>
#include <limr_vm_data.h>
#include <inttypes.h>
#include <stdio.h>

G_DEFINE_QUARK
(limr-state-patch-error-quark,
 limr_state_patch_error);

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
#define THROW_GERROR(L, error) \
  G_STMT_START { \
    lua_pushstring(L, ((error))->message); \
    g_error_free(((error))); \
    lua_error(L); \
  } G_STMT_END
#define _lua_close0(var) ((var == NULL) ? NULL : (var = (lua_close (var), NULL)))

G_GNUC_INTERNAL
lua_State*
limr_state_patch_create_vm (GError** error)
{
  lua_State* L =
  luaL_newstate ();
  luaL_openlibs (L);
  lua_settop (L, 0);

  _limr_data_init (L);
  VmData* data =
  _limr_data_fetch (L);

  lua_getglobal (L, "io");
  lua_getfield (L, -1, "stdout");
  VmStream* stream = 
  lua_touserdata (L, -1);
  stream->handle = data->stdout_;
  lua_pop (L, 2);

  GError* tmp_err = NULL;
  limr_state_patch_reset_vm (L, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    _lua_close0 (L);
    return NULL;
  }
return L;
}

static int
_limr_ref_string (lua_State* L)
{
  int idx, id;

  id = luaL_checkinteger (L, 1);
  idx = lua_upvalueindex (1);
  lua_pushinteger (L, id);
  lua_gettable (L, idx);
return 1;
}

G_GNUC_INTERNAL
int
limr_state_patch_reset_vm (lua_State* L, GError** error)
{
  lua_createtable (L, 0, 0);
  lua_pushvalue (L, -1);
  lua_setfield (L, LUA_REGISTRYINDEX, LSTRINGS);
  lua_pushcclosure (L, _limr_ref_string, 1);
  lua_setglobal (L, "ref_string");
  lua_gc (L, LUA_GCCOLLECT, 1);
}

G_GNUC_INTERNAL
int
limr_state_patch_add_string (lua_State* L, const gchar* string_, gssize length)
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
limr_state_patch_compile (lua_State* L, const gchar* source, gssize length, GError** error)
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
       lua_tostring (L, -1));
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

static int
_limr_boot (lua_State* L)
{
  lua_pushcfunction (L, _limr_data_reset);
  lua_call (L, 0, 0);
  lua_getfield (L, LUA_REGISTRYINDEX, LSKETCH);
  lua_call (L, 0, 0);
return 0;
}

G_GNUC_INTERNAL
int
limr_state_patch_execute (lua_State* L, GOutputStream* stream, GCancellable* cancellable, GError** error)
{
  int top = lua_gettop (L);
  GError* tmp_err = NULL;
  VmData* data = NULL;
  int result = 0;
  long size = 0;

  lua_pushcfunction (L, _limr_throwrap);
  lua_pushcfunction (L, _limr_boot);

  result =
  lua_pcall (L, 0, 0, top);
  IF_LUA_ERROR (L, result)
  {
    g_set_error
    (error,
     LIMR_STATE_PATCH_ERROR,
     LIMR_STATE_PATCH_ERROR_RUN,
     "%s",
     lua_tostring(L, -1));
    lua_pop (L, 1);
  }

  data = _limr_data_fetch (L);
  size = ftell (data->stdout_);
  lua_pop (L, 1);

  result =
  (fflush (data->stdout_) == 0);
  if (G_UNLIKELY (result == FALSE))
  {
    int en = errno;
    g_set_error
    (error,
     LIMR_STATE_PATCH_ERROR,
     LIMR_STATE_PATCH_ERROR_FAILED,
     "%s",
     strerror (en));
    goto _error_;
  }

  result =
  g_output_stream_write_all (stream, data->buffer, size, NULL, cancellable, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    goto _error_;
  }

_error_:
#if DEVELOPER == 1
  g_assert (lua_gettop (L) == top);
#endif // DEVELOPER
return result != 0;
}
