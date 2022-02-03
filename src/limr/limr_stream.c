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

#define IS_VALID_INPUT_STREAM(L, stream, operation) \
  if (G_IS_INPUT_STREAM ((stream)) == FALSE) \
    WRONG_OPERATION (L, operation); \
  else \
  if (g_input_stream_is_closed ((stream)) == TRUE) \
    luaL_error (L, "closed file"); \
  else
#define IS_VALID_OUTPUT_STREAM(L, stream, operation) \
  if (G_IS_OUTPUT_STREAM ((stream)) == FALSE) \
    WRONG_OPERATION (L, operation); \
  else \
  if (g_output_stream_is_closed ((stream)) == TRUE) \
    luaL_error (L, "closed file"); \
  else
#define IS_VALID_SEEKABLE_STREAM(L, stream, operation) \
  if (G_IS_SEEKABLE ((stream)) == FALSE \
    || g_seekable_can_seek (G_SEEKABLE (stream)) == TRUE) \
    WRONG_OPERATION (L, operation); \
  else \
  if (g_io_stream_is_closed ((stream)) == TRUE) \
    luaL_error (L, "closed file"); \
  else
#define SEEK_OTHER(L, other, offset, type) \
  G_STMT_START { \
    GError* tmp_err = NULL; \
    if (G_IS_SEEKABLE (other) \
      && (g_seekable_can_seek (G_SEEKABLE (other)) == TRUE)) \
    { \
      g_seekable_seek (G_SEEKABLE (other), (offset), (type), NULL, &tmp_err); \
      if (G_UNLIKELY (tmp_err != NULL)) \
      { \
          lua_pushnil (L); \
          PUSH_GERROR (L, tmp_err); \
          return 2; \
      } \
    } \
  } G_STMT_END

#ifndef LUA_INTEGER_FMT
# ifdef LUA_INTFRMLEN
#  define LUA_INTEGER_FMT "%" LUA_INTFRMLEN
# else
#  define LUA_INTEGER_FMT "%d"
# endif // LUA_INTFRMLEN
#endif // !LUA_INTEGER_FMT

/*
 * Utils
 *
 */

static int
_io_seek (lua_State* L, GIOStream* stream, GSeekable* seekable, int start)
{
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  const gchar* whence = NULL;
  goffset offset = 0;
  GSeekType type;

  if (lua_isnoneornil (L, 2) == FALSE)
    whence = luaL_checkstring (L, 2);

  gboolean
  null_ = (whence == NULL),
  set_ = !(null_ || g_strcmp0 (whence, "set")),
  cur_ = !(null_ || g_strcmp0 (whence, "cur")),
  end_ = !(null_ || g_strcmp0 (whence, "end"));

  if (G_LIKELY ((set_ || cur_ || end_) == TRUE))
  {
    if (!lua_isnoneornil (L, 3))
      offset = luaL_checknumber (L, 3);
    else
      offset = 0;

    if (set_) type = G_SEEK_SET;
    if (cur_) type = G_SEEK_CUR;
    if (end_) type = G_SEEK_END;

    if (seekable == (GSeekable*) stream)
    {
      success =
      g_seekable_seek (G_SEEKABLE (stream), offset, type, NULL, &tmp_err);
      if (G_UNLIKELY (tmp_err != NULL))
      {
        lua_pushnil (L);
        PUSH_GERROR (L, tmp_err);
        return 2;
      }
    }
    else
    {
      GInputStream* istream = g_io_stream_get_input_stream (stream);
      GOutputStream* ostream = g_io_stream_get_output_stream (stream);
      gpointer other = NULL;

      if (seekable == (GSeekable*) istream)
        other = ostream;
      else
      if (seekable == (GSeekable*) ostream)
        other = istream;
      else
      if (istream != NULL && ostream != NULL)
        g_assert_not_reached ();

      success =
      g_seekable_seek (G_SEEKABLE (seekable), offset, type, NULL, &tmp_err);
      if (G_UNLIKELY (tmp_err != NULL))
      {
        lua_pushnil (L);
        PUSH_GERROR (L, tmp_err);
        return 2;
      }
      else
      {
        if (G_IS_INPUT_STREAM (other)
          || G_IS_OUTPUT_STREAM (other))
        {
          SEEK_OTHER (L, other, offset, type);
        }
      }
    }
  } else
  if (G_LIKELY (null_ == TRUE))
  {
    offset =
    g_seekable_tell (G_SEEKABLE(stream));
    lua_pushnumber (L, offset);
    return 1;
  }
  else
  {
    lua_pushnil (L);
    lua_pushfstring (L, "invalid option '%s'", whence);
    return 2;
  }

  lua_pushboolean (L, TRUE);
return 1;
}

