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
#include <limr/liblimr.h>

G_DEFINE_QUARK
(scp-jhtml-parser-private-data,
 state_store);

G_GNUC_INTERNAL
GBytes*
_scp_browser_compile_jhtml (ScpBrowser* self, GBytes* input_, const gchar* path, GError** error)
{
  GCancellable* cancellable = NULL;
  GOutputStream* endpoint = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;

/*
 * Prepare objects
 *
 */

  static
  LimrState* state = NULL;
  if (G_UNLIKELY (state == NULL))
  {
    state =
    limr_state_new (cancellable, &tmp_err);
    if (G_UNLIKELY (tmp_err != NULL))
    {
      g_propagate_error (error, tmp_err);
      g_clear_object (&state);
      return NULL;
    }

    g_object_set_qdata_full
    (G_OBJECT (self),
     state_store_quark (),
     g_object_ref (state),
     g_object_unref);
  }
  else
  {
    g_object_ref (state);
  }

  endpoint =
  g_memory_output_stream_new_resizable ();

/*
 * Execute code
 *
 */

  success =
  limr_state_parse (state, input_, endpoint, cancellable, &tmp_err);
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
  _g_object_unref0 (state);
  if (G_UNLIKELY (success == FALSE))
    _g_bytes_unref0 (bytes);
return bytes;
}
