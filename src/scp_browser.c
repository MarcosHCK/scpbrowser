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

G_DEFINE_QUARK
(scp-browser-error-quark,
 scp_browser_error);

static void
scp_browser_g_initable_iface_init (GInitableIface* iface);

/*
 * Object definition
 *
 */

G_DEFINE_TYPE_WITH_CODE
(ScpBrowser,
 scp_browser,
 G_TYPE_OBJECT,
 G_IMPLEMENT_INTERFACE
 (G_TYPE_INITABLE,
  scp_browser_g_initable_iface_init));

static void
on_uri_scheme_request_resource (WebKitURISchemeRequest* request, const gchar* path, gpointer pself)
{
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  gchar* fullpath = NULL;
  gchar* mimetype = NULL;
  gchar* data = NULL;

  if (g_str_has_prefix (path, GRESNAME) == FALSE)
    fullpath = g_build_filename (GRESNAME, path, NULL);
  else
    fullpath = g_strdup (path);

  bytes =
  g_resources_lookup_data (fullpath, G_RESOURCE_LOOKUP_FLAGS_NONE, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    webkit_uri_scheme_request_finish_error (request, tmp_err);
    _g_error_free0 (tmp_err);
  }
  else
  {
    if (G_LIKELY (bytes != NULL))
    {
      GInputStream* stream = NULL;
      gsize length = 0;

      stream = g_memory_input_stream_new_from_bytes (bytes);
      data = g_bytes_get_data (bytes, &length);
      mimetype = g_content_type_guess (fullpath, (const guchar*) data, length, NULL);
      webkit_uri_scheme_request_finish (request, stream, length, mimetype);
      _g_object_unref0 (stream);
      _g_free0 (mimetype);
    }
    else
    {
      g_set_error
      (&tmp_err,
       G_RESOURCE_ERROR,
       G_RESOURCE_ERROR_NOT_FOUND,
       "The resource \"%s\" does not exist",
       fullpath);
      webkit_uri_scheme_request_finish_error (request, tmp_err);
      _g_error_free0 (tmp_err);
    }
  }

  _g_free0 (fullpath);
}

static void
on_uri_scheme_request_resources (WebKitURISchemeRequest* request, gpointer pself)
{
  const gchar* path = NULL;

  path =
  webkit_uri_scheme_request_get_path(request);
  on_uri_scheme_request_resource (request, path, pself);
}

static void
on_uri_scheme_request_scss (WebKitURISchemeRequest* request, gpointer pself)
{
  ScpBrowser* self = SCP_BROWSER(pself);
  GInputStream* stream = NULL;
  const gchar* path = NULL;
  const gchar* data = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  GBytes* bytes = NULL;
  gsize length = 0;

  path =
  webkit_uri_scheme_request_get_path(request);

  do
  {
    success =
    g_hash_table_lookup_extended (self->scsses, path, NULL, (gpointer*) &bytes);
    if (G_UNLIKELY (success == FALSE))
    {
      bytes =
      g_resources_lookup_data (path, G_RESOURCE_LOOKUP_FLAGS_NONE, &tmp_err);
      if (G_UNLIKELY (tmp_err != NULL))
      {
        webkit_uri_scheme_request_finish_error (request, tmp_err);
        _g_bytes_unref0 (bytes);
        _g_error_free0 (tmp_err);
        break;
      }
      else
      {
        data = (const gchar*)
        g_bytes_get_data (bytes, &length);

        bytes =
        _scp_browser_compile_scss (self, data, path, &tmp_err);
        if (G_UNLIKELY (tmp_err != NULL))
        {
          g_warning
          ("(%s: %i): %s: %i: %s",
           G_STRFUNC,
           __LINE__,
           g_quark_to_string (tmp_err->domain),
           tmp_err->code,
           tmp_err->message);
          webkit_uri_scheme_request_finish_error (request, tmp_err);
          _g_bytes_unref0 (bytes);
          _g_error_free0 (tmp_err);
          break;
        }
        else
        {
          g_hash_table_insert
          (self->scsses,
           g_strdup (path),
           g_bytes_ref (bytes));
          _g_bytes_unref0 (bytes);
        }
      }
    }
    else
    {
      stream = g_memory_input_stream_new_from_bytes (bytes);
      length = g_bytes_get_size (bytes);

      webkit_uri_scheme_request_finish (request, stream, length, "text/html");
      _g_object_unref0 (stream);
    }
  }
  while (success == FALSE);
}