static int
_io_write (lua_State* L, GIOStream* stream, GOutputStream* ostream, int start)
{
  GError* tmp_err = NULL;
  gboolean success = FALSE;
  int nargs = lua_gettop (L) - start;
  int n;

  if (nargs == 0)
  {
    return 0;
  }
  else
  {
    luaL_checkstack (L, nargs + LUA_MINSTACK, "too many arguments");
    success = TRUE;
    gsize wrote;

    for (n = start; nargs-- && success; n++)
    {
      if (lua_type (L, n + 1) == LUA_TNUMBER)
      {
#ifdef lua_isinteger
        if (lua_isinteger(L, n + 1))
        {
          success =
          g_output_stream_printf (ostream, &wrote, NULL, &tmp_err, LUA_INTEGER_FMT, lua_tointeger (L, n + 1));
          if (G_UNLIKELY (tmp_err != NULL))
          {
            lua_pushnil (L);
            PUSH_GERROR (L, tmp_err);
            return 2;
          }
          else
          {
            GInputStream* istream = g_io_stream_get_input_stream (stream);
            if (G_IS_INPUT_STREAM (istream))
            {
              SEEK_OTHER (L, istream, (goffset) wrote, G_SEEK_CUR);
            }
          }
        }
        else
        {
#endif // lua_isinteger
          success =
          g_output_stream_printf (ostream, &wrote, NULL, &tmp_err, LUA_NUMBER_FMT, lua_tonumber (L, n + 1));
          if (G_UNLIKELY (tmp_err != NULL))
          {
            lua_pushnil (L);
            PUSH_GERROR (L, tmp_err);
            return 2;
          }
          else
          {
            GInputStream* istream = g_io_stream_get_input_stream (stream);
            if (G_IS_INPUT_STREAM (istream))
            {
              SEEK_OTHER (L, istream, (goffset) wrote, G_SEEK_CUR);
            }
          }
#ifdef lua_isinteger
        }
#endif // lua_isinteger
      }
      else
      {
        const char* str = NULL;
        size_t size;

        str =
        luaL_checklstring (L, n + 1, &size);
        success =
        g_output_stream_write_all (ostream, str, size, &wrote, NULL, &tmp_err);
        if (G_UNLIKELY (tmp_err != NULL))
        {
          lua_pushnil (L);
          PUSH_GERROR (L, tmp_err);
          return 2;
        }
        else
        {
          GInputStream* istream = g_io_stream_get_input_stream (stream);
          if (G_IS_INPUT_STREAM (istream))
          {
            SEEK_OTHER (L, istream, (goffset) wrote, G_SEEK_CUR);
          }
        }
      }
    }
  }

  lua_pushboolean (L, TRUE);
return 1;
}

/*
 * Methods
 *
 */

static int
f_close (lua_State* L)
{
  GIOStream* stream = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;

  stream = luaL_checkstream (L, 1);
  success = g_io_stream_close (stream, g_cancellable_get_current (), &tmp_err);
  _g_object_unref0 (stream);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  lua_pushboolean (L, success);
return 1;
}

static int
f_flush (lua_State* L)
{
  GIOStream* stream = NULL;
  GOutputStream* ostream = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;

  stream = luaL_checkstream (L, 1);
  ostream = g_io_stream_get_output_stream (stream);
  _g_object_unref0 (stream);
  IS_VALID_OUTPUT_STREAM (L, ostream, "flush")
  {
    success =
    g_output_stream_flush (ostream, g_cancellable_get_current (), &tmp_err);
    if (G_UNLIKELY (tmp_err != NULL))
    {
      THROW_GERROR (L, tmp_err);
      g_assert_not_reached ();
    }
  }

  lua_pushboolean (L, success);
return 1;
}

static int
lines_iter (lua_State* L)
{
  GIOStream* stream = NULL;
  int top = lua_gettop (L);
  int idx;

  idx = lua_upvalueindex (1);
  stream = luaL_checkstream (L, idx);
  _g_object_unref0 (stream);

  lua_getfield (L, idx, "read");
  lua_pushvalue (L, idx);
  lua_pushliteral (L, "*l");
  lua_call (L, 2, LUA_MULTRET);
return lua_gettop (L) - top;
}

static int
f_lines (lua_State* L)
{
  GIOStream* stream = NULL;
  GInputStream* istream = NULL;
  int result = 0;

  stream = luaL_checkstream (L, 1);
  istream = g_io_stream_get_input_stream (stream);
  _g_object_unref0 (stream);
  IS_VALID_INPUT_STREAM (L, istream, "lines")
  {
    lua_pushvalue (L, 1);
    lua_pushcclosure (L, lines_iter, 1);
    result++;
  }
return 0;
}

static int
f_read (lua_State* L)
{
  luaL_error (L, "unimplemented");
return 0;
}

