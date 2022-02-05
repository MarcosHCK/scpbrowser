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
  public class BridgeClient : Bridge
  {
    private GLib.SubprocessLauncher launcher = null;
    private GLib.DataInputStream stdout = null;
    private GLib.Subprocess server = null;

    /*
     * Disconnect signal
     *
     */

    private void disconnected_inner () throws GLib.Error
    {
      message ("Restarting limr server ...");
#if DEVELOPER == 1
      var path = GLib.Path.build_filename (Config.ABSTOPBUILDDIR, "src/limr", "limr");
#else
      var path = GLib.Path.build_filename (Config.PKGLIBEXECDIR, "limr");
#endif

      var args = new string[1] {path};
      server = launcher.spawnv (args);

      var stdout_ = server.get_stdout_pipe ();
      stdout = new GLib.DataInputStream (stdout_);
    }

    public virtual signal void disconnected ()
    {
      try
      {
        disconnected_inner ();
      } catch (GLib.Error e)
      {
        critical (@"$(e.domain):$(e.code):$(e.message)");
        assert_not_reached ();
      }
    }

    /*
     * Watcher
     *
     */

    protected override void watch_inner (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      do
      {
        try
        {
          server.wait (cancellable);
        } catch (GLib.Error e)
        {
          if (!e.matches (GLib.IOError.quark (), GLib.IOError.CANCELLED))
            throw e;
        }

        if (cancellable.is_cancelled ())
          break;

        lock (server)
        {
          server = null;
          stdout = null;

          disconnected ();
        }
      }
      while (!cancellable.is_cancelled ());
    }

    /*
     * API
     *
     */

    public void release ()
    {
      this.end ();
      server.force_exit ();

      try
      {
        server.wait ();
      } catch (GLib.Error e)
      {
        critical (@"$(e.domain):$(e.code):$(e.message)");
        assert_not_reached ();
      }
    }

    private bool parse_inner (GLib.OutputStream stdout_, GLib.Bytes code, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      var stdin = server.get_stdin_pipe ();
      string captured = null;
      bool finished = false;
      size_t read = 0;

      stdin.write (code.get_data (), cancellable);
      stdin.write (stopchars, cancellable);

      do
      {
        captured =
        stdout.read_upto ((string) stopchars, stopchars.length, out read, cancellable);
        if (captured != null)
        {
          stdout.skip (stopchars.length, cancellable);
          stdout_.write_all (captured.data, null, cancellable);
          finished = true;
        }
      }
      while (!finished);
    return true;
    }

    public bool parse (GLib.OutputStream stdout_, GLib.Bytes code, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      bool result = true;
      lock (server)
      {
        result =
        parse_inner (stdout_, code, cancellable);
      }
    return result;
    }

    /*
     * Constructor
     *
     */

    public BridgeClient ()
    {
      Object ();
      launcher = new GLib.SubprocessLauncher
      (GLib.SubprocessFlags.STDIN_PIPE
       | GLib.SubprocessFlags.STDOUT_PIPE);
      this.disconnected ();
      this.begin ();
    }
  }
}