static void
on_uri_scheme_request_scpbrowser (WebKitURISchemeRequest* request, gpointer pself)
{
  ScpBrowser* self = SCP_BROWSER(pself);
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  gchar* path = NULL;

  path =
  g_path_get_basename (webkit_uri_scheme_request_get_path (request));
  if (g_strcmp0 (path, "home") == 0)
    on_uri_scheme_request_resource (request, "html/home.html", pself);
  else
  if (g_strcmp0 (path, "settings") == 0)
    on_uri_scheme_request_resource (request, "html/settings.html", pself);
  else
  if (g_strcmp0 (path, "about") == 0)
    on_uri_scheme_request_resource (request, "html/about.html", pself);
  else
  {
    g_set_error
    (&tmp_err,
     SCP_BROWSER_ERROR,
     SCP_BROWSER_ERROR_INVALID_PAGE,
     "Invalid page '%s' requested",
     webkit_uri_scheme_request_get_uri
     (request));
    webkit_uri_scheme_request_finish_error (request, tmp_err);
    _g_error_free0 (tmp_err);
  }

  _g_free0 (path);
}

static gboolean
scp_browser_g_initable_iface_init_sync (GInitable* pself, GCancellable* cancellable, GError** error)
{
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  ScpBrowser* self = SCP_BROWSER (pself);

  self->user_content =
  (WebKitUserContentManager*)
  g_object_new
  (WEBKIT_TYPE_USER_CONTENT_MANAGER,
   NULL);

  self->website_data =
  (WebKitWebsiteDataManager*)
  g_object_new
  (WEBKIT_TYPE_WEBSITE_DATA_MANAGER,
   "base-cache-directory", NULL,
   "base-data-directory", NULL,
   "is-ephemeral", FALSE,
   NULL);

  self->settings =
  (WebKitSettings*)
  g_object_new
  (WEBKIT_TYPE_SETTINGS,
   "default-charset", "UTF-8",
   "enable-developer-extras", TRUE,
   "enable-fullscreen", FALSE,
   "enable-java", FALSE,
   NULL);

  self->context =
  (WebKitWebContext*)
  g_object_new
  (WEBKIT_TYPE_WEB_CONTEXT,
   "website-data-manager", self->website_data,
   NULL);

  webkit_web_context_set_cache_model
  (self->context,
   WEBKIT_CACHE_MODEL_DOCUMENT_BROWSER);

  webkit_web_context_register_uri_scheme
  (self->context,
   "resources",
   on_uri_scheme_request_resources,
   self,
   NULL);

  webkit_web_context_register_uri_scheme
  (self->context,
   "scss",
   on_uri_scheme_request_scss,
   self,
   NULL);

  webkit_web_context_register_uri_scheme
  (self->context,
   "scpbrowser",
   on_uri_scheme_request_scpbrowser,
   self,
   NULL);

_error_:
return success;
}

static void
scp_browser_g_initable_iface_init (GInitableIface* iface)
{
  iface->init = scp_browser_g_initable_iface_init_sync;
}

static void
scp_browser_class_finalize (GObject* pself)
{
  ScpBrowser* self = SCP_BROWSER (pself);
  g_hash_table_remove_all (self->scsses);
  g_hash_table_unref (self->scsses);
G_OBJECT_CLASS (scp_browser_parent_class)->finalize (pself);
}

static void
scp_browser_class_dispose (GObject* pself)
{
  ScpBrowser* self = SCP_BROWSER (pself);
  g_hash_table_remove_all (self->scsses);
G_OBJECT_CLASS (scp_browser_parent_class)->dispose (pself);
}

static void
scp_browser_class_init (ScpBrowserClass* klass)
{
  GObjectClass* oclass = G_OBJECT_CLASS (klass);

  oclass->finalize = scp_browser_class_finalize;
  oclass->dispose = scp_browser_class_dispose;
}

static void
scp_browser_init (ScpBrowser* self)
{
  /*
   * Compiled SCSS files cache
   *
   */

  self->scsses =
  g_hash_table_new_full
  (g_str_hash,
   g_str_equal,
   g_free,
   (GDestroyNotify)
   g_bytes_unref);
}

/*
 * Object methods
 *
 */

ScpBrowser*
scp_browser_new (GCancellable* cancellable, GError** error)
{
  return
  (ScpBrowser*)
  g_initable_new
  (SCP_TYPE_BROWSER,
   cancellable,
   error,
   NULL);
}

WebKitWebView*
scp_browser_create_view (ScpBrowser* browser)
{
  g_return_val_if_fail (SCP_IS_BROWSER (browser), NULL);
  gboolean is_ephemeral = FALSE;

  g_object_get
  (browser->website_data,
   "is-ephemeral", &is_ephemeral,
   NULL);

  return
  (WebKitWebView*)
  g_object_ref_sink
  (g_object_new
   (WEBKIT_TYPE_WEB_VIEW,
    "automation-presentation-type", WEBKIT_AUTOMATION_BROWSING_CONTEXT_PRESENTATION_TAB,
    "is-ephemeral", is_ephemeral,
    "settings", browser->settings,
    "user-content-manager", browser->user_content,
    "web-context", browser->context,
    NULL));
}
