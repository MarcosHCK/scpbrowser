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
#ifndef __SCP_BROWSER_INTERNAL__
#define __SCP_BROWSER_INTERNAL__ 1
#include <scp_browser.h>

#define goto_error() \
  G_STMT_START { \
    success = FALSE; \
    goto _error_; \
  } G_STMT_END

#define _webkit_user_style_sheet_unref0(var) ((var == NULL) ? NULL : (var = (webkit_user_style_sheet_unref (var), NULL)))
#define _webkit_user_script_unref0(var) ((var == NULL) ? NULL : (var = (webkit_user_script_unref (var), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_bytes_unref0(var) ((var == NULL) ? NULL : (var = (g_bytes_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) ((var == NULL) ? NULL : (var = (g_free (var), NULL)))

#define IMPLEMENT_CACHED_ON_REQUEST(prefix,cache,generator) \
  static void \
  on_uri_scheme_request_##prefix##_with_fullpath (WebKitURISchemeRequest* request, const gchar* fullpath, gpointer pself) \
  { \
    ScpBrowser* self = SCP_BROWSER(pself); \
    GHashTable* cache = self-> cache; \
    GInputStream* stream = NULL; \
    const gchar* data = NULL; \
    gboolean success = TRUE; \
    GError* tmp_err = NULL; \
    GBytes* bytes = NULL; \
    gsize length = 0; \
;\
    do \
    { \
      success = \
      g_hash_table_lookup_extended (cache, fullpath, NULL, (gpointer*) &bytes); \
      if (G_UNLIKELY (success == FALSE)) \
      { \
        bytes = \
        g_resources_lookup_data (fullpath, G_RESOURCE_LOOKUP_FLAGS_NONE, &tmp_err); \
        if (G_UNLIKELY (tmp_err != NULL)) \
        { \
          webkit_uri_scheme_request_finish_error (request, tmp_err); \
          _g_bytes_unref0 (bytes); \
          _g_error_free0 (tmp_err); \
          break; \
        } \
        else \
        { \
          bytes = \
          (generator) (self, bytes, fullpath, &tmp_err); \
          if (G_UNLIKELY (tmp_err != NULL)) \
          { \
            g_warning \
            ("(%s: %i): %s: %i: %s", \
            G_STRFUNC, \
            __LINE__, \
            g_quark_to_string (tmp_err->domain), \
            tmp_err->code, \
            tmp_err->message); \
            webkit_uri_scheme_request_finish_error (request, tmp_err); \
            _g_bytes_unref0 (bytes); \
            _g_error_free0 (tmp_err); \
            break; \
          } \
          else \
          { \
            g_hash_table_insert \
            (cache, \
            g_strdup (fullpath), \
            g_bytes_ref (bytes)); \
            _g_bytes_unref0 (bytes); \
          } \
        } \
      } \
      else \
      { \
        stream = g_memory_input_stream_new_from_bytes (bytes); \
        length = g_bytes_get_size (bytes); \
; \
        webkit_uri_scheme_request_finish (request, stream, length, "text/html"); \
        _g_object_unref0 (stream); \
      } \
    } \
    while (success == FALSE); \
  } \
  static void \
  on_uri_scheme_request_##prefix (WebKitURISchemeRequest* request, gpointer pself) \
  { \
    const gchar* path = NULL; \
    gchar* fullpath = NULL; \
;\
    path = \
    webkit_uri_scheme_request_get_path (request); \
;\
    if (g_str_has_prefix (path, GRESNAME) == FALSE) \
      fullpath = g_build_filename (GRESNAME, path, NULL); \
    else \
      fullpath = g_strdup (path); \
; \
    on_uri_scheme_request_##prefix##_with_fullpath (request, fullpath, pself); \
    _g_free0 (fullpath); \
  }
#define IMPLEMENT_ON_REQUEST(prefix,generator) \
  static void \
  on_uri_scheme_request_##prefix##_with_fullpath (WebKitURISchemeRequest* request, const gchar* fullpath, gpointer pself) \
  { \
    ScpBrowser* self = SCP_BROWSER(pself); \
    GInputStream* stream = NULL; \
    gboolean success = TRUE; \
    GError* tmp_err = NULL; \
    GBytes* bytes = NULL; \
    gsize length = 0; \
;\
    bytes = \
    g_resources_lookup_data (fullpath, G_RESOURCE_LOOKUP_FLAGS_NONE, &tmp_err); \
    if (G_UNLIKELY (tmp_err != NULL)) \
    { \
      webkit_uri_scheme_request_finish_error (request, tmp_err); \
      _g_bytes_unref0 (bytes); \
      _g_error_free0 (tmp_err); \
    } \
    else \
    { \
      bytes = \
      (generator) (self, bytes, fullpath, &tmp_err); \
      if (G_UNLIKELY (tmp_err != NULL)) \
        { \
          g_warning \
          ("(%s: %i): %s: %i: %s", \
          G_STRFUNC, \
          __LINE__, \
          g_quark_to_string (tmp_err->domain), \
          tmp_err->code, \
          tmp_err->message); \
          webkit_uri_scheme_request_finish_error (request, tmp_err); \
          _g_bytes_unref0 (bytes); \
          _g_error_free0 (tmp_err); \
        } \
      else \
        { \
          stream = g_memory_input_stream_new_from_bytes (bytes); \
          length = g_bytes_get_size (bytes); \
; \
          webkit_uri_scheme_request_finish (request, stream, length, "text/html"); \
          _g_object_unref0 (stream); \
        } \
    } \
  } \
  static void \
  on_uri_scheme_request_##prefix (WebKitURISchemeRequest* request, gpointer pself) \
  { \
    const gchar* path = NULL; \
    gchar* fullpath = NULL; \
;\
    path = \
    webkit_uri_scheme_request_get_path (request); \
;\
    if (g_str_has_prefix (path, GRESNAME) == FALSE) \
      fullpath = g_build_filename (GRESNAME, path, NULL); \
    else \
      fullpath = g_strdup (path); \
; \
    on_uri_scheme_request_##prefix##_with_fullpath (request, fullpath, pself); \
    _g_free0 (fullpath); \
  }

#if __cplusplus
extern "C" {
#endif // __cplusplus

struct _ScpBrowser
{
  GObject parent_instance;

  /*<private>*/
  WebKitWebContext* context;
  WebKitSettings* settings;
  WebKitUserContentManager* user_content;
  WebKitWebsiteDataManager* website_data;
};

struct _ScpBrowserClass
{
  GObjectClass parent_class;
};

G_GNUC_INTERNAL
GBytes*
_scp_browser_compile_jhtml (ScpBrowser* self, GBytes* input, const gchar* path, GError** error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __SCP_BROWSER_INTERNAL__
