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
#include <errno.h>
#include <limr_patch.h>
#include <limr_stream.h>
#include <limr_xpcall.h>
#include <stdio.h>

static const int chunkSize = 256;
static const int slicesSize = 16;
static const int bufferSize = 1024;

#define _g_string_chunk_free0(var) ((var == NULL) ? NULL : (var = (g_string_chunk_free (var), NULL)))
#define _g_ptr_array_unref0(var) ((var == NULL) ? NULL : (var = (g_ptr_array_unref (var), NULL)))
#define _g_string_free0(var) ((var == NULL) ? NULL : (var = (g_string_free (var, TRUE), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

G_GNUC_INTERNAL
gboolean
limr_parser_parse (lua_State* L,
                   lua_Reader reader,
                   gpointer reader_target,
                   GString* source,
                   GStringChunk* table,
                   GPtrArray* slices);

static int
_pread (lua_State* L)
{
  const char** repo_result = lua_touserdata (L, 1);
  size_t* repo_length = lua_touserdata (L, 2);
  lua_Reader reader = lua_touserdata (L, 3);
  gpointer target = lua_touserdata (L, 4);
  const char* result = NULL;
  size_t length = 0;

  result =
  reader (L, target, &length);

  *repo_result = result;
  *repo_length = length;
return 0;
}

G_GNUC_INTERNAL
const char*
_limr_preader (lua_State* L, lua_Reader reader, gpointer target, size_t* length, GError** error)
{
  GError* tmp_err = NULL;
  const char* repo_result = NULL;
  size_t repo_length = 0;
  int top = lua_gettop (L);

  lua_pushcfunction (L, _pread);
  lua_pushlightuserdata (L, &repo_result);
  lua_pushlightuserdata (L, &repo_length);
  lua_pushlightuserdata (L, reader);
  lua_pushlightuserdata (L, target);

  _limr_xpcall (L, 4, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    repo_result = NULL;
    repo_length = 0;
    goto _out_;
  }

_out_:
  if (length != NULL)
    *length = repo_length;
#if DEVELOPER == 1
  g_assert (lua_gettop (L) == top);
#endif // DEVELOPER
return repo_result;
}

static int
_ref_string_write (lua_State* L, LStream* stream, const gchar* string_)
{
  size_t length = strlen (string_);
  size_t wrote = 0;

  wrote = (fwrite (string_, 1, length, stream->handle));

  if (G_UNLIKELY (wrote != length))
  {
    int err = errno;
    lua_pushstring (L, g_strerror (err));
    lua_error (L);
  }
return 0;
}

static int
_ref_string (lua_State* L)
{
  lua_Integer index = luaL_checkinteger (L, 1);
  const gchar** strings = (const gchar**)
  (lua_touserdata (L, lua_upvalueindex (2)));
  gsize length = GPOINTER_TO_SIZE
  (lua_touserdata (L, lua_upvalueindex (3)));
  gpointer pstream = NULL;
  int result = 0;

  if (index < length)
  {
    lua_pushliteral (L, "io");
    lua_gettable (L, LUA_ENVIRONINDEX);
    if (!lua_isnil (L, -1))
    {
      lua_pushliteral (L, "stdout");
      lua_gettable (L, -2);
      if (!lua_isnil (L, -1))
      {
        pstream = lua_touserdata (L, -1);
        result = _ref_string_write (L, pstream, strings[index]);
        lua_pop (L, 1);
      }
      else
      {
        luaL_error (L, "Environment's IO capabilities doesn't supports standard output");
        g_assert_not_reached ();
      }

      lua_pop (L, 1);
    }
    else
    {
      luaL_error (L, "Environment doesn't supports IO");
      g_assert_not_reached ();
    }
  }
  else
  {
    luaL_error (L, "Out of bounds");
    g_assert_not_reached ();
  }
return result;
}

static int
_sketch (lua_State* L)
{
  int arguments = lua_gettop (L);

  lua_pushvalue (L, lua_upvalueindex (1));
  lua_pushvalue (L, LUA_ENVIRONINDEX);
  lua_pushliteral (L, "ref_string");
  lua_pushvalue (L, lua_upvalueindex (2));
  lua_rawset (L, -3);
  lua_setfenv (L, -2);
  lua_insert (L, 1);

  lua_call (L, arguments, LUA_MULTRET);
return lua_gettop (L);
}

G_GNUC_INTERNAL
int
_limr_loadx (lua_State* L, lua_Reader reader, void* dt, const gchar* chunkname, const gchar* mode)
{
  GStringChunk* table = g_string_chunk_new (chunkSize);
  GPtrArray* slices = g_ptr_array_sized_new (slicesSize);
  GString* source = g_string_sized_new (bufferSize);
  int top = lua_gettop (L);
  gboolean success = TRUE;
  gsize length = 0;
  int result = 0;

  success =
  limr_parser_parse (L, reader, dt, source, table, slices);
  if (G_UNLIKELY (success == FALSE))
  {
    lua_pushlstring (L, source->str, source->len);
    result = LUA_ERRSYNTAX;
    goto _out_;
  }

  result =
  luaL_loadbufferx (L, source->str, source->len, chunkname, mode);
  if (G_LIKELY (result == LUA_OK))
  {
    g_assert (lua_isfunction (L, -1));
    lua_pushlightuserdata (L, g_steal_pointer (&table));
    lua_pushlightuserdata (L, g_ptr_array_steal (slices, &length));
    lua_pushlightuserdata (L, GSIZE_TO_POINTER (length));
    lua_pushcclosure (L, _ref_string, 3);
    lua_pushcclosure (L, _sketch, 2);
  }

_out_:
  _g_string_chunk_free0 (table);
  _g_ptr_array_unref0 (slices);
  _g_string_free0 (source);
#if DEVELOPER == 1
  g_assert (lua_gettop (L) == (top + 1));
#endif // DEVELOPER
return result;
}

G_GNUC_INTERNAL
int
_limr_load (lua_State* L, lua_Reader reader, void* dt, const gchar* chunkname)
{
  int result =
  _limr_loadx (L, reader, dt, chunkname, "bt");
return result;
}

G_GNUC_INTERNAL
GBytes*
_limr_run_sketch (lua_State* L, GError** error)
{
  int top = lua_gettop (L);
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;

  /* setup environment */
  lua_pushliteral (L, LLIBRARY);
  lua_gettable (L, LUA_REGISTRYINDEX);
  lua_pushliteral (L, "environ");
  lua_gettable (L, -2);
  lua_remove (L, -2);
  lua_pushvalue (L, -1);
  lua_setfenv (L, -3);

  /* create and setup stream */
  _limr_stream_new (L);
  _limr_stream_setup (L, -2);

  /* push sketch function */
  lua_pushvalue (L, -3);

  /* execute sketch */
  _limr_xpcall (L, 0, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    success = FALSE;
    goto _out_;
  }

_out_:
  /* dump output */
  if (G_LIKELY (success == TRUE))
    bytes = _limr_stream_dump (L);

  /* close stream */
  _limr_stream_close (L, -2);

  /* pop stream, environment and sketch function */
  lua_pop (L, 3);

  /* perform a full-cycle garbage collection */
  lua_gc (L, LUA_GCCOLLECT, 1);
#if DEVELOPER == 1
  g_assert (lua_gettop (L) == (top - 1));
#endif // DEVELOPER
return bytes;
}
