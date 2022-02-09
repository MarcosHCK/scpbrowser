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
#include <glib-object.h>
#include <limr_state_patch.h>

GType
limr_state_get_type (void) G_GNUC_CONST;
#define LIMR_IS_STATE(var) (G_TYPE_CHECK_INSTANCE_TYPE ((var), limr_state_get_type ()))

/*
 * API
 *
 */

G_GNUC_INTERNAL
int
_limr_vm_init (lua_State* L)
{
  gpointer state = lua_touserdata (L, 1);
  g_assert (LIMR_IS_STATE (state));

  luaL_openlibs (L);
  lua_pushvalue (L, 1);
  _limr_data_init (L);
  _limr_proc_init (L);
return 0;
}

G_GNUC_INTERNAL
int
_limr_vm_setfenv (lua_State* L, int idx)
{
  return lua_setfenv (L, idx);
}

G_GNUC_NORETURN
G_GNUC_INTERNAL
void
_limr_vm_throwgerror (lua_State* L, GError* error)
{
  lua_pushfstring
  (L,
   "%s: %i: %s",
   g_quark_to_string (error->domain),
   error->code,
   error->message);
  g_error_free (error);
  lua_error (L);
for (;;);
}
