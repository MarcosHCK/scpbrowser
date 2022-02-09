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
#include <limr_proc.h>
#include <limr_state_patch.h>

#define LHELPER GRESNAME "/lua/patch.lua"

#define THROW_GERROR(L, error) (_limr_vm_throwgerror ((L), (error)))
#define _g_bytes_unref0(var) ((var == NULL) ? NULL : (var = (g_bytes_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) ((var == NULL) ? NULL : (var = (g_free (var), NULL)))

G_GNUC_INTERNAL
gint
_limr_parser_parse_bytes (lua_State* L,
                          GBytes* bytes,
                          const gchar* chunkname,
                          const gchar* mode);

/*
 * Global patches
 *
 */

G_GNUC_INTERNAL
int
_limr_proc_print (lua_State* L)
{
  int i, top = lua_gettop (L);
  lua_getfield (L, LUA_ENVIRONINDEX, "tostring");
  lua_getfield (L, LUA_ENVIRONINDEX, "io");
  lua_getfield (L, -1, "stdout");
  lua_remove (L, -2);

  if (G_UNLIKELY (lua_isnil (L, -1)))
    luaL_error (L, "can't get standard output");
  if (G_UNLIKELY (!lua_isuserdata (L, -1)))
    luaL_error (L, "Invalid standard output (got %s)", luaL_typename (L, -1));
  lua_getfield (L, -1, "write");
  if (G_UNLIKELY (lua_isfunction (L, -1) == FALSE))
      luaL_error (L, "standard output hasn't write method");

  /* 1 -> top : arguments       */
  /* top + 1  : tostring        */
  /* top + 2  : io.stdout       */
  /* top + 3  : io.stdout:write */

  for (i = 1; i < (top + 1); i++)
  {
    lua_pushvalue (L, top + 3);
    lua_pushvalue (L, top + 2);
    lua_pushvalue (L, top + 1);
    lua_pushvalue (L, i);

    /* top + 4 : io.stdout:write  */
    /* top + 5 : io.stdout        */
    /* top + 6 : tostring         */
    /* top + 7 : arguments[i]     */
    lua_call (L, 1, 1);

    if (!lua_isstring (L, -1))
      luaL_error (L, "can't serialize argument #%i", i);

    if (i == top)
      lua_pushliteral (L, "\r\n");
    else
      lua_pushliteral (L, "\t");

    /* top + 4 : io.stdout:write          */
    /* top + 5 : io.stdout                */
    /* top + 6 : tostring (arguments[i])  */
    /* top + 7 : "\r\n" or "\t"           */
    lua_call (L, 3, 0);
  }
return 0;
}

G_GNUC_INTERNAL
int
_limr_proc_printerr (lua_State* L)
{
  int i, top = lua_gettop (L);
  lua_getfield (L, LUA_ENVIRONINDEX, "tostring");
  lua_getfield (L, LUA_ENVIRONINDEX, "io");
  lua_getfield (L, -1, "stderr");
  lua_remove (L, -2);

  if (G_UNLIKELY (lua_isnil (L, -1)))
    luaL_error (L, "can't get standard output");
  if (G_UNLIKELY (!lua_isuserdata (L, -1)))
    luaL_error (L, "Invalid standard output (got %s)", luaL_typename (L, -1));
  lua_getfield (L, -1, "write");
  if (G_UNLIKELY (lua_isfunction (L, -1) == FALSE))
      luaL_error (L, "standard output hasn't write method");

  /* 1 -> top : arguments       */
  /* top + 1  : tostring        */
  /* top + 2  : io.stderr       */
  /* top + 3  : io.output:write */

  for (i = 1; i < (top + 1); i++)
  {
    lua_pushvalue (L, top + 3);
    lua_pushvalue (L, top + 2);
    lua_pushvalue (L, top + 1);
    lua_pushvalue (L, i);

    /* top + 4 : io.stderr:write  */
    /* top + 5 : io.stderr        */
    /* top + 6 : tostring         */
    /* top + 7 : arguments[i]     */
    lua_call (L, 1, 1);

    if (!lua_isstring (L, -1))
      luaL_error (L, "can't serialize argument #%i", i);

    if (i == top)
      lua_pushliteral (L, "\r\n");
    else
      lua_pushliteral (L, "\t");

    /* top + 4 : io.stderr:write          */
    /* top + 5 : io.stderr                */
    /* top + 6 : tostring (arguments[i])  */
    /* top + 7 : "\r\n" or "\t"           */
    lua_call (L, 3, 0);
  }
return 0;
}

