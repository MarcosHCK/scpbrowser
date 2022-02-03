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
  public class State : GLib.Object, GLib.Initable, GLib.AsyncInitable
  {
    private Lua.LuaVM L;
    private const int bufferSize = 1024;

  /*
   * Subtypes
   *
   */

    public struct CompileState
    {
      public bool inmacro;
      public bool pending;
      public bool inlr;
      public uint64 line;
      public uint64 coln;

      public void reset ()
      {
        inmacro = false;
        pending = false;
        inlr = false;
        line = 0;
        coln = 0;
      }

      public CompileState ()
      {
        this.reset ();
      }
    }

    private delegate void Emitter (uint8 c);
    private delegate void Flusher ();

  /*
   * Initialization
   *
   */

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      L = Limr.Patch.create_vm ();
    return true;
    }

  /*
   * API
   *
   */

    public bool load_stream (GLib.InputStream stream, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      var buffer = new uint8[bufferSize];
      var source = new GLib.StringBuilder.sized (bufferSize);
      var databuf = new GLib.StringBuilder.sized (bufferSize);
      var scriptbuf = new GLib.StringBuilder.sized (bufferSize);
      var state = CompileState ();
      string sign = "lua";
      size_t i, read = 0;
      uint8 c;

      /* delegates */
      unowned Flusher emitflush = null;
      unowned Emitter emit = null;
      unowned Flusher flushmacro = null;
      unowned Emitter tomacro = null;
      unowned Emitter tomacro_unsigned = null;

      emitflush = (() =>
      {
        int id =
        Limr.Patch.add_string (L, databuf.str, databuf.len);
        source.append_printf ("do io.stdout:write (ref_string (%i)); end;\r\n", id);
        databuf.truncate (0);
      });

      emit = ((c) =>
      {
        if (databuf.len >= bufferSize)
          emitflush ();
        databuf.append_c ((char) c);
      });

      flushmacro = (() =>
      {
        tomacro = tomacro_unsigned;
        source.append_len (scriptbuf.str, scriptbuf.len);
        scriptbuf.truncate (0);
      });

      tomacro_unsigned = ((c) =>
      {
        scriptbuf.append_c ((char) c);
        if (scriptbuf.len >= sign.length)
        {
          if (scriptbuf.str != sign)
          {
            databuf.append_c ('<');
            databuf.append_c ('?');
            databuf.append_len (scriptbuf.str, scriptbuf.len);
            scriptbuf.truncate (0);
            state.inmacro = false;
            state.pending = false;
          }
          else
          {
            scriptbuf.truncate (0);
            tomacro = (c) =>
            {
              if (databuf.len >= bufferSize)
                flushmacro ();
              scriptbuf.append_c ((char) c);
            };
          }
        }
      });

      tomacro = tomacro_unsigned;

      do
      {
        read =
        stream.read (buffer, cancellable);
        for (i = 0; i < read; i++)
        {
          c = buffer[i];
          if (state.inmacro)
          {
            if (state.pending)
            {
              if (c == '>')
              {
                emitflush ();
                flushmacro ();
                state.pending = false;
                state.inmacro = false;
              }
              else
              {
                state.pending = false;
                tomacro ('?');
                tomacro (c);
              }
            }
            else
            {
              if (c == '?')
              {
                state.pending = true;
              }
              else
              {
                tomacro(c);
              }
            }
          }
          else
          {
            if (state.pending)
            {
              if(c == '?')
              {
                state.pending = false;
                state.inmacro = true;
              }
              else
              {
                state.pending = false;
                emit ('<');
                emit (c);
              }
            }
            else
            {
              if (c == '<')
              {
                state.pending = true;
              }
              else
              {
                emit (c);
              }
            }
          }
        }
      }
      while (read != 0);

      emitflush ();
      flushmacro ();

      Limr.Patch.compile (L, source.str, source.len);
    return true;
    }

    public bool load_bytes (GLib.Bytes bytes, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      var stream = new GLib.MemoryInputStream.from_bytes (bytes);
    return this.load_stream (stream, cancellable);
    }

    public bool execute (GLib.OutputStream stream, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Limr.Patch.execute (L, stream, cancellable);
    return true;
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

    public State.from_stream (GLib.InputStream stream, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object ();
      this.init (cancellable);
      this.load_stream (stream, cancellable);
    }

    public State.from_file (GLib.File file, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object ();
      this.init (cancellable);
      var stream = file.read (cancellable);
      this.load_stream (stream, cancellable);
    }

    public State.from_bytes (GLib.Bytes bytes, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object ();
      this.init (cancellable);
      this.load_bytes (bytes, cancellable);
    }

    public State.from_string (string script, GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object ();
      this.init (cancellable);
      var bytes = new GLib.Bytes.static (script.data);
      this.load_bytes (bytes, cancellable);
    }
  }
}
