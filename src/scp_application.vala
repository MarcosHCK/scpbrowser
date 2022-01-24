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

namespace Scp
{
  public class Application : Gtk.Application, GLib.Initable
  {
    private Scp.Browser? browser = null;
    private Scp.Window? window = null;

    /*
     * Overrides
     *
     */

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      browser = new Scp.Browser ();
    return false;
    }

    public override void activate ()
    {
      GLib.File home = GLib.File.new_for_uri ("scpbrowser://home");
      GLib.File[] vector = new GLib.File[1] {home};
      this.open (vector, "url");
    }

    public override void open (GLib.File[] files, string hint)
    {
      if (unlikely (window == null))
      {
        window = new Scp.Window ();
        window.set_application (this);
        window.present ();
      }

      foreach (var file in files)
      {
        try
        {
          window.open (file, hint);
        }
        catch (GLib.Error e)
        {
          critical (@"$(e.domain): $(e.code): $(e.message)\r\n");
          assert_not_reached ();
        }
      }
    }

    /*
     * Constructors
     *
     */

    construct
    {
    }

    public Application (string application_id, GLib.ApplicationFlags flags)
    {
      Object (application_id : application_id, flags : flags);
    }
  }
}
