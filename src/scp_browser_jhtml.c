/* Copyright 2021-2025 MarcosHCK
 * This file is part of scpbrowser.
 *
 * scpbrowser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scpbrowser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with scpbrowser.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <config.h>
#include <scp_browser_internl.h>
#include <liblimr.h>

G_DEFINE_QUARK
(scp-jhtml-parser-private-data,
 state_store);

G_GNUC_INTERNAL
GBytes*
_scp_browser_compile_jhtml (ScpBrowser* self, GBytes* input_, const gchar* path, GError** error)
{
  GCancellable* cancellable = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;

/*
 * Prepare objects
 *
 */

  static
  LimrHost* host = NULL;
  if (G_UNLIKELY (host == NULL))
  {
    host =
    limr_host_new (cancellable, &tmp_err);
    if (G_UNLIKELY (tmp_err != NULL))
      {
        g_propagate_error (error, tmp_err);
        g_clear_object (&host);
        return NULL;
      }

    bytes =
    g_resources_lookup_data (GRESNAME "/lua/patch.lua", 0, &tmp_err);
    if (G_UNLIKELY (tmp_err != NULL))
      {
        g_propagate_error (error, tmp_err);
        g_clear_object (&host);
        return NULL;
      }

    success =
    limr_host_inject (host, bytes, cancellable, &tmp_err);
    if (G_UNLIKELY (tmp_err != NULL))
      {
        g_propagate_error (error, tmp_err);
        g_clear_object (&host);
        return NULL;
      }
    _g_bytes_unref0 (bytes);

    g_object_set_qdata_full
    (G_OBJECT (self),
     state_store_quark (),
     g_object_ref (host),
     g_object_unref);
  }
  else
  {
    g_object_ref (host);
  }

/*
 * Execute code
 *
 */

  bytes =
  limr_host_parse (host, input_, cancellable, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    goto_error ();
  }

_error_:
  _g_object_unref0 (host);
  if (G_UNLIKELY (success == FALSE))
    _g_bytes_unref0 (bytes);
return bytes;
}
