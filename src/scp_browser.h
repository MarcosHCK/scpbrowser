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
#ifndef __SCP_BROWSER__
#define __SCP_BROWSER__ 1
#include <gio/gio.h>
#include <webkit2/webkit2.h>

#define SCP_BROWSER_ERROR (scp_browser_error_quark ())

typedef enum
{
  SCP_BROWSER_ERROR_FAILED,
  SCP_BROWSER_ERROR_INVALID_PAGE,
  SCP_BROWSER_ERROR_SASS_COMPILING,
} ScpBrowserError;

#define SCP_TYPE_BROWSER (scp_browser_get_type ())
#define SCP_BROWSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SCP_TYPE_BROWSER, ScpBrowser))
#define SCP_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SCP_TYPE_BROWSER, ScpBrowserClass))
#define SCP_IS_BROWSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SCP_TYPE_BROWSER))
#define SCP_IS_BROWSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SCP_TYPE_BROWSER))
#define SCP_BROWSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SCP_TYPE_BROWSER, ScpBrowserClass))

typedef struct _ScpBrowser ScpBrowser;
typedef struct _ScpBrowserClass ScpBrowserClass;

#if __cplusplus
extern "C" {
#endif // __cplusplus

GQuark
scp_browser_error_quark (void) G_GNUC_CONST;
GType
scp_browser_get_type (void) G_GNUC_CONST;

ScpBrowser*
scp_browser_new (GCancellable* cancellable, GError** error);
void
scp_browser_set_sanboxed (ScpBrowser* browser, gboolean sandboxed);
gboolean
scp_browser_get_sanboxed (ScpBrowser* browser);
WebKitWebView*
scp_browser_create_view (ScpBrowser* browser);

#if __cplusplus
}
#endif // __cplusplus

#endif // __SCP_BROWSER__
