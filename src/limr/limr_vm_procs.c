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
#include <limr_vm_procs.h>

#define PATCH_FILE (GRESNAME "/lua/patch.lua")

gboolean
limr_state_load_bytes (gpointer state, GBytes* bytes, GCancellable* cancellable, GError** error);
void
limr_state_set_rpath (gpointer state, const gchar*);
const gchar*
limr_state_get_rpath (gpointer state);
int
_limr_throwrap (lua_State* L);

#define _g_bytes_unref0(var) ((var == NULL) ? NULL : (var = (g_bytes_unref (var), NULL)))
#define _g_free0(var) ((var == NULL) ? NULL : (var = (g_free (var), NULL)))

G_GNUC_INTERNAL
int
limr_proc_print (lua_State* L)
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
limr_proc_printerr (lua_State* L)
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
limr_proc_resource_exists (lua_State* L)
{
  const gchar* resname = NULL;
  gchar* fullpath = NULL;
  GError* tmp_err = NULL;
  gboolean exists = TRUE;

  resname =
  luaL_checkstring (L, 1);
  fullpath =
  g_canonicalize_filename (resname, GRESNAME);
  exists =
  g_resources_get_info (fullpath, 0, NULL, NULL, &tmp_err);
  _g_free0 (fullpath);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    if (g_error_matches (tmp_err, G_RESOURCE_ERROR, G_RESOURCE_ERROR_NOT_FOUND))
    {
      g_error_free (tmp_err);
      exists = FALSE;
    }
    else
    {
      THROW_GERROR (L, tmp_err);
      g_assert_not_reached ();
    }
  }

  lua_pushboolean (L, exists);
return 1;
}

