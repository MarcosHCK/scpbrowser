/* Copyright 2021-2025 MarcosHCK
 * This file is part of liblimr.
 *
 * liblimr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblimr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liblimr.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

namespace Limr
{
  public class Application : GLib.Application, GLib.Initable
  {
    private Limr.BridgeServer server = null;

    public override void activate ()
    {
      server.listen (this);
    }

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      server = new Limr.BridgeServer ();
    return true;
    }

    public static int main (string[] args)
    {
      GLib.Application app = null;

      try
      {
        app = new Limr.Application (Config.GAPPNAME, GLib.ApplicationFlags.FLAGS_NONE);
      } catch (GLib.Error e)
      {
        critical (@"$(e.domain):$(e.code):$(e.message)");
        assert_not_reached ();
      }

    return app.run (args);
    }

    public Application (string application_id, GLib.ApplicationFlags flags) throws GLib.Error
    {
      Object (application_id : application_id, flags : flags);
      this.init ();
    }
  }
}
