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
  errordomain StateError
  {
    FAILED,
    SYNTAX,
    RUN,
    MEMORY,
  }

  public class State : GLib.Object, GLib.Initable
  {
    private Lua.LuaVM L;

  /*
   * Initialization
   *
   */

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      L = new Lua.LuaVM ();
      L.push_cfunction (Limr.StatePatch.init);
      L.push_lightuserdata (this);
      Limr.Xpcall.xpcall (L, 1, 0);
      L.gc (Lua.GarbageCollection.COLLECT, 1);
    return true;
    }

  /*
   * API
   *
   */

    public bool execute_stream (GLib.InputStream input, GLib.OutputStream stdout_, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      int result =
      Limr.Parser.parse_stream (L, input, "=sketch", "t");
      switch (result)
      {
      case Lua.ThreadStatus.ERRSYNTAX:
        throw new StateError.SYNTAX (L.to_string (-1));
      case Lua.ThreadStatus.ERRMEM:
        throw new StateError.SYNTAX ("Lua thread out of memory");
      default:
        assert (result == 0);
        assert (L.is_function (-1));
        L.get_field (Lua.PseudoIndex.REGISTRY, StatePatch.LENVIRON);
        Limr.StatePatch.setfenv (L, -2);
        Limr.Xpcall.xpcall (L, 0, 0);
        break;
      }
    return true;
    }

    public bool execute_bytes (GLib.Bytes input, GLib.OutputStream stdout_, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      GLib.InputStream input_ = new GLib.MemoryInputStream.from_bytes (input);
    return execute_stream (input_, stdout_, cancellable);
    }

  /*
   * Constructors
   *
   */

    public State (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object ();
      this.init (cancellable);
    }
  }
}
