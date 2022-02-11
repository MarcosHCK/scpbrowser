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
#include <limr_stream.h>

#define _fflush0(var) ((var == NULL) ? NULL : (var = (fflush (var), NULL)))
#define _fclose0(var) ((var == NULL) ? NULL : (var = (fclose (var), NULL)))
#define _free0(var) ((var == NULL) ? NULL : (var = (free (var), NULL)))
#define LMETA "__LIMR_STREAM"

static int inline
RELADDR (lua_State* L, int addr)
{
  if (addr >= 0)
    return addr;
return (lua_gettop (L) + 1) + addr;
}

struct _LimrStream
{
  FILE* handle;
  FILE* original;
  char* buffer;
  size_t buffsz;
};

static int
__gc (lua_State* L)
{
  LimrStream* stream =
  luaL_checkudata (L, 1, LMETA);
  _fflush0 (stream->handle);
  _fclose0 (stream->handle);
  _free0 (stream->buffer);
return 0;
}

/*
 * API
 *
 */

G_GNUC_INTERNAL
void
_limr_stream_new (lua_State* L)
{
  LimrStream* stream = NULL;
  int top = lua_gettop (L);
  static int meta = 0;

  if (G_UNLIKELY (meta == 0))
  {
    luaL_newmetatable (L, LMETA);
    {
      lua_pushliteral(L, "__metatable");
      lua_pushliteral(L, "protected");
      lua_settable(L, -3);
      lua_pushliteral (L, "__gc");
      lua_pushcfunction (L, __gc);
      lua_settable (L, -3);
    }
    lua_pop (L, 1);
  }

  stream =
  lua_newuserdata (L, sizeof (LimrStream));
  luaL_setmetatable (L, LMETA);

  stream->handle =
  open_memstream
  (&(stream->buffer),
   &(stream->buffsz));
#if DEBUG == 1
  g_assert (lua_gettop (L) == (top + 1));
#endif // DEBUG
}

G_GNUC_INTERNAL
void
_limr_stream_setup (lua_State* L, int env)
{
  LimrStream* stream = NULL;
  LStream* lstream = NULL;
  int top = lua_gettop (L);
  int idx = RELADDR (L, -1);
      env = RELADDR (L, env);

  stream = luaL_checkudata (L, idx, LMETA);

  lua_pushliteral (L, "io");
  lua_gettable (L, env);
  if (!lua_isnil (L, -1))
  {
    lua_pushliteral (L, "stdout");
    lua_gettable (L, -2);
    if (!lua_isnil (L, -1))
    {
      lstream = lua_touserdata (L, -1);
      stream->original = lstream->handle;
      lstream->handle = stream->handle;
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
#if DEBUG == 1
  g_assert (lua_gettop (L) == top);
#endif // DEBUG
}

G_GNUC_INTERNAL
void
_limr_stream_close (lua_State* L, int env)
{
  LimrStream* stream = NULL;
  LStream* lstream = NULL;
  int top = lua_gettop (L);
  int idx = RELADDR (L, -1);
      env = RELADDR (L, env);

  stream =
  luaL_checkudata (L, idx, LMETA);

  /* restore previous stream */
  lua_pushliteral (L, "io");
  lua_gettable (L, env);
  g_assert (!lua_isnil (L, -1));

  lua_pushliteral (L, "stdout");
  lua_gettable (L, -2);
  g_assert (!lua_isnil (L, -1));
  {
    lstream = lua_touserdata (L, -1);
    lstream->handle = stream->original;
  }
  lua_pop (L, 2);

  _fflush0 (stream->handle);
  _fclose0 (stream->handle);
#if DEBUG == 1
  g_assert (lua_gettop (L) == top);
#endif // DEBUG
}

G_GNUC_INTERNAL
GBytes*
_limr_stream_dump (lua_State* L)
{
  LimrStream* stream = NULL;
  gpointer buffer = NULL;
  GBytes* bytes = NULL;
  int top = lua_gettop (L);
  int idx = RELADDR (L, -1);

  stream =
  luaL_checkudata (L, idx, LMETA);
  {
    _fflush0 (stream->handle);

    bytes =
    g_bytes_new (stream->buffer, stream->buffsz);
  }
#if DEBUG == 1
  g_assert (lua_gettop (L) == top);
#endif // DEBUG
return bytes;
}
