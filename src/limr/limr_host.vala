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
  public class Host : GLib.Object, GLib.Initable
  {
    private Lua.LuaVM L = null;

    private string _include_path;
    public string include_path
    {
      get
      {
        L.push_literal (Library.MACROS);
        L.get_table (Lua.PseudoIndex.REGISTRY);
        L.push_literal ("path");
        L.get_table (-2);

        _include_path =
        L.to_string (-1);
        L.pop (2);
      return _include_path;
      }
      set
      {
        _include_path = value;
        L.push_literal (Library.MACROS);
        L.get_table (Lua.PseudoIndex.REGISTRY);
        L.push_literal ("path");
        L.push_string (_include_path);
        L.set_table (-3);
        L.pop (1);
      }
    }

  /*
   * Initialization
   *
   */

    private static int bootstrap (Lua.LuaVM L)
    {
      L.open_libs ();
      L.push_literal (Limr.Patch.LIBRARY);
      Limr.Library.open (L);
      L.set_table (Lua.PseudoIndex.REGISTRY);
    return 0;
    }

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      L = new Lua.LuaVM ();
      L.push_cfunction (bootstrap);
      Limr.Xpcall.xpcall (L, 0, 0);
      L.gc (Lua.GarbageCollection.COLLECT, 1);
    return true;
    }
  
  /*
   * API
   *
   */

    public GLib.Bytes parse (GLib.Bytes code, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      var stream = new GLib.MemoryInputStream.from_bytes (code);
#if DEBUG == 1
      int top = L.get_top ();
#endif // DEBUG
      const int bufferSize = 256;
      var buffer = new uint8[bufferSize];
      GLib.Bytes bytes = null;
      size_t read = 0;

      Patch.loadx
      (L, () =>
       {
        try
        {
          read =
          stream.read (buffer, cancellable);
          unowned var result = (char[]) buffer;
                      result.length = (int) read;
          return result;
        } catch (GLib.Error e) {
          Limr.Xpcall.throwgerror (L, e);
        }
       },
       "=sketch",
       "t");

      if (L.is_function (-1))
      {
        try {
          bytes = Limr.Patch.run_sketch (L);
        } catch (GLib.Error e) {
#if DEBUG == 1
          assert (L.get_top () == top);
#endif // DEBUG
          throw e;
        }
      }
      else
      {
        var err = L.to_string (-1);
        var e = new Limr.XpcallError.SYNTAX (err);
        L.pop (1);
        throw e;
      }
#if DEBUG == 1
      assert (L.get_top () == top);
#endif // DEBUG
    return bytes;
    }

    public bool inject (GLib.Bytes code, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
#if DEBUG == 1
      int top = L.get_top ();
#endif // DEBUG
      LuaPatch.loadbufferx (L, (char[]) code.get_data (), "=inject", "bt");
      if (L.is_function (-1))
      {
        try
        {
          L.push_literal (Library.SELF);
          L.get_table (Lua.PseudoIndex.REGISTRY);
          Xpcall.xpcall (L, 1, 0);
        }
        catch (GLib.Error e)
        {
#if DEBUG == 1
          assert (L.get_top () == top);
#endif // DEBUG
          throw e;
        }
      }
      else
      {
        var err = L.to_string (-1);
        var e = new Limr.XpcallError.SYNTAX (err);
        L.pop (1);
        throw e;
      }
#if DEBUG == 1
      assert (L.get_top () == top);
#endif // DEBUG
    return true;
    }

    /*
     * Constructor
     *
     */

    public Host (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object ();
      this.init (cancellable);
    }
  }
}
