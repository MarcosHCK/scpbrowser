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
#ifndef __SCP_EXTENSION__
#define __SCP_EXTENSION__ 1
#include <gio/gio.h>

#define SCP_TYPE_EXTENSION (scp_extension_get_type ())
#define SCP_EXTENSION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SCP_TYPE_EXTENSION, ScpExtension))
#define SCP_EXTENSION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SCP_TYPE_EXTENSION, ScpExtensionClass))
#define SCP_IS_EXTENSION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SCP_TYPE_EXTENSION))
#define SCP_IS_EXTENSION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SCP_TYPE_EXTENSION))
#define SCP_EXTENSION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SCP_TYPE_EXTENSION, ScpExtensionClass))

typedef struct _ScpExtension ScpExtension;
typedef struct _ScpExtensionClass ScpExtensionClass;

#if __cplusplus
extern "C" {
#endif // __cplusplus

GType
scp_extension_get_type (void) G_GNUC_CONST;

#if __cplusplus
}
#endif // __cplusplus

#endif // __SCP_EXTENSION__
