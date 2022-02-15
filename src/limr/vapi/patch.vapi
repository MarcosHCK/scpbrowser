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
  [CCode (cheader_filename = "limr_patch.h")]
  public class Patch : GLib.Object
  {
    [CCode (cname = "LLIBRARY")]
    public const string LIBRARY;
    [CCode (cname = "lua_Reader", instance_pos = 1.1, array_length_type = "size_t")]
	  public delegate unowned char[] ReaderFunc (Lua.LuaVM L);
    [CCode (cname = "_limr_preader", array_length_type = "gsize", array_length_pos = 3.1)]
    internal static unowned char[] preader (Lua.LuaVM L, ReaderFunc reader) throws GLib.Error;
    [CCode (cname = "_limr_load")]
    internal static int load (Lua.LuaVM L, ReaderFunc reader, string chunkname);
    [CCode (cname = "_limr_loadx")]
    internal static int loadx (Lua.LuaVM L, ReaderFunc reader, string chunkname, string mode);
    [CCode (cname = "_limr_run_sketch")]
    internal static GLib.Bytes run_sketch (Lua.LuaVM L) throws GLib.Error;
  }

  [CCode (cheader_filename = "private.h")]
  public class LuaPatch : GLib.Object
  {
    [CCode (cname = "luaL_loadbufferx")]
    public static void loadbufferx (Lua.LuaVM L, char[] buffer, string? chunkname = null, string? mode = null);
  }

  [CCode (cheader_filename = "private.h")]
  public class Library : GLib.Object
  {
    [CCode (cname = "MACROS")]
    public const string MACROS;
    [CCode (cname = "MACROS_SEARCHERS")]
    public const string MACROS_SEARCHERS;
    [CCode (cname = "lua_istrue")]
    public bool istrue (Lua.LuaVM L, int idx);
    [CCode (cname = "lua_isfalse")]
    public bool isfalse (Lua.LuaVM L, int idx);
    [CCode (cname = "luaopen_liblimr")]
    public static int open (Lua.LuaVM L);
  }
}
