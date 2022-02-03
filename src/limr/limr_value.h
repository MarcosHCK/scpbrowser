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
#ifndef __LIMR_VALUE__
#define __LIMR_VALUE__ 1
#include <glib-object.h>

#define _IO_VALUE "__LIMR_IO_VALUE"
typedef struct _CleanupValue CleanupValue;

#define _g_object_unref0(var) \
  ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) \
  ((var == NULL) ? NULL : (var = (g_free (var), NULL)))

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

struct _CleanupValue
{
  GType gtype;
  gpointer object;
  GDestroyNotify notify;
};

G_GNUC_INTERNAL
void
_limr_value_create_meta (lua_State* L);
G_GNUC_INTERNAL
CleanupValue*
luaL_createcvalue (lua_State* L, gpointer object, GDestroyNotify notify);
G_GNUC_INTERNAL
CleanupValue*
luaL_pushcvalue (lua_State* L, gpointer object, GDestroyNotify notify);
G_GNUC_INTERNAL
CleanupValue*
luaL_checkcvalue (lua_State* L, int idx, GType gtype);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_VALUE__
