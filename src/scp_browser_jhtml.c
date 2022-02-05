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

G_GNUC_INTERNAL
GBytes*
_scp_browser_compile_jhtml (ScpBrowser* self, GBytes* input_, const gchar* path, GError** error)
{
  GCancellable* cancellable = NULL;
  GOutputStream* endpoint = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;

  endpoint =
  g_memory_output_stream_new_resizable ();

/*
 * Execute code
 *
 */

  success =
  limr_bridge_client_parse (self->limr_client, endpoint, input_, NULL, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    goto_error ();
  }

/*
 * Access data
 *
 */

  success =
  g_output_stream_close (endpoint, cancellable, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_propagate_error (error, tmp_err);
    goto_error ();
  }

  bytes =
  g_memory_output_stream_steal_as_bytes (G_MEMORY_OUTPUT_STREAM (endpoint));
  if (G_UNLIKELY (bytes == NULL))
  {
    g_set_error_literal
    (error,
     SCP_BROWSER_ERROR,
     SCP_BROWSER_ERROR_JHTML_COMPILING,
     "Empty stream as output");
    goto_error ();
  }

_error_:
  _g_object_unref0 (endpoint);
  if (G_UNLIKELY (success == FALSE))
    _g_bytes_unref0 (bytes);
return bytes;
}
