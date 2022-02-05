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
  public class BridgeServer : Bridge
  {
    private GLib.OutputStream stderr = null;
    private GLib.OutputStream stdout = null;
    private GLib.InputStream stdin = null;
    private Limr.State state = null;

    /*
     * Watcher
     *
     */

    protected override void watch_inner (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      var data = new GLib.DataInputStream (stdin);
      GLib.Bytes bytes = null;
      string captured = null;
      size_t read;

      do
      {
        captured =
        data.read_upto ((string) stopchars, stopchars.length, out read, cancellable);
        if (captured != null)
        {
          data.skip (stopchars.length, cancellable);
          bytes = new GLib.Bytes.static (captured.data);
          state.load_bytes (bytes, cancellable);
          state.execute (stdout, cancellable);
        }
      }
      while (!cancellable.is_cancelled ());
    }

    /*
     * API
     *
     */

    public void listen (GLib.Application app)
    {
      app.hold ();
      this.begin ();
    }

    public void unlisten (GLib.Application app)
    {
      app.release ();
      this.end ();
    }

    /*
     * Constructor
     *
     */

    public BridgeServer () throws GLib.Error
    {
      Object ();
      state = new Limr.State ();
      stderr = Limr.BridgePatch.get_stderr ();
      stdout = Limr.BridgePatch.get_stdout ();
      stdin = Limr.BridgePatch.get_stdin ();
    }
  }
}
