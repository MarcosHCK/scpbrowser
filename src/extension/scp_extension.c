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
#include <gmodule.h>
#include <scp_extension.h>
#include <webkit2/webkit-web-extension.h>

G_DEFINE_QUARK (scpbrowser, scpbrowser);

/*
 * Object definition
 *
 */

struct _ScpExtension
{
  GObject parent_instance;

  /*<private>*/
};

struct _ScpExtensionClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE_WITH_CODE
(ScpExtension,
 scp_extension,
 G_TYPE_OBJECT,
 );

static void
scp_extension_class_finalize (GObject* pself)
{
  ScpExtension* extension = SCP_EXTENSION (pself);
G_OBJECT_CLASS (scp_extension_parent_class)->finalize (pself);
}

static void
scp_extension_class_dispose (GObject* pself)
{
  ScpExtension* extension = SCP_EXTENSION (pself);
G_OBJECT_CLASS (scp_extension_parent_class)->dispose (pself);
}

static void
scp_extension_class_init (ScpExtensionClass* klass)
{
  GObjectClass* oclass = G_OBJECT_CLASS (klass);

  oclass->finalize = scp_extension_class_finalize;
  oclass->dispose = scp_extension_class_dispose;
}

static void
scp_extension_init (ScpExtension* self)
{
}

/*
 * Object methods
 *
 */

static void
on_page_created (WebKitWebExtension  *extension,
                 WebKitWebPage       *page,
                 ScpExtension        *self)
{
}

static void
on_window_object_cleared (WebKitScriptWorld  *world,
                          WebKitWebPage      *page,
                          WebKitFrame        *frame,
                          ScpExtension       *self)
{
  JSCContext* context = NULL;
  const gchar* uri = NULL;
  gchar* scheme = NULL;

  uri = webkit_web_page_get_uri (page);
  scheme = g_uri_parse_scheme (uri);

  if (G_LIKELY (scheme != NULL))
  {
    if (g_strcmp0 (scheme, "scpbrowser"))
    {
      context =
      webkit_frame_get_js_context_for_script_world (frame, world);
      g_object_unref (context);
    }

    g_free (scheme);
  }
}

G_MODULE_EXPORT
void
webkit_web_extension_initialize_with_user_data (WebKitWebExtension *extension,
                                                const GVariant *user_data)
{
  ScpExtension* ext = NULL;

  /*
   * Prepare extension
   *
   */

  ext =
  (ScpExtension*)
  g_object_new
  (SCP_TYPE_EXTENSION,
   NULL);

  /*
   * Signals
   *
   */

  g_signal_connect_data
  (extension,
   "page-created",
   G_CALLBACK (on_page_created),
   g_object_ref (ext),
   (GClosureNotify)
   g_object_unref,
   0);

  g_signal_connect_data
  (webkit_script_world_get_default (),
   "window-object-cleared",
   G_CALLBACK (on_window_object_cleared),
   g_object_ref (ext),
   (GClosureNotify)
   g_object_unref,
   0);

  /*
   * Finalize
   *
   */

  g_object_unref (ext);
}
