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
#include <scp_browser.h>
#include <webkit2/webkit2.h>

G_DEFINE_QUARK
(scp-browser-error-quark,
 scp_browser_error);

static void
scp_browser_g_initable_iface_init (GInitableIface* iface);

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

/*
 * Object definition
 *
 */

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

G_DEFINE_TYPE_WITH_CODE
(ScpBrowser,
 scp_browser,
 G_TYPE_OBJECT,
 G_IMPLEMENT_INTERFACE
 (G_TYPE_INITABLE,
  scp_browser_g_initable_iface_init));

static GInputStream*
on_uri_scheme_request_scpbrowser_ (WebKitURISchemeRequest* request, ScpBrowser* self, gsize* plength, GError** error)
{
  GInputStream* stream = NULL;
  GError* tmp_err = NULL;
  gboolean success = FALSE;
  gchar* path = NULL;
  gsize length = 0;

  path =
  g_path_get_basename (webkit_uri_scheme_request_get_path (request));
  if (!strcmp (path, "home"))
  {
    GBytes* bytes =
    g_resources_lookup_data (GRESNAME "/html/home.html", 0, &tmp_err);
    if G_LIKELY (tmp_err == NULL)
    {
      stream = g_memory_input_stream_new_from_bytes (bytes);
      length = g_bytes_get_size (bytes);
      _g_bytes_unref0 (bytes);
    }
    else
    {
      g_propagate_error (error, tmp_err);
      goto_error ();
    }
  } else
  if (!strcmp (path, "settings"))
  {
    GBytes* bytes =
    g_resources_lookup_data(GRESNAME "/html/settings.html", 0, &tmp_err);
    if G_LIKELY (tmp_err == NULL)
    {
      stream = g_memory_input_stream_new_from_bytes (bytes);
      length = g_bytes_get_size (bytes);
      _g_bytes_unref0 (bytes);
    }
    else
    {
      g_propagate_error (error, tmp_err);
      goto_error ();
    }
  } else
  if (!strcmp (path, "about"))
  {
    GBytes* bytes =
    g_resources_lookup_data (GRESNAME "/html/about.html", 0, &tmp_err);
    if G_LIKELY (tmp_err == NULL)
    {
      stream = g_memory_input_stream_new_from_bytes (bytes);
      length = g_bytes_get_size (bytes);
      _g_bytes_unref0 (bytes);
    }
    else
    {
      g_propagate_error (error, tmp_err);
      goto_error ();
    }
  }
  else
  {
    g_set_error
    (error,
     SCP_BROWSER_ERROR,
     SCP_BROWSER_ERROR_INVALID_PAGE,
     "Invalid page '%s' requested",
     webkit_uri_scheme_request_get_uri
     (request));
    goto_error ();
  }

_error_:
  *plength = length;
  if (G_UNLIKELY (success == FALSE))
    _g_object_unref0(stream);
  _g_free0 (path);
return stream;
}

static void
on_uri_scheme_request_scpbrowser (WebKitURISchemeRequest* request, gpointer pself)
{
  ScpBrowser* self = SCP_BROWSER(pself);
  GInputStream* stream = NULL;
  GError* tmp_err = NULL;
  gsize length = 0;

  stream =
  on_uri_scheme_request_scpbrowser_ (request, self, &length, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    webkit_uri_scheme_request_finish_error (request, tmp_err);
    _g_error_free0 (tmp_err);
  }
  else
  {
    webkit_uri_scheme_request_finish (request, stream, length, "text/html");
    _g_object_unref0 (stream);
  }
}

static void
on_uri_scheme_request_resources (WebKitURISchemeRequest* request, gpointer pobj)
{
  GInputStream* stream = NULL;
  const gchar* path = NULL;
  GBytes* bytes = NULL;
  GError* tmp_err = NULL;
  gsize length = 0;

  path =
  webkit_uri_scheme_request_get_path(request);

  bytes =
  g_resources_lookup_data(path, 0, &tmp_err);
  if G_LIKELY(tmp_err == NULL)
  {
    stream = g_memory_input_stream_new_from_bytes(bytes);
    length = g_bytes_get_size(bytes);
    _g_bytes_unref0(bytes);

    webkit_uri_scheme_request_finish(request, stream, length, NULL);
    _g_object_unref0 (stream);
  }
  else
  {
    webkit_uri_scheme_request_finish_error(request, tmp_err);
    _g_error_free0 (tmp_err);
  }
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
   "scpbrowser",
   on_uri_scheme_request_scpbrowser,
   self,
   NULL);

  webkit_web_context_register_uri_scheme
  (self->context,
   "resources",
   on_uri_scheme_request_resources,
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
G_OBJECT_CLASS (scp_browser_parent_class)->finalize (pself);
}

static void
scp_browser_class_dispose (GObject* pself)
{
  ScpBrowser* self = SCP_BROWSER (pself);
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