G_GNUC_INTERNAL
int
_limr_proc_pack (lua_State* L)
{
  int i, top = lua_gettop (L);
  lua_createtable (L, top, 0);
  for (i = 1; i < (top + 1); i++)
  {
    lua_pushinteger (L, i);
    lua_pushvalue (L, i);
    lua_settable (L, -3);
  }
return 1;
}

/*
 * Library
 *
 */

G_GNUC_INTERNAL
int
_limr_proc_resources_exists (lua_State* L)
{
  const gchar* resname = NULL;
  gboolean exists = FALSE;
  GError* tmp_err = NULL;

  resname = luaL_checkstring (L, 1);
  exists = g_resources_get_info (resname, 0, NULL, NULL, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    if (!g_error_matches (tmp_err, G_RESOURCE_ERROR, G_RESOURCE_ERROR_NOT_FOUND))
    {
      THROW_GERROR (L, tmp_err);
      g_assert_not_reached ();
    }
    else
    {
      _g_error_free0 (tmp_err);
      exists = FALSE;
    }
  }

  lua_pushboolean (L, exists);
return 1;
}

G_GNUC_INTERNAL
int
_limr_proc_resources_lookup (lua_State* L)
{
  const gchar* resname = NULL;
  const gchar* data = NULL;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  gsize length = 0;

  resname = luaL_checkstring (L, 1);
  bytes = g_resources_lookup_data (resname, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  data = g_bytes_get_data (bytes, &length);
  lua_pushlstring (L, data, length);
  _g_bytes_unref0 (bytes);
return 1;
}

/*
 * loadresource (resname : string, chunkname : string, mode : string, environ : table or number, parse : boolean)
 * -> chunk : function
 * or
 * -> successed : boolean
 * -> reason : string
 *
 */

static int
load_bytes (lua_State* L, GBytes* bytes, const gchar* chunkname, const gchar* mode, gboolean parse)
{
  const gchar* data = NULL;
  gsize length = 0;
  int result;

  data = g_bytes_get_data (bytes, &length);

  if (parse == FALSE)
    result = luaL_loadbufferx (L, data, length, chunkname, mode);
  else
    result = _limr_parser_parse_bytes (L, bytes, chunkname, mode);

  switch (result)
  {
  case LUA_ERRSYNTAX:
    g_assert (lua_isstring (L, -1));
    lua_pushboolean (L, FALSE);
    lua_insert (L, -2);
    return 2;
  case LUA_ERRMEM:
    g_warning ("Lua thread out of memory");
    lua_settop (L, 0);
    lua_gc (L, LUA_GCCOLLECT, TRUE);
    lua_pushboolean (L, FALSE);
    lua_pushstring (L, "Lua thread out of memory");
    return 2;
  default:
    g_assert (result == LUA_OK);
    if (!lua_isnoneornil (L, 4))
    { g_assert (lua_isfunction (L, -1));
      lua_pushvalue (L, 4);
      g_assert (lua_setfenv (L, -2) == 1); }
    return 1;
  }
return 0;
}

G_GNUC_INTERNAL
int
_limr_proc_loadresource (lua_State* L)
{
  const gchar* chunkname = NULL;
  const gchar* resname = NULL;
  const gchar* mode = NULL;
  gboolean parse = FALSE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  int result;

  resname = luaL_checkstring (L, 1);
  if (!lua_isnoneornil (L, 2))
    chunkname = lua_tostring (L, 2);
  else
  {
    gchar* basename_ =
    g_path_get_basename (resname);
    if (G_UNLIKELY (basename_ == NULL))
      chunkname = lua_pushfstring (L, "=%s", resname);
    else
    {
      chunkname = lua_pushfstring (L, "=%s", basename_);
      _g_free0 (basename_);
    }
  }

  mode = luaL_optstring (L, 3, "t");
  if (!lua_isnoneornil (L, 4))
    if (!lua_istable (L, 4))
      luaL_typerror (L, 4, "table");
  parse = (!lua_isboolean (L, 5))
          ? FALSE
          : lua_toboolean (L, 5);

  bytes = g_resources_lookup_data (resname, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    lua_pushboolean (L, FALSE);
    lua_pushstring (L, tmp_err->message);
    _g_error_free0 (tmp_err);
    _g_bytes_unref0 (bytes);
    return 1;
  }

  result = load_bytes (L, bytes, chunkname, mode, parse);
  _g_bytes_unref0 (bytes);
return result;
}

G_GNUC_INTERNAL
int
_limr_proc_loadstring (lua_State* L)
{
  const gchar* data = NULL;
  const gchar* chunkname = NULL;
  const gchar* mode = NULL;
  gboolean parse = FALSE;
  GBytes* bytes = NULL;
  size_t length = 0;
  int result;

  data = luaL_checklstring (L, 1, &length);
  chunkname = luaL_optstring (L, 2, "=(load)");
  mode = luaL_optstring (L, 3, "t");
  if (!lua_isnoneornil (L, 4))
    if (!lua_istable (L, 4))
      luaL_typerror (L, 4, "table");
  parse = (!lua_isboolean (L, 5))
          ? FALSE
          : lua_toboolean (L, 5);
  bytes = g_bytes_new_static (data, length);
  result = load_bytes (L, bytes, chunkname, mode, parse);
  _g_bytes_unref0 (bytes);
return result;
}

G_GNUC_INTERNAL
int
_limr_proc_loadfile (lua_State* L)
{
  const gchar* filename = NULL;
  const gchar* chunkname = NULL;
  const gchar* mode = NULL;
  gboolean parse = FALSE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  int result;

  filename = luaL_checkstring (L, 1);
  if (!lua_isnoneornil (L, 2))
    chunkname = lua_tostring (L, 2);
  else
  {
    gchar* basename_ =
    g_path_get_basename (filename);
    if (G_UNLIKELY (basename_ == NULL))
      chunkname = lua_pushfstring (L, "=%s", filename);
    else
    {
      chunkname = lua_pushfstring (L, "=%s", basename_);
      _g_free0 (basename_);
    }
  }

  mode = luaL_optstring (L, 3, "t");
  if (!lua_isnoneornil (L, 4))
    if (!lua_istable (L, 4))
      luaL_typerror (L, 4, "table");
  parse = (!lua_isboolean (L, 5))
          ? FALSE
          : lua_toboolean (L, 5);

  GFile* file =
  g_file_new_for_commandline_arg (filename);
  bytes =
  g_file_load_bytes (file, NULL, NULL, &tmp_err);
  g_object_unref (file);

  if (G_UNLIKELY (tmp_err != NULL))
  {
    lua_pushboolean (L, FALSE);
    lua_pushstring (L, tmp_err->message);
    _g_error_free0 (tmp_err);
    _g_bytes_unref0 (bytes);
    return 1;
  }

  result = load_bytes (L, bytes, chunkname, mode, parse);
  _g_bytes_unref0 (bytes);
return result;
}

/*
 * Init function
 *
 */

static int
__newindex (lua_State* L)
{
return 0;
}

G_GNUC_INTERNAL
int
_limr_proc_init (lua_State* L)
{
  /* global patches */
  lua_pushliteral (L, "print");
  lua_pushcfunction (L, _limr_proc_print);
  lua_settable (L, LUA_GLOBALSINDEX);
  lua_pushliteral (L, "printerr");
  lua_pushcfunction (L, _limr_proc_printerr);
  lua_settable (L, LUA_GLOBALSINDEX);
  lua_pushliteral (L, "pack");
  lua_pushcfunction (L, _limr_proc_pack);
  lua_settable (L, LUA_GLOBALSINDEX);

/*
 * 'limr' library
 *
 */

  lua_pushliteral (L, "limr");
  lua_createtable (L, 0, 2);
  {
    lua_pushliteral (L, "resources");
    lua_createtable (L, 0, 4);
    {
      lua_pushliteral (L, "exists");
      lua_pushcfunction (L, _limr_proc_resources_exists);
      lua_settable (L, -3);             /*  1.1 */
      lua_pushliteral (L, "lookup");
      lua_pushcfunction (L, _limr_proc_resources_lookup);
      lua_settable (L, -3);             /*  1.2 */
    }
    lua_settable (L, -3);               /*  1 */
    lua_pushliteral (L, "loadresource");
    lua_pushcfunction (L, _limr_proc_loadresource);
    lua_settable (L, -3);               /*  2 */
    lua_pushliteral (L, "loadstring");
    lua_pushcfunction (L, _limr_proc_loadstring);
    lua_settable (L, -3);               /*  3 */
    lua_pushliteral (L, "loadfile");
    lua_pushcfunction (L, _limr_proc_loadfile);
    lua_settable (L, -3);               /*  4 */
  }
  lua_createtable (L, 0, 2);
  lua_pushliteral (L, "__metatable");
  lua_pushliteral (L, "metatable");
  lua_settable (L, -3);
  lua_pushliteral (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_settable (L, -3);
  lua_setmetatable (L, -2);
  lua_settable (L, LUA_GLOBALSINDEX);

/*
 * Call native helper
 *
 */

  lua_pushcfunction (L, _limr_proc_loadresource);
  lua_pushliteral (L, LHELPER);
  lua_call (L, 1, 2);
  if (!lua_isfunction (L, -2))
    lua_error (L);
  lua_pop (L, 1);

  lua_call (L, 0, 1);
  g_assert (lua_istable (L, -1));
  lua_setfield (L, LUA_REGISTRYINDEX, LENVIRON);
return 0;
}
