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
#ifndef __SCP_APPLICATION__
#define __SCP_APPLICATION__ 1
#include <gtk/gtk.h>

/**
 * HCL_APPLICATION_ERROR:
 *
 * Error domain for #ScpApplication. Errors in this domain will be from the #ScpApplicationError enumeration.
 * See #GError for more information on error domains.
 */
#define SCP_APPLICATION_ERROR (scp_application_error_quark ())

/**
 * ScpApplicationError:
 * @SCP_APPLICATION_ERROR_FAILED: generic error condition.
 *
 * Error code returned by #ScpApplication API.
 * Note that %SCP_APPLICATION_ERROR_FAILED is here only for compatibility with
 * error domain definition paradigm as defined on GLib documentation.
 */
typedef enum
{
  SCP_APPLICATION_ERROR_FAILED,
} ScpApplicationError;

#define SCP_TYPE_APPLICATION (scp_application_get_type ())
#define SCP_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SCP_TYPE_APPLICATION, ScpApplication))
#define SCP_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SCP_TYPE_APPLICATION, ScpApplicationClass))
#define SCP_IS_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SCP_TYPE_APPLICATION))
#define SCP_IS_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), SCP_TYPE_APPLICATION))
#define SCP_APPLICATION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SCP_TYPE_APPLICATION, ScpApplicationClass))

typedef struct _ScpApplication        ScpApplication;
typedef struct _ScpApplicationPrivate ScpApplicationPrivate;
typedef struct _ScpApplicationClass   ScpApplicationClass;

#if __cplusplus
extern "C" {
#endif // __cplusplus

GQuark
scp_application_error_quark (void) G_GNUC_CONST;
GType
scp_application_get_type (void) G_GNUC_CONST;

struct _ScpApplication
{
  GtkApplication parent_instance;
  ScpApplicationPrivate* priv;
};

struct _ScpApplicationClass
{
  GtkApplicationClass parent_class;
};

GApplication*
scp_application_new (const gchar* application_id, GApplicationFlags flags);

#if __cplusplus
}
#endif // __cplusplus

#endif // __SCP_APPLICATION__
