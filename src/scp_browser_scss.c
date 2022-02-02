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
#include <sass/context.h>
#include <scp_browser_internl.h>

static Sass_Import_List
resources_importer (const char* url, Sass_Importer_Entry cb, struct Sass_Compiler* compiler)
{
  Sass_Import_List list = NULL;
  Sass_Import_Entry entry = NULL;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  gchar* path = NULL;
  gchar* data = NULL;
  gsize length = 0;

  path =
  g_build_filename (GRESNAME "/css", url, NULL);

  bytes =
  g_resources_lookup_data (path, G_RESOURCE_LOOKUP_FLAGS_NONE, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    list = sass_make_import_list (1);
    data = sass_copy_c_string (tmp_err->message);
    sass_import_set_error (list[0], data, 0, 0);
    _g_bytes_unref0 (bytes);
  }
  else
  {
    list = sass_make_import_list (1);
    data = (gchar*) g_bytes_get_data (bytes, &length);
    data = sass_copy_c_string (data);
    list[0] = sass_make_import_entry (path, data, 0);
    _g_bytes_unref0 (bytes);
  }
return list;
}

static
Sass_Importer_Fn importers[] =
{
  resources_importer,
};

G_GNUC_INTERNAL
GBytes*
_scp_browser_compile_scss (ScpBrowser* self, const gchar* input, const gchar* path, GError** error)
{
  struct Sass_Data_Context* dctx = NULL;
  struct Sass_Context* ctx = NULL;
  struct Sass_Options* opt = NULL;
  Sass_Importer_List list = NULL;
  Sass_Importer_Entry entry = NULL;
  gsize importern = G_N_ELEMENTS (importers);
  const gchar* output = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  gchar* data = NULL;
  gsize length = 0;
  int status = 0;
  gsize i;

  /*
   * Prepare context
   *
   */

  data = sass_copy_c_string (input);
  dctx = sass_make_data_context (data);
  ctx = sass_data_context_get_context (dctx);
  opt = sass_context_get_options (ctx);

  /*
   * Create importers
   *
   */

  list =
  sass_make_importer_list (importern);

  for (i = 0; i < importern; i++)
  {
    entry = sass_make_importer (importers[i], 0, self);
    sass_importer_set_list_entry (list, i, entry);
  }

  /*
   * Set context options
   *
   */

  sass_option_set_output_style (opt, SASS_STYLE_EXPANDED);
  sass_option_set_c_importers (opt, list);

  /*
   * Parse input
   *
   */

  status =
  sass_compile_data_context (dctx);
  if (G_UNLIKELY (status != 0))
  {
    g_set_error
    (error,
     SCP_BROWSER_ERROR,
     SCP_BROWSER_ERROR_SASS_COMPILING,
     "Error compiling sass '%s': %s",
     path,
     sass_context_get_error_message (ctx));
    goto_error ();
  }
  else
  {
    output = sass_context_get_output_string (ctx);
    length = strlen (output);
    bytes = g_bytes_new (output, length);
  }

_error_:
  sass_delete_data_context (dctx);
  if (G_UNLIKELY (success == FALSE))
    _g_bytes_unref0 (bytes);
return bytes;
}
