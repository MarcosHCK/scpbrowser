/* Copyright 2021-2025 MarcosHCK
 * This file is part of libLimr.
 *
 * libLimr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libLimr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libLimr.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

namespace Limr
{
  [CCode (cheader_filename = "limr_state_patch.h")]
  public class StatePatch
  {
    [CCode (cname = "LENVIRON")]
    public const string LENVIRON;
    [CCode (cname = "_limr_vm_init")]
    public static int init (Lua.LuaVM L);
    [CCode (cname = "_limr_vm_setfenv")]
    public static int setfenv (Lua.LuaVM L, int idx);
    [NoReturn]
    [CCode (cname = "_limr_vm_throwgerror")]
    public static void throwgerror (Lua.LuaVM L, GLib.Error e);
  }

  [CCode (cheader_filename = "limr_parser_patch.h")]
  public class ParserPatch
  {
    [CCode (cname = "_limr_parser_loadbufferx")]
    public static int loadbufferx (Lua.LuaVM L, [CCode (array_length_type = "gsize", array_length_pos = 2.1)] uint8[] source, string? chunkname = null, string mode = "t");
  }

  [CCode (cheader_filename = "limr_xpcall.h")]
  public class Xpcall
  {
    [CCode (cname = "_limr_throwrap")]
    public static int throwrap (Lua.LuaVM L);
    [CCode (cname = "_limr_xpcall")]
    public static int xpcall (Lua.LuaVM L, int nargs, int nresults) throws GLib.Error;
  }
}
