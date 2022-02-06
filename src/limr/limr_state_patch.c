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
#include <limr_vm_procs.h>
#include <stdio.h>

G_DEFINE_QUARK
(limr-state-patch-error-quark,
 limr_state_patch_error);

#define _lua_close0(var) ((var == NULL) ? NULL : (var = (lua_close (var), NULL)))

G_GNUC_INTERNAL
lua_State*
limr_state_patch_create_vm (GError** error)
{
  lua_State* L = NULL;
  VmData* data = NULL;
  VmStream* stream = NULL;
  GError* tmp_err = NULL;

  L =
  luaL_newstate ();
  luaL_openlibs (L);
  lua_settop (L, 0);

  _limr_data_init (L);
  data =
  _limr_data_fetch (L);

  lua_getglobal (L, "io");
  lua_getfield (L, -1, "stdout");
  stream = 
  lua_touserdata (L, -1);
  stream->handle = data->stdout_;
  lua_pop (L, 2);

  limr_procs_emit (L, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    _lua_close0 (L);
    return NULL;
  }

  lua_settop (L, 0);
  lua_gc (L, LUA_GCCOLLECT, 1);
return L;
}

static int
_ref_string (lua_State* L)
{
  int stridx;
  int idx;

  idx = lua_upvalueindex (1);
  stridx = luaL_checkinteger (L, 1);
  lua_pushinteger (L, stridx);
  lua_gettable (L, idx);
return 1;
}

static int
_sketch (lua_State* L)
{
  int idx1 = lua_upvalueindex (1);
  int idx2 = lua_upvalueindex (2);

  lua_pushvalue (L, idx2);
  lua_setfield (L, LUA_ENVIRONINDEX, "ref_string");
  lua_pushvalue (L, idx1);
  lua_call (L, 0, 0);
return 0;
}

G_GNUC_INTERNAL
int
limr_state_patch_compile (lua_State* L, const gchar** strings, gint n_strings, GString* source, GError** error)
{
  int result;

  result =
  luaL_loadbuffer (L, source->str, source->len, "=sketch");
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

  {
    GStringChunk** store;
    gint i;

    lua_createtable (L, n_strings, 1);
    for (i = 0; i < n_strings; i++)
    {
      lua_pushinteger (L, i + 1);
      lua_pushstring (L, strings[i]);
      lua_settable (L, -3);
    }

    lua_pushcclosure (L, _ref_string, 1);
    lua_pushcclosure (L, _sketch, 2);
  }

#if DEVELOPER == 1
  g_assert (lua_isfunction (L, -1));
  g_assert (lua_gettop (L) == 1);
#endif // DEVELOPER
return (result == LUA_OK) ? 0 : 1;
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
limr_state_patch_execute (lua_State* L, GOutputStream* stream, GCancellable* cancellable, GError** error)
{
  int top = lua_gettop (L);
  GError* tmp_err = NULL;
  VmData* data = NULL;
  int result = 0;
  long size = 0;

  lua_pushcfunction (L, _limr_throwrap);
  lua_pushcfunction (L, _limr_data_reset);
  result =
  lua_pcall (L, 0, 0, top + 1);
  IF_LUA_ERROR (L, result)
  {
    g_set_error
    (error,
     LIMR_STATE_PATCH_ERROR,
     LIMR_STATE_PATCH_ERROR_FAILED,
     "%s",
     lua_tostring(L, -1));
    lua_pop (L, 1);
  }

  lua_getfield (L, LUA_REGISTRYINDEX, LEXECUTOR);
  lua_pushvalue (L, top);
  result =
  lua_pcall (L, 1, 0, top + 1);
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
