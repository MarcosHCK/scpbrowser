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
#include <gio/gio.h>
#include <libscpbrowser.h>

int
main(int argc, char* argv[])
{
  GApplication* app = NULL;
  gboolean success = TRUE;
  GError* tmp_err = NULL;
  int status = 0;

  /*
   * Execute
   *
   */

  app =
  scp_application_new (GAPPNAME, G_APPLICATION_HANDLES_OPEN);
  if (G_UNLIKELY (app == NULL))
  {
    g_critical
    ("(%s: %i): null application",
     G_STRFUNC,
     __LINE__);
    g_assert_not_reached ();
  }

  success =
  g_initable_init (G_INITABLE (app), NULL, &tmp_err);
  if (G_UNLIKELY (tmp_err != NULL))
  {
    g_critical
    ("(%s: %i): %s: %i: %s",
     G_STRFUNC,
     __LINE__,
     g_quark_to_string (tmp_err->domain),
     tmp_err->code,
     tmp_err->message);
    g_error_free (tmp_err);
    g_assert_not_reached ();
  }

  status =
  g_application_run (app, argc, argv);

  /*
   * Cleanup
   *
   */

#if DEBUG == 1
  g_assert_finalize_object (app);
#else
  g_object_unref (app);
#endif // DEBUG
return status;
}
