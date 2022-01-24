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
  int status = 0;
  pid_t child;

  child = fork ();
  if (G_UNLIKELY (child == -1))
  {
    char* errmsg;
    int err;

    err = errno;
    errmsg = strerror (err);

    g_critical
    ("(%s: %i): %s",
     G_STRFUNC,
     __LINE__,
     errmsg);
    free (errmsg);
    g_assert_not_reached ();
  } else
  if (child == 0)
  {
    app =
    scp_browser_application_new
    (GAPPNAME,
     G_APPLICATION_IS_SERVICE);
  }
  else
  {
    /*
     * I am your father!
     *
     */

    app =
    scp_laucher_application_new
    (GAPPNAME,
     G_APPLICATION_IS_LAUNCHER);
  }

  /*
   * Execution
   *
   */

  if (G_UNLIKELY (app == NULL))
  {
    g_critical
    ("(%s: %i): null application",
     G_STRFUNC,
     __LINE__);
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
