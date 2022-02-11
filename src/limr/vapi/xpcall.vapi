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
  [CCode (cheader_filename = "limr_xpcall.h")]
  public class Xpcall
  {
    [NoReturn]
    [CCode (cname = "_limr_throwgerror")]
    public static int throwgerror (Lua.LuaVM L, GLib.Error e);
    [CCode (cname = "_limr_throwrap")]
    public static int throwrap (Lua.LuaVM L);
    [CCode (cname = "_limr_xpcall")]
    public static bool xpcall (Lua.LuaVM L, int narguments, int nreturns) throws Limr.XpcallError;
  }

  [CCode (cheader_filename = "limr_xpcall.h")]
  public errordomain XpcallError
  {
    FAILED,
    SYNTAX,
    RUN,
    RECURSIVE,
    MEMORY;
    public static GLib.Quark quark ();
  }
}
