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
 * API
 * functions
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
    buffer = g_malloc (bufferSize);
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
lib_loadfile (lua_State* L)
{
#if DEBUG == 1
  int top = lua_gettop (L);
#endif // DEBUG
  GInputStream* stream = NULL;
  const gchar* chunkname = NULL;
  const gchar* mode = NULL;
  const gchar* name = NULL;
  gchar* basename = NULL;
  GError* tmp_err = NULL;
  GFile* file = NULL;
  int result = 0;

  name = luaL_checkstring (L, 1);
  chunkname = luaL_optstring (L, 2, NULL);
  mode = luaL_optstring (L, 3, NULL);
  if (!lua_isnoneornil (L, 4))
    luaL_checktype (L, 4, LUA_TTABLE);

  file = g_file_new_for_commandline_arg (name);

  stream = (GInputStream*)
  g_file_read (file, NULL, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
    {
      _g_object_unref0 (stream);
      _g_object_unref0 (file);
      _limr_throwgerror (L, tmp_err);
    }
  else
    {
      basename = g_file_get_basename (file);
      _g_object_unref0 (file);
    }

  result =
  _limr_loadx (L, _stream_reader, stream, basename, mode);
  _g_object_unref0 (stream);
  _g_free0 (basename);

  switch (result)
  {
  case LUA_ERRSYNTAX:
    lua_pushboolean (L, FALSE);
    lua_insert (L, -2);
#if DEBUG == 1
    g_assert (lua_gettop (L) == (top + 2));
#endif // DEBUG
    return 2;
  case LUA_ERRMEM:
    lua_pushboolean (L, FALSE);
    lua_pushstring (L, "Out of memory");
#if DEBUG == 1
    g_assert (lua_gettop (L) == (top + 2));
#endif // DEBUG
    return 2;
  default:
    g_assert (result == LUA_OK);
    if (!lua_isnoneornil (L, 4))
    { lua_pushvalue (L, 4);
      lua_setfenv (L, -2); }
#if DEBUG == 1
    g_assert (lua_gettop (L) == (top + 1));
#endif // DEBUG
    return 1;
  }
#if DEBUG == 1
  g_assert (lua_gettop (L) == top);
#endif // DEBUG
return 0;
}

static int
lib_checkarg (lua_State* L)
{
  int i, argn = 0;
  const char* got = NULL;
  const char* type = NULL;
  int top = lua_gettop (L);
  int result = FALSE;

  argn = luaL_checkinteger (L, 1);
  got = luaL_typename (L, 2);

  luaL_Buffer b;
  luaL_buffinit(L, &b);
  luaL_addstring (&b, "expected");

  for (i = 3; i < (top + 1); i++)
    {
      type = luaL_checkstring (L, i);
      if (!g_strcmp0 (type, got))
        {
          result = TRUE;
          break;
        }
      else
        {
          if (i == 3)
            {
              lua_pushfstring (L, " %s", type);
              luaL_addvalue (&b);
            }
          else
            {
              lua_pushfstring (L, " o %s", type);
              luaL_addvalue (&b);
            }
        }
    }


  if (result == TRUE)
    {
      luaL_pushresult (&b);
      lua_pop (L, 1);
    }
  else
    {
      lua_pushfstring (L, ", got %s", got);
      luaL_addvalue (&b);
      luaL_pushresult (&b);
      type = lua_tostring (L, -1);
      luaL_argerror (L, argn, type);
    }
#if DEBUG == 1
  g_assert (lua_gettop (L) == top);
#endif // DEBUG
return 0;
}

static int
_include (lua_State* L)
{
  luaL_checktype (L, 1, LUA_TSTRING);
  luaL_checktype (L, 2, LUA_TFUNCTION);

  lua_pushvalue (L, 2);
  lua_pushvalue (L, 1);
  lua_pushvalue (L, 3);
  lua_call (L, 2, 2);

  if (lua_isfalse (L, -2))
    {
      lua_error (L);
      g_assert_not_reached();
    }
  else
    {
      if (G_UNLIKELY (lua_isfunction (L, -2) == FALSE))
        {
          luaL_error (L, "searcher function should return a function");
          g_assert_not_reached ();
        }

      lua_pop (L, 1);

      lua_pushvalue (L, 1);
      lua_pushvalue (L, 3);

      int result =
      lua_pcall (L, 2, 1, 0);
      switch (result)
      {
      case LUA_ERRRUN:
        g_assert (lua_isstring (L, -1));
        lua_error (L);
        g_assert_not_reached ();
        break;
      case LUA_ERRERR:
        luaL_error (L, "recursive error");
        g_assert_not_reached ();
        break;
      case LUA_ERRMEM:
        luaL_error (L, "thread out of memory");
        g_assert_not_reached ();
        break;
      default:
        g_assert (result == LUA_OK);
        lua_pushvalue (L, 3);
        break;
      }
    }
return 1;
}

static int
lib_include (lua_State* L)
{
  const gchar* modname = NULL;
  int i, length = 0;
  int messages = 0;

  modname = luaL_checkstring (L, 1);

  lua_createtable (L, 0, 0);
  lua_insert (L, 2);

  lua_pushnumber (L, ++messages);
  lua_pushfstring (L, "module '%s' not found:", modname);
  lua_settable (L, 2);

  lua_pushliteral (L, MACROS_SEARCHERS);
  lua_gettable (L, LUA_REGISTRYINDEX);
  length = lua_objlen (L, -1);

  lua_pushcfunction (L, _include);
  lua_pushvalue (L, 1);

  for (i = 0; i < length; i++)
    {
      lua_pushnumber(L, i + 1);
      lua_gettable (L, -4);
      lua_pushvalue (L, 1);
      lua_call (L, 1, 2);

      if (lua_isfalse (L, -2))
        {
          lua_pushnumber (L, ++messages);
          lua_pushvalue (L, -2);
          lua_settable (L, 2);
          lua_pop (L, 2);
        }
      else
        {
          if (G_UNLIKELY (lua_isfunction (L, -2) == FALSE))
          { luaL_error (L, "searchers function should return a function");
            g_assert_not_reached (); }
          lua_call (L, 3, 2);
          return 2;
        }
    }

  lua_pop (L, 2);

  luaL_Buffer b;
  luaL_buffinit (L, &b);
  for (i = 0; i < messages; i++)
    {
      lua_pushnumber (L, i + 1);
      lua_gettable (L, 2);
      luaL_addvalue (&b);
    }

  luaL_pushresult (&b);
  lua_error (L);
  g_assert_not_reached ();
return 0;
}

/*
 * API
 * internal
 *
 */

static int
_default_searcher1 (lua_State* L)
{
  luaL_checkstring (L, 1);

  lua_pushliteral (L, MACROS_PRELOAD);
  lua_gettable (L, LUA_REGISTRYINDEX);
  lua_pushvalue (L, 1);
  lua_gettable (L, -2);

  if (lua_isnil (L, -1) == FALSE)
    {
      lua_pushliteral (L, ":preload:");
      return 2;
    }
return 0;
}

static const gchar*
pushnexttemplate (lua_State* L, const gchar* path)
{
  const char* l;
  while (*path == *LUA_PATHSEP)
    path++;
  if (*path == '\0')
    return NULL;
  l = strchr (path, *LUA_PATHSEP);
  if (l == NULL)
    l = path + strlen (path);
  lua_pushlstring (L, path, (size_t)(l - path));
return l;
}

static int
_searchpath_iter (lua_State* L)
{
  const gchar *modname = NULL;
  const gchar *template = NULL;
  const gchar *name = NULL;
  const gchar *sep, *rep;

  modname = luaL_checkstring (L, lua_upvalueindex (1));
  template = luaL_checkstring (L, lua_upvalueindex (2));
  sep = luaL_checkstring (L, lua_upvalueindex (3));
  rep = luaL_checkstring (L, lua_upvalueindex (4));

  template =
  pushnexttemplate (L, template);
  if (template == NULL)
    {
      lua_pushnil (L);
    }
  else
    {
      name = luaL_gsub (L, lua_tostring (L, -1), LUA_PATH_MARK, modname);
      lua_remove (L, -2);
      lua_pushstring (L, template);
      lua_replace (L, lua_upvalueindex (2));
    }
return 1;
}

static void
_searchpath (lua_State* L, const gchar* name, const gchar* path)
{
  const gchar *sep = ".";
  const gchar *dirsep = LUA_DIRSEP;
#if DEBUG == 1
  int top = lua_gettop (L);
#endif // DEBUG

  luaL_gsub (L, name, sep, dirsep);
  lua_pushstring (L, path);
  lua_pushstring (L, sep);
  lua_pushstring (L, dirsep);
  lua_pushcclosure (L, _searchpath_iter, 4);
#if DEBUG == 1
  g_assert (lua_gettop (L) == (top + 1));
#endif // DEBUG
}

static int
_search_closure (lua_State* L)
{
  int top = lua_gettop (L);
  lua_pushcfunction (L, lib_loadfile);
  lua_pushvalue (L, lua_upvalueindex (1));
  lua_pushnil (L);
  lua_pushliteral (L, "t");
  lua_pushliteral (L, LIBRARY);
  lua_gettable (L, LUA_REGISTRYINDEX);
  lua_pushliteral (L, "environ");
  lua_gettable (L, -2);
  lua_remove (L, -2);
  lua_call (L, 4, LUA_MULTRET);
return lua_gettop (L) - top;
}

G_DEFINE_QUARK
(g-file-peek-uri-cache,
 g_file_peek_uri);

const gchar*
g_file_peek_uri (GFile* file)
{
  g_return_if_fail (G_IS_FILE (file));
  const gchar* cached = NULL;
        gchar* uri = NULL;

  cached =
  g_object_get_qdata
  (G_OBJECT (file),
   g_file_peek_uri_quark ());
  if (G_UNLIKELY (cached == NULL))
  {
    uri = g_file_get_uri (file);
    g_object_set_qdata_full
    (G_OBJECT (file),
     g_file_peek_uri_quark (),
     uri,
     (GDestroyNotify)
     g_free);
    cached = uri;
  }
return cached;
}

static int
_default_searcher2 (lua_State* L)
{
  GFile* file = NULL;
  GFileInfo* info = NULL;
  GFileType type = 0;
  GError* tmp_err = NULL;
  const gchar* name = NULL;
  const gchar* uri = NULL;
  gchar* dynuri = NULL;
  int i, messages = 0;

  name = luaL_checkstring (L, 1);
  lua_createtable (L, 0, 0);
  lua_insert (L, 2);

  lua_pushliteral (L, MACROS);
  lua_gettable (L, LUA_REGISTRYINDEX);
  lua_pushliteral (L, "path");
  lua_gettable (L, -2);

  if (lua_isstring (L, -1) == FALSE)
    luaL_error(L, "macros.path must be a string");

  _searchpath (L, name, lua_tostring (L, -1));
  lua_insert (L, 3);
  lua_pop (L, 2);

  do
  {
    lua_pushvalue (L, 3);
    lua_call (L, 0, 1);
    if (lua_isnil (L, -1))
      {
        lua_pop (L, 1);
        break;
      }
    else
      {
        if (lua_isstring (L, -1) == FALSE)
          luaL_error (L, "wtf?");

        uri = lua_tostring (L, -1);
        file = g_file_new_for_commandline_arg (uri);
        if (G_UNLIKELY (file == NULL))
          {
            lua_pushnumber (L, ++messages);
            lua_pushfstring (L, "\r\n\tno file %s", uri);
            lua_settable (L, 2);
          }
        else
          {
            info =
            g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE, 0, NULL, &tmp_err);
            if (G_UNLIKELY (tmp_err != NULL))
              {
                _g_object_unref0 (info);

                if (g_error_matches (tmp_err, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
                  {
                    g_clear_error (&tmp_err);
                    lua_pushnumber (L, ++messages);
                    lua_pushfstring (L, "\r\n\tno file %s", g_file_peek_uri (file));
                    lua_settable (L, 2);
                  }
                else
                  {
                    _g_object_unref0 (file);
                    _limr_throwgerror (L, tmp_err);
                  }
              }
            else
              {
                type = g_file_info_get_file_type (info);
                if (type != G_FILE_TYPE_REGULAR)
                  {
                    lua_pushnumber (L, ++messages);
                    lua_pushfstring (L, "\r\n\tinvalid file %s", g_file_peek_uri (file));
                    lua_settable (L, 2);
                  }
                else
                  {
                    dynuri = g_file_get_uri (file);
                    lua_pushstring (L, dynuri);
                    lua_pushcclosure (L, _search_closure, 1);
                    _g_object_unref0 (info);
                    _g_object_unref0 (file);
                    _g_free0 (dynuri);
                    return 1;
                  }
              }
            _g_object_unref0 (info);
          }
        _g_object_unref0 (file);
      }
    lua_pop (L, 1);
  }
  while (TRUE);

  luaL_Buffer b;
  luaL_buffinit (L, &b);
  for (i = 0; i < messages; i++)
    {
      lua_pushnumber (L, (i + 1));
      lua_gettable (L, 2);
      luaL_addvalue (&b);
    }

  lua_pushnil (L);
  luaL_pushresult (&b);
return 2;
}

gboolean
(lua_istrue) (lua_State* L, int idx)
{
  if (lua_isboolean (L, idx))
    return lua_toboolean (L, idx);
  if (lua_isnumber (L, idx))
    return lua_tonumber (L, idx) != 0;
  if (lua_isnoneornil (L, idx))
    return FALSE;
return TRUE;
}

/*
 * Functions table
 *
 */

static luaL_Reg
functions[] =
{
  { "load", lib_load },
  { "loadfile", lib_loadfile },
  { "include", lib_include },
  { NULL, NULL },
};

/*
 * Library entry point
 *
 */

static void
create_macros (lua_State* L)
{
  lua_pushliteral (L, "macros");
  lua_createtable (L, 0, 3);
  {
    lua_pushliteral (L, MACROS_PRELOAD);
    lua_createtable (L, 0, 0);
    lua_pushliteral (L, MACROS_PRELOAD);
    lua_pushvalue (L, -2);
    lua_settable (L, LUA_REGISTRYINDEX);
    lua_settable (L, -3);

    lua_pushliteral (L, "searchers");
    lua_createtable (L, 2, 0);
    {
      lua_pushnumber (L, 1);
      lua_pushcfunction (L, _default_searcher1);
      lua_settable (L, -3);
      lua_pushnumber (L, 2);
      lua_pushcfunction (L, _default_searcher2);
      lua_settable (L, -3);
    }

    lua_pushliteral (L, MACROS_SEARCHERS);
    lua_pushvalue (L, -2);
    lua_settable (L, LUA_REGISTRYINDEX);
    lua_settable (L, -3);

    lua_pushliteral (L, "path");
    lua_pushliteral (L, LUA_PATH_DEFAULT);
    lua_settable (L, -3);
  }

  lua_pushliteral (L, MACROS);
  lua_pushvalue (L, -2);
  lua_settable (L, LUA_REGISTRYINDEX);
  lua_settable (L, -3);
}

static void
patch_global (lua_State* L)
{
  lua_pushliteral (L, "checkArg");
  lua_pushcfunction (L, lib_checkarg);
  lua_settable (L, LUA_GLOBALSINDEX);
}

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
#if DEBUG == 1
  int top = lua_gettop (L);
#endif // DEBUG
  _limr_stream_init (L);
  patch_global (L);
#if LUA_VERSION_NUM >= 502
  luaL_newlib (L, functions);
#else // LUA_VERSION_NUM < 502
  lua_newtable (L);
  luaL_register (L, NULL, functions);
#endif // LUA_VERSION_NUM
  lua_pushliteral (L, LIBRARY);
  lua_pushvalue (L, -2);
  lua_settable (L, LUA_REGISTRYINDEX);
  create_macros (L);
  set_info (L);
  patch_library (L);
#if DEBUG == 1
  g_assert (lua_gettop (L) == (top + 1));
#endif // DEBUG
return 1;
}
