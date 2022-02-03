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
#include <limr_value.h>
#include <gmodule.h>

#define _IO_LIBRARY "__LIMR_IO_LIBRARY"

#ifdef G_PLATFORM_WIN32
# include <gio-win32-2.0/gio/gwin32inputstream.h>
# include <gio-win32-2.0/gio/gwin32outputstream.h>
# include <windows.h>
#else // !G_PLATFORM_WIN32
# include <gio-unix-2.0/gio/gunixinputstream.h>
# include <gio-unix-2.0/gio/gunixoutputstream.h>
# include <stdio.h>
#endif // G_PLATFORM_WIN32

/*
 * Functions
 *
 */

static int
io_open (lua_State* L)
{
  luaL_error (L, "unimplemented");
return 0;
}

static int
io_popen (lua_State* L)
{
  luaL_error (L, "unimplemented");
return 0;
}

static int
io_tmpfile (lua_State* L)
{
  GIOStream* stream = NULL;
  GError* tmp_err = NULL;
  GFile* file = NULL;

  file =
  g_file_new_tmp (NULL, (GFileIOStream**) &stream, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  luaL_pushstream (L, stream);
  _g_object_unref0 (stream);
  _g_object_unref0 (file);
return 1;
}

static int
io_type (lua_State* L)
{
  GIOStream* stream = NULL;
  gboolean closed = FALSE;

  stream = luaL_checkstream (L, 1);
  closed = g_io_stream_is_closed (stream);
  _g_object_unref0 (stream);

  if (closed)
    lua_pushliteral (L, "closed file");
  else
    lua_pushliteral (L, "file");
return 1;
}

/*
 * Library functions
 *
 */

static luaL_Reg
functions[] =
{
  { "open", io_open },
  { "popen", io_popen },
  { "tmpfile", io_tmpfile },
  { "type", io_type },
  {NULL, NULL},
};

/*
 * Library entry point
 * luaopen_* function
 *
 */

static void
set_info(lua_State * L)
{
  lua_pushliteral (L, "Copyright 2021-2025 MarcosHCK");
  lua_setfield (L, -2, "_COPYRIGHT");
  lua_pushliteral
  (L,
   "limr_io is a GIO-based drop-in replacement for "
   "standard Lua IO library. Is part of Limr project "
   "so be carefully when use it outside said project.");
  lua_setfield (L, -2, "_DESCRIPTION");
  lua_pushliteral (L, PACKAGE_STRING);
  lua_setfield (L, -2, "_VERSION");
}

static int
_limr_patch_native (lua_State* L)
{
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  int result;

  bytes =
  g_resources_lookup_data (GRESNAME "/limr_io/io.lua", 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_bytes_unref (bytes);
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  result =
  luaL_loadbuffer (L, g_bytes_get_data (bytes, NULL), g_bytes_get_size (bytes), "=io.lua");
  if (result != LUA_OK)
  {
    if (!lua_isstring (L, -1))
      lua_pushliteral (L, "wtf?");
    lua_error (L);
  }

  result =
  

  g_bytes_unref (bytes);
return 0;
}

G_MODULE_EXPORT
int
luaopen_limr_io (lua_State* L)
{
  GIOStream *_stdin = NULL, *_stdout = NULL, *_stderr = NULL;
  GOutputStream *__stdout = NULL, *__stderr = NULL;
  GInputStream *__stdin = NULL;

  _limr_value_create_meta (L);
  _limr_stream_create_meta (L);
#if LUA_VERSION_NUM >= 502
  luaL_newlib (L, functions);
#else // LUA_VERSION_NUM < 502
  lua_newtable (L);
  luaL_register (L, NULL, functions);
#endif // LUA_VERSION_NUM

  /* registry */
  lua_pushvalue (L, 1);
  lua_setfield (L, LUA_REGISTRYINDEX, _IO_LIBRARY);

  /* platform std{in,out,err} */
#if G_PLATFORM_WIN32
  __stdin = (GInputStream*) g_win32_input_stream_new ((void*) STD_INPUT_HANDLE, FALSE);
  __stdout = (GOutputStream*) g_win32_output_stream_new ((void*) STD_OUTPUT_HANDLE, FALSE);
  __stderr = (GOutputStream*) g_win32_output_stream_new ((void*) STD_ERROR_HANDLE, FALSE);
#else // !G_PLATFORM_WIN32
  __stdin = (GInputStream*) g_unix_input_stream_new (STDIN_FILENO, FALSE);
  __stdout = (GOutputStream*) g_unix_output_stream_new (STDOUT_FILENO, FALSE);
  __stderr = (GOutputStream*) g_unix_output_stream_new (STDERR_FILENO, FALSE);
#endif // G_PLATFORM_WIN32

  /* wrap demi streams */
  g_set_object (&(__stdin), (GInputStream*) g_data_input_stream_new (__stdin));
  g_set_object (&(__stdout), (GOutputStream*) g_data_output_stream_new (__stdout));
  g_set_object (&(__stderr), (GOutputStream*) g_data_output_stream_new (__stderr));

  /* create streams */
  _stdin = g_simple_io_stream_new (__stdin, NULL);
  _stdout = g_simple_io_stream_new (NULL, __stdout);
  _stderr = g_simple_io_stream_new (NULL, __stderr);

  /* stdin */
  luaL_pushstream (L, _stdin);
  luaL_setmetatable (L, _IO_STREAM);
  lua_setfield (L, -2, "stdin");

  /* stdout */
  luaL_pushstream (L, _stdout);
  luaL_setmetatable (L, _IO_STREAM);
  lua_setfield (L, -2, "stdout");

  /* stderr */
  luaL_pushstream (L, _stderr);
  luaL_setmetatable (L, _IO_STREAM);
  lua_setfield (L, -2, "stderr");

  /* info */
  set_info (L);

  /* cleanup */
  _g_object_unref0 (__stderr);
  _g_object_unref0 (_stderr);
  _g_object_unref0 (__stdout);
  _g_object_unref0 (_stdout);
  _g_object_unref0 (__stdin);
  _g_object_unref0 (_stdin);

  /* execute native patch */
  _limr_patch_native (L);
return 1;
}