static int
f_seek (lua_State* L)
{
  GIOStream* stream = NULL;
  GInputStream* istream = NULL;
  GOutputStream* ostream = NULL;
  int result = 0;

  stream = luaL_checkstream (L, 1);
  istream = g_io_stream_get_input_stream (stream);
  ostream = g_io_stream_get_output_stream (stream);
  _g_object_unref0 (stream);
  if (G_IS_SEEKABLE (stream) && (g_seekable_can_seek (G_SEEKABLE (stream)) == TRUE))
  {
    result = _io_seek (L, stream, G_SEEKABLE (stream), 1);
  }
  else
  {
    if (G_IS_SEEKABLE (istream) && (g_seekable_can_seek (G_SEEKABLE (istream)) == TRUE))
      result = _io_seek (L, stream, G_SEEKABLE (stream), 1);
    else
    if (G_IS_SEEKABLE (ostream) && (g_seekable_can_seek (G_SEEKABLE (ostream)) == TRUE))
      result = _io_seek (L, stream, G_SEEKABLE (stream), 1);
  }
return result;
}

static int
f_setvbuf (lua_State* L)
{
  WRONG_OPERATION (L, "setvbuf");
  g_assert_not_reached ();
return 0;
}

static int
f_write (lua_State* L)
{
  GIOStream* stream = NULL;
  GOutputStream* ostream = NULL;
  int result = 0;

  stream = luaL_checkstream (L, 1);
  ostream = g_io_stream_get_output_stream (stream);
  _g_object_unref0 (stream);
  IS_VALID_OUTPUT_STREAM (L, ostream, "write");
  {
    CleanupValue* value = lua_touserdata (L, 1);
    result = _io_write (L, value->object, ostream, 1);
  }
return result;
}

/*
 * Metatable
 *
 */

static luaL_Reg
stream__index[] =
{
  { "close", f_close },
  { "flush", f_flush },
  { "lines", f_lines },
  { "read", f_read },
  { "seek", f_seek },
  { "setvbuf", f_setvbuf },
  { "write", f_write },
  {NULL, NULL},
};

static int
stream__tostring (lua_State* L)
{
  CleanupValue* value =
  luaL_checkudata (L, 1, _IO_STREAM);

  lua_pushfstring
  (L,
   _IO_STREAM ": %p",
   (guintptr)
   value->object);
return 1;
}

static int
stream__gc (lua_State* L)
{
  CleanupValue* value = NULL;

  if (G_UNLIKELY (lua_type (L, 1) != LUA_TUSERDATA))
    luaL_typerror (L, 1, "userdata");
  value =
  lua_touserdata (L, 1);
  if (G_LIKELY
      (value->notify != NULL
       && value->object != NULL))
  {
    g_clear_pointer (&(value->object), value->notify);
  }
return 0;
}

static luaL_Reg
stream_mt[] =
{
  { "__tostring", stream__tostring },
  { "__gc", stream__gc },
  {NULL, NULL},
};

G_GNUC_INTERNAL
void
_limr_stream_create_meta (lua_State* L)
{
  luaL_newmetatable (L, _IO_STREAM);
  luaL_setfuncs (L, stream_mt, 0);

  /* __name */
  lua_pushliteral (L, "__name");
  lua_pushliteral (L, _IO_STREAM);
  lua_settable (L, -3);

  /* __index */
  lua_pushliteral (L, "__index");
#if LUA_VERSION_NUM >= 502
  luaL_newlib (L, stream__index);
#else // LUA_VERSION_NUM < 502
  lua_newtable (L);
  luaL_register (L, NULL, stream__index);
#endif // LUA_VERSION_NUM
  lua_settable (L, -3);

  /* end */
  lua_pop (L, 1);
}

/*
 * API
 *
 */

void
luaL_pushstream (lua_State* L, gpointer stream)
{
  CleanupValue* value =
  luaL_createcvalue (L, g_object_ref (stream), g_object_unref);
  luaL_setmetatable (L, _IO_STREAM);
  value->gtype = G_TYPE_IO_STREAM;
}

gpointer
luaL_tostream (lua_State* L, int idx)
{
  CleanupValue* value = NULL;
  gpointer stream = NULL;
  int result;

  value =
  lua_touserdata (L, idx);
  if (value != NULL)
  {
    result =
    lua_getmetatable (L, idx);
    if (result != 0)
    {
      luaL_getmetatable (L, _IO_STREAM);

      result =
      lua_rawequal (L, -1, -2);
      lua_pop (L, 2);

      if (result == 1 && value->gtype == G_TYPE_IO_STREAM)
      {
        stream = g_object_ref (value->object);
      }
    }
  }
return stream;
}

gpointer
luaL_checkstream (lua_State* L, int idx)
{
  CleanupValue* value = NULL;
  gpointer stream = NULL;

  value = luaL_checkudata (L, idx, _IO_STREAM);
  stream = value->object;
  if (value->gtype != G_TYPE_IO_STREAM
    || G_IS_IO_STREAM (stream) == FALSE)
    luaL_error (L, "bad object");
return g_object_ref (stream);
}
