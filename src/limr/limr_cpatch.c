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
  g_return_val_if_fail (G_IS_FILE (file), NULL);
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

/*
 * Version
 *
 */

static guint
uint_parse (const gchar* str, guint _base)
{
	guint result = 0U;
	g_return_val_if_fail (str != NULL, 0U);
	result = (guint) strtoul (str, NULL, (gint) _base);
	return result;
}

static guint _mayor_version = -1;
static guint _minor_version = -1;
static guint _micro_version = -1;

G_MODULE_EXPORT
guint
limr_get_major_version (void)
{
	guint result = 0U;
	if (G_UNLIKELY (_mayor_version == ((guint) -1)))
    {
		  _mayor_version = uint_parse (PACKAGE_VERSION_MAYOR, (guint) 0);
	  }
return _mayor_version;
}

G_MODULE_EXPORT
guint
limr_get_minor_version (void)
{
	guint result = 0U;
	if (G_UNLIKELY (_minor_version == ((guint) -1)))
    {
		_minor_version = uint_parse (PACKAGE_VERSION_MINOR, (guint) 0);
	  }
return _minor_version;
}

G_MODULE_EXPORT
guint
limr_get_micro_version (void)
{
	guint result = 0U;
	if (G_UNLIKELY (_micro_version == ((guint) -1)))
    {
		_micro_version = uint_parse (PACKAGE_VERSION_MICRO, (guint) 0);
	  }
return _micro_version;
}