G_GNUC_INTERNAL
int
limr_proc_resource_lookup (lua_State* L)
{
  const gchar* resname = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  const gchar* data = NULL;
  gsize size = 0;

  resname =
  luaL_checkstring (L, 1);
  bytes =
  g_resources_lookup_data (resname, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    _g_bytes_unref0 (bytes);
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  data = g_bytes_get_data (bytes, &size);
  lua_pushlstring (L, data, size);
  _g_bytes_unref0 (bytes);
return 1;
}

G_GNUC_INTERNAL
int
limr_proc_resource_load (lua_State* L)
{
  const gchar* resname = NULL;
  const gchar* mode = NULL;
  gchar* chunkname = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  const gchar* data = NULL;
  gsize size = 0;
  int result;

  resname =
  luaL_checkstring (L, 1);
  bytes =
  g_resources_lookup_data (resname, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    _g_bytes_unref0 (bytes);
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  data = g_bytes_get_data (bytes, &size);
  if (!lua_isnoneornil (L, 2))
    chunkname = g_strdup (luaL_checkstring (L, 2));
  else
    chunkname = g_strconcat ("=", resname, NULL);
  if (!lua_isnoneornil (L, 3))
    mode = luaL_checkstring (L, 3);
  else
    mode = "t";

  result =
  luaL_loadbufferx (L, data, size, chunkname, mode);
  _g_bytes_unref0 (bytes);
  _g_free0 (chunkname);
  IF_LUA_ERROR (L, result)
    lua_error (L);

  if (!lua_isnoneornil (L, 4))
  { lua_pushvalue (L, 4);
    lua_setfenv (L, -2); }
return 1;
}

G_GNUC_INTERNAL
int
limr_proc_resource_parse (lua_State* L)
{
  const gchar* resname = NULL;
  const gchar* mode = NULL;
  gchar* chunkname = NULL;
  gpointer instance = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  int result;

  resname =
  luaL_checkstring (L, 1);
  bytes =
  g_resources_lookup_data (resname, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    _g_bytes_unref0 (bytes);
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }

  if (!lua_isnoneornil (L, 2))
    chunkname = g_strdup (luaL_checkstring (L, 2));
  else
    chunkname = g_strconcat ("=", resname, NULL);
  if (!lua_isnoneornil (L, 3))
    mode = luaL_checkstring (L, 3);
  else
    mode = "t";

  instance =
  lua_touserdata (L, lua_upvalueindex (1));
  result =
  limr_state_load_bytes (instance, bytes, NULL, &tmp_err);
  _g_bytes_unref0 (bytes);
  _g_free0 (chunkname);
  IF_LUA_ERROR (L, result)
    lua_error (L);

  if (!lua_isnoneornil (L, 4))
  { lua_pushvalue (L, 4);
    lua_setfenv (L, -2); }
return 1;
}

G_GNUC_INTERNAL
int
limr_proc_pack (lua_State* L)
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

G_GNUC_INTERNAL
int
limr_proc_parse (lua_State* L)
{
  const gchar* source = NULL;
  gpointer instance = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  gsize length = 0;

  source = luaL_checklstring (L, 1, &length);
  bytes = g_bytes_new_static (source, length);
  instance = lua_touserdata(L, lua_upvalueindex(1));

  success =
  limr_state_load_bytes (instance, bytes, NULL, &tmp_err);
  _g_bytes_unref0 (bytes);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    THROW_GERROR (L, tmp_err);
    g_assert_not_reached ();
  }
return 1;
}

static const gchar*
pushnexttemplate (lua_State* L, const char* path)
{
  const char* l;
  while (*path == *LUA_PATHSEP)
    path++;
  if (*path == '\0')
    return NULL;
  l = strchr(path, *LUA_PATHSEP);
  if (l == NULL)
    l = path + strlen(path);
  lua_pushlstring(L, path, (size_t)(l - path));
return l;
}

static int
_searchpath_iter (lua_State* L)
{
  const gchar* modname = NULL;
  const gchar* template = NULL;
  const gchar* name = NULL;
  const gchar *sep, *rep;

  modname = luaL_checkstring (L, lua_upvalueindex (1));
  template = luaL_checkstring (L, lua_upvalueindex (2));
  sep = luaL_checkstring (L, lua_upvalueindex (3));
  rep = luaL_checkstring (L, lua_upvalueindex (4));

  template = pushnexttemplate (L, template);
  if (template == NULL)
    lua_pushnil (L);
  else
  {
    name = luaL_gsub (L, lua_tostring (L, -1), LUA_PATH_MARK, modname);
    lua_remove (L, -2);
    lua_pushstring (L, template);
    lua_replace (L, lua_upvalueindex (2));
  }
return 1;
}

G_GNUC_INTERNAL
int
limr_proc_searchpath (lua_State* L)
{
  luaL_checkstring (L, 1);
  lua_pushvalue (L, 1);
  luaL_checkstring (L, 2);
  lua_pushvalue (L, 2);

  if (lua_isnoneornil (L, 3))
  { lua_pushliteral (L, ".");
    lua_replace (L, 3); }
  else
    luaL_checkstring (L, 3);
  lua_pushvalue (L, 3);
  if (lua_isnoneornil (L, 4))
  { lua_pushliteral (L, LUA_DIRSEP);
    lua_replace (L, 4); }
  else
    luaL_checkstring (L, 4);
  lua_pushvalue (L, 4);

  lua_pushcclosure (L, _searchpath_iter, 4);
return 1;
}

/*
 * Metamethods
 *
 */

static int
__index (lua_State* L)
{
  const gchar* idx;
  idx = luaL_checkstring (L, 2);

  if (!g_strcmp0 (idx, "rpath"))
  {
    gpointer state = NULL;
    const gchar* rpath = NULL;

    state = lua_touserdata (L, lua_upvalueindex (1));
    rpath = limr_state_get_rpath (state);
    lua_pushstring (L, rpath);
    return 1;
  }
return 0;
}

static int
__newindex (lua_State* L)
{
  const gchar* idx;
  idx = luaL_checkstring (L, 2);

  if (!g_strcmp0 (idx, "rpath"))
  {
    gpointer state = NULL;
    const gchar* rpath = NULL;

    state = lua_touserdata (L, lua_upvalueindex (1));
    rpath = luaL_checkstring (L, 3);
    limr_state_set_rpath (state, rpath);
  }
return 0;
}

/*
 * Procedures emitter
 *
 */

G_GNUC_INTERNAL
int
limr_procs_emit (lua_State* L, gpointer state, GError** error)
{
  GBytes* bytes = NULL;
  GError* tmp_err = NULL;
  const gchar* data = NULL;
  gsize size = 0;
  int result;

  /*
   * limr Lua side library
   *
   */

  lua_createtable (L, 0, 4);
  {
    lua_pushliteral (L, "resources");
    lua_createtable (L, 0, 3);
    {
      lua_pushliteral (L, "exists");
      lua_pushcfunction (L, limr_proc_resource_exists);
      lua_settable (L, -3);           /*  1.1 */

      lua_pushliteral (L, "lookup");
      lua_pushcfunction (L, limr_proc_resource_lookup);
      lua_settable (L, -3);           /*  1.2 */

      lua_pushliteral (L, "load");
      lua_pushcfunction (L, limr_proc_resource_load);
      lua_settable (L, -3);           /*  1.3 */

      lua_pushliteral (L, "parse");
      lua_pushlightuserdata (L, state);
      lua_pushcclosure (L, limr_proc_resource_parse, 1);
      lua_settable (L, -3);
    }
    lua_settable (L, -3);             /*  1 */

    lua_pushliteral (L, "pack");
    lua_pushcfunction (L, limr_proc_pack);
    lua_settable (L, -3);             /*  2 */

    lua_pushliteral (L, "parse");
    lua_pushlightuserdata (L, state);
    lua_pushcclosure (L, limr_proc_parse, 1);
    lua_settable (L, -3);             /*  3 */

    lua_pushliteral (L, "searchpath");
    lua_pushcfunction (L, limr_proc_searchpath);
    lua_settable (L, -3);             /*  4 */
  }
  lua_createtable (L, 0, 1);
  lua_pushliteral (L, "__index");
  lua_pushlightuserdata (L, state);
  lua_pushcclosure (L, __index, 1);
  lua_settable (L, -3);
  lua_pushliteral (L, "__newindex");
  lua_pushlightuserdata (L, state);
  lua_pushcclosure (L, __newindex, 1);
  lua_settable (L, -3);
  lua_setmetatable (L, -2);
  lua_setglobal (L, "limr");

  /*
   * Patch Lua side functions with C side's ones
   *
   */

  lua_pushcfunction (L, limr_proc_print);
  lua_setglobal (L, "print");
  lua_pushcfunction (L, limr_proc_printerr);
  lua_setglobal (L, "printerr");

  /*
   * Patch Lua side functions with Lua side's ones
   *
   */

  bytes =
  g_resources_lookup_data (PATCH_FILE, 0, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    goto _error_no_pop_;
  }

  data =
  g_bytes_get_data (bytes, &size);
  lua_pushcfunction (L, _limr_throwrap);

  result =
  luaL_loadbuffer (L, data, size, "=patch");
  IF_LUA_ERROR (L, result)
  {
    g_set_error
    (error,
     LIMR_STATE_PATCH_ERROR,
     LIMR_STATE_PATCH_ERROR_SYNTAX,
     "%s",
     lua_tostring (L, -1));
    lua_pop (L, 1);
    goto _error_;
  }

  result =
  lua_pcall (L, 0, 1, -2);
  IF_LUA_ERROR (L, result)
  {
    g_set_error
    (error,
     LIMR_STATE_PATCH_ERROR,
     LIMR_STATE_PATCH_ERROR_RUN,
     "%s",
     lua_tostring (L, -1));
    lua_pop (L, 1);
    goto _error_;
  }

  if (G_UNLIKELY (!lua_istable (L, -1)))
  { g_critical ("unexpected patch library return value");
   g_assert_not_reached (); }
  lua_getfield (L, -1, "environ");
  if (G_UNLIKELY (!lua_istable (L, -1)))
  { g_critical ("missing mandatory environment table");
   g_assert_not_reached (); }
  lua_setfield (L, LUA_REGISTRYINDEX, LENVIRON);
  lua_pop (L, 1);

_error_:
  lua_pop (L, 1);
_error_no_pop_:
return 0;
}
