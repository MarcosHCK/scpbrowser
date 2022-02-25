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
#include <private.h>

G_GNUC_INTERNAL G_DEFINE_QUARK
(g-file-peek-uri-cache,
 g_file_peek_uri);

G_GNUC_INTERNAL
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

G_GNUC_INTERNAL
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
