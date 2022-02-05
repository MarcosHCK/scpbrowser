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
#include <limr/liblimr.h>

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

#if __cplusplus
extern "C" {
#endif // __cplusplus

struct _ScpBrowser
{
  GObject parent_instance;

  /*<private>*/
  GHashTable* jhtmls;
  GHashTable* scsses;

  /*<private>*/
  LimrBridgeClient* limr_client;

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
_scp_browser_compile_scss (ScpBrowser* self, GBytes* input, const gchar* path, GError** error);
G_GNUC_INTERNAL
GBytes*
_scp_browser_compile_jhtml (ScpBrowser* self, GBytes* input, const gchar* path, GError** error);

#if __cplusplus
}
#endif // __cplusplus

#endif // __SCP_BROWSER_INTERNAL__
