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
#include <limr_value.h>

/*
 * Metatable
 *
 */

static int
value__tostring (lua_State* L)
{
  CleanupValue* value =
  luaL_checkudata (L, 1, _IO_VALUE);

  lua_pushfstring
  (L,
   _IO_VALUE ": %p",
   (guintptr)
   value->object);
return 1;
}

static int
value__gc (lua_State* L)
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
value_mt[] =
{
  { "__tostring", value__tostring },
  { "__gc", value__gc },
  {NULL, NULL},
};

G_GNUC_INTERNAL
void
_limr_value_create_meta (lua_State* L)
{
  luaL_newmetatable (L, _IO_VALUE);
  luaL_setfuncs (L, value_mt, 0);

  /* __name */
  lua_pushliteral (L, "__name");
  lua_pushliteral (L, _IO_VALUE);
  lua_settable (L, -3);

  /* end */
  lua_pop (L, 1);
}

/*
 * API
 *
 */

G_GNUC_INTERNAL
CleanupValue*
luaL_createcvalue (lua_State* L, gpointer object, GDestroyNotify notify)
{
  CleanupValue* value;
  value = lua_newuserdata (L, sizeof (CleanupValue));
  value->gtype = G_TYPE_POINTER;
  value->object = object;
  value->notify = notify;
return value;
}

G_GNUC_INTERNAL
CleanupValue*
luaL_pushcvalue (lua_State* L, gpointer object, GDestroyNotify notify)
{
  CleanupValue* value;
  value = luaL_createcvalue (L, object, notify);
  luaL_setmetatable (L, _IO_VALUE);
return value;
}

G_GNUC_INTERNAL
CleanupValue*
luaL_checkcvalue (lua_State* L, int idx, GType gtype)
{
  CleanupValue* value = NULL;
  value = luaL_checkudata (L, idx, _IO_VALUE);
  if (!g_type_check_instance_is_a (value->object, gtype))
  {
    luaL_typerror (L, idx, g_type_name (gtype));
  }
return value;
}
