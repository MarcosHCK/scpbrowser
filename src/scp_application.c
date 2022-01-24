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
#include <scp_application.h>

G_DEFINE_QUARK
(scp-application-error-quark,
 scp_application_error);

/*
 * Object definition
 *
 */

struct _ScpApplicationPrivate
{
  int dummy;
};

G_DEFINE_TYPE_WITH_CODE
(ScpApplication,
 scp_application,
 GTK_TYPE_APPLICATION,
 G_ADD_PRIVATE(ScpApplication)
 );

static void
scp_application_class_init (ScpApplicationClass* klass)
{
}

static void
scp_application_init (ScpApplication* self)
{
}

/*
 * Object methods
 *
 */

GApplication*
scp_application_new (const gchar* application_id, GApplicationFlags flags)
{
  return
  (GApplication*)
  g_object_new
  (SCP_TYPE_APPLICATION,
  "application-id", application_id,
  "flags", flags,
  NULL);
}
