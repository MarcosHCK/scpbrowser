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
#include <gio/gio.h>
#include <gmodule.h>
#include <limr_patch.h>
#include <limr_stream.h>
#include <limr_xpcall.h>
#include <private.h>

G_DEFINE_QUARK
(limr-stream-reader-buffer-quark,
 _limr_stream_reader_buffer);
G_DEFINE_QUARK
(limr-stream-reader-chunkname-quark,
 _limr_stream_reader_chunkname);

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) ((var == NULL) ? NULL : (var = (g_free (var), NULL)))

/*
 * Functions
 *
 */

static int
_string_reader (lua_State* L)
{
  lua_pushvalue (L, lua_upvalueindex (1));
  lua_pushnil (L);
  lua_replace (L, lua_upvalueindex (1));
return 1;
}

static const char*
_function_reader (lua_State* L, void* ud, size_t* length)
{
  lua_State* L2 = ud;
  lua_State* L1 = L;
  *length = 0;

  lua_settop (L2, 1);
  lua_pushvalue (L2, 1);
  lua_call (L2, 0, 1);
  if (lua_isstring (L2, 2))
    return lua_tolstring (L2, 2, length);
return NULL;
}

static int
lib_load (lua_State* L)
{
  const gchar* chunkname = NULL;
  const gchar* mode = NULL;
  int top = lua_gettop (L);
  int result;

  chunkname = luaL_optstring (L, 2, NULL);
  mode = luaL_optstring (L, 3, NULL);
  if (!lua_isnoneornil (L, 4))
    luaL_checktype (L, 4, LUA_TTABLE);

  if (!lua_isfunction (L, 1))
  {
    const gchar* buffer = NULL;
    size_t length = 0;

    lua_pushvalue (L, 1);
    lua_pushboolean (L, 1);
    lua_pushcclosure (L, _string_reader, 2);
    lua_replace (L, 1);
#if DEBUG == 1
    g_assert (lua_gettop (L) == top);
#endif // DEBUG
  }

  lua_State* L2 =
  lua_newthread (L);
  lua_pushvalue (L, 1);
  lua_xmove (L, L2, 1);

  result =
  _limr_loadx (L, _function_reader, L2, chunkname, mode);
  switch (result)
  {
  case LUA_ERRSYNTAX:
#if DEBUG == 1
    g_assert (lua_isthread (L, -2));
    lua_remove (L, -2);
#endif // DEBUG
    lua_pushboolean (L, FALSE);
    lua_insert (L, -2);
    return 2;
  case LUA_ERRMEM:
#if DEBUG == 1
    g_assert (lua_isthread (L, -2));
    lua_remove (L, -2);
#endif // DEBUG
    lua_pushboolean (L, FALSE);
    lua_pushstring (L, "Out of memory");
    return 2;
  default:
    g_assert (result == LUA_OK);
#if DEBUG == 1
    g_assert (lua_isthread (L, -2));
    lua_remove (L, -2);
#endif // DEBUG
    if (!lua_isnoneornil (L, 4))
    { lua_pushvalue (L, 4);
      lua_setfenv (L, -2); }
    return 1;
  }
return 0;
}

static const char*
_stream_reader (lua_State* L, void* ud, size_t* length)
{
  GInputStream* stream = ud;
  const int bufferSize = 1024;
  GError* tmp_err = NULL;
  gchar* buffer = NULL;
  gsize read;

  buffer =
  g_object_get_qdata
  (G_OBJECT (stream),
   _limr_stream_reader_buffer_quark ());
  if (G_UNLIKELY (buffer == NULL))
  {
    buffer = g_malloc (sizeof (bufferSize));
    g_object_set_qdata_full
    (G_OBJECT (stream),
     _limr_stream_reader_buffer_quark (),
     buffer,
     (GDestroyNotify)
     g_free);
  }

  read =
  g_input_stream_read (stream, buffer, bufferSize, NULL, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    _limr_throwgerror (L, tmp_err);
    g_assert_not_reached ();
  }
  else
  {
    *length = (size_t) read;
  }
return buffer;
}

static int
lib_include (lua_State* L)
{
#if DEBUG == 1
  int top = lua_gettop (L);
#endif // DEBUG
  GInputStream* stream = NULL;
  const gchar* chunkname = NULL;
  const gchar* name = NULL;
  gchar* basename = NULL;
  GError* tmp_err = NULL;
  GFile* file = NULL;
  int result = 0;

  name = luaL_checkstring (L, 1);
  file = g_file_new_for_commandline_arg (name);
  stream = (GInputStream*)
  g_file_read (file, NULL, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  { _g_object_unref0 (stream);
    _g_object_unref0 (file);
    _limr_throwgerror (L, tmp_err); }

  basename = g_file_get_basename (file);
  if (basename == NULL)
    basename = g_strdup (name);
  _g_object_unref0 (file);
  g_object_set_qdata_full
  (G_OBJECT (stream),
   _limr_stream_reader_chunkname_quark (),
   basename,
   (GDestroyNotify)
   g_free);

  result =
  _limr_loadx (L, _stream_reader, stream, basename, NULL);
  g_object_unref (stream);

  switch (result)
  {
  case LUA_ERRSYNTAX:
    lua_error (L);
    break;
  case LUA_ERRMEM:
    luaL_error (L, "Out of memory");
    break;
  default:
    g_assert (result == LUA_OK);
    break;
  }

#if DEBUG == 1
  g_assert (lua_gettop (L) == (top + 1));
#endif // DEBUG
  lua_call (L, 0, LUA_MULTRET);
return lua_gettop (L) - top;
}

/*
 * Functions table
 *
 */

static luaL_Reg
functions[] =
{
  {"load", lib_load},
  {"include", lib_include},
};

/*
 * Library entry point
 *
 */

static void
patch_library (lua_State* L)
{
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  const gchar* data = NULL;
  gsize length = 0;
  int result;

  bytes =
  g_resources_lookup_data (GRESNAME "/lua/patch.lua", 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
    _limr_throwgerror (L, tmp_err);

  data = g_bytes_get_data (bytes, &length);

  result =
  luaL_loadbufferx (L, data, length, "=patch", "bt");
  g_bytes_unref (bytes);

  switch (result)
  {
  case LUA_ERRSYNTAX:
    lua_error (L);
    break;
  case LUA_ERRMEM:
    luaL_error (L, "Out of memory");
    break;
  default:
    g_assert (result == LUA_OK);
    break;
  }

  lua_pushvalue (L, -2);
  lua_call (L, 1, 0);
}

static void
set_info (lua_State* L)
{
  lua_pushliteral (L, "Copyright 2021-2025 MarcosHCK");
  lua_setfield (L, -2, "_COPYRIGHT");
  lua_pushliteral
  (L,
   "Limr is a php-inspired parser using Lua"
   "as scripting language");
  lua_setfield (L, -2, "_DESCRIPTION");
  lua_pushliteral (L, PACKAGE_STRING);
  lua_setfield (L, -2, "_VERSION");
}

G_MODULE_EXPORT
int
luaopen_liblimr (lua_State* L)
{
  _limr_stream_init (L);
#if LUA_VERSION_NUM >= 502
  luaL_newlib (L, functions);
#else // LUA_VERSION_NUM < 502
  lua_newtable (L);
  luaL_register (L, NULL, functions);
#endif // LUA_VERSION_NUM
  patch_library (L);
  set_info (L);
return 1;
}
