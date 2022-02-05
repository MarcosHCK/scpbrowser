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
  [CCode (cheader_filename = "limr_bridge_patch.h")]
  public class BridgePatch
  {
    public static GLib.InputStream get_stdin ();
    public static GLib.OutputStream get_stdout ();
    public static GLib.OutputStream get_stderr ();
  }

  [CCode (cheader_filename = "limr_state_patch.h")]
  public class StatePatch
  {
    public static Lua.LuaVM create_vm () throws GLib.Error;
    public static int add_string (Lua.LuaVM L, string string_, size_t length = -1);
    public static int compile (Lua.LuaVM L, string string_, size_t length = -1) throws GLib.Error;
    public static int execute (Lua.LuaVM L, GLib.OutputStream stream, GLib.Cancellable? cancellable = null) throws GLib.Error;
  }
}
