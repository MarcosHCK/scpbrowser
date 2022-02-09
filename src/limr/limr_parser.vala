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
  public class Parser : GLib.Object
  {
    private const string sign = "lua";
    private const int bufferSize = 1024;
    private const int chunkSize = 256;

    public struct State
    {
      public bool inmacro;
      public bool pending;
      public bool inlr;
      public uint64 line;
      public uint64 coln;
      public char queue;
      public size_t read;
      public size_t used;
    }

    private delegate void Emitter (uint8 c);
    private delegate void Flusher ();

    internal static int parse_stream_internal (Lua.LuaVM L, GLib.InputStream stream, string chunkname, string mode) throws GLib.Error
    {
      var buffer = new uint8[bufferSize];
      var source = new GLib.StringBuilder.sized (bufferSize);
      var databuf = new GLib.StringBuilder.sized (bufferSize);
      var scriptbuf = new GLib.StringBuilder.sized (bufferSize);
      var table = new GLib.StringChunk (chunkSize);
      var slices = new GenericArray<unowned string> ();
      var state = Parser.State ();
      size_t i, read = 0;
      uint8 c;

      /*
       * Delegates
       *
       */

      unowned Flusher flushemit = null;
      unowned Emitter emitchar = null;
      unowned Flusher flushmacro = null;
      unowned Emitter emitmacro = null;
      unowned Emitter emitmacro_unsigned = null;

      flushemit = (() =>
      {
        var idx = slices.data.length + 1;
        slices.add (table.insert_len (databuf.str, databuf.len));
        source.append_printf ("do io.stdout:write (ref_string (%i)); end;\r\n", idx);
        databuf.truncate (0);
      });

      emitchar = ((c) =>
      {
        if (databuf.len >= bufferSize)
          flushemit ();
        databuf.append_c ((char) c);
      });

      flushmacro = (() =>
      {
        emitmacro = emitmacro_unsigned;
        source.append_len (scriptbuf.str, scriptbuf.len);
        scriptbuf.truncate (0);
      });

      emitmacro_unsigned = ((c) =>
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
            emitmacro = (c) =>
            {
              if (databuf.len >= bufferSize)
                flushmacro ();
              scriptbuf.append_c ((char) c);
            };
          }
        }
      });

      emitmacro = emitmacro_unsigned;

      /*
       * Load loop
       *
       */

      do
      {
        read =
        stream.read (buffer, null);
        for (i = 0; i < read; i++)
        {
          c = buffer[i];
          if (state.inmacro)
          {
            if (state.pending)
            {
              if (c == '>')
              {
                flushemit ();
                flushmacro ();
                state.pending = false;
                state.inmacro = false;
              }
              else
              {
                emitmacro ('?');
                emitmacro (c);
                state.pending = false;
              }
            }
            else
            {
              if (c == '?')
                state.pending = true;
              else
                emitmacro(c);
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
                emitchar ('<');
                emitchar (c);
                state.pending = false;
              }
            }
            else
            {
              if (c == '<')
                state.pending = true;
              else
                emitchar (c);
            }
          }
        }
      }
      while (read != 0);

      flushemit ();
      flushmacro ();

      /*
       * Load
       *
       */

      int result =
      Limr.ParserPatch.loadbufferx (L, source.data, chunkname, mode);
    return result;
    }

    internal static int parse_bytes_internal (Lua.LuaVM L, GLib.Bytes bytes, string chunkname, string mode) throws GLib.Error
    {
      GLib.InputStream stream = new GLib.MemoryInputStream.from_bytes (bytes);
    return parse_stream (L, stream, chunkname, mode);
    }

    [CCode (cname = "_limr_parser_parse_stream")]
    internal static int parse_stream (Lua.LuaVM L, GLib.InputStream stream, string chunkname, string mode) throws GLib.Error
    {
      try {
        int result =
        parse_stream_internal (L, stream, chunkname, mode);
        return result;
      } catch (GLib.Error e)
      {
        Limr.StatePatch.throwgerror (L, e);
      }
    }

    [CCode (cname = "_limr_parser_parse_bytes")]
    internal static int parse_bytes (Lua.LuaVM L, GLib.Bytes bytes, string chunkname, string mode) throws GLib.Error
    {
      try {
        int result =
        parse_bytes_internal (L, bytes, chunkname, mode);
        return result;
      } catch (GLib.Error e)
      {
        Limr.StatePatch.throwgerror (L, e);
      }
    }
  }
}
