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

  /*
   * API
   *
   */

    internal static bool parse (Lua.LuaVM L, Limr.Patch.ReaderFunc reader, GLib.StringBuilder source, GLib.StringChunk table, GenericArray<unowned string> slices)
    {
      var databuf = new GLib.StringBuilder.sized (bufferSize);
      var scriptbuf = new GLib.StringBuilder.sized (bufferSize);
      var state = Parser.State ();
      unowned char[] buffer;
      size_t i;
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
        var idx = slices.data.length;
        slices.add (table.insert_len (databuf.str, databuf.len));
        source.append_printf ("do ref_string (%i); end;\r\n", idx);
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
        try {
          buffer = Limr.Patch.preader (L, reader);
        } catch (GLib.Error e) {
          source.assign (e.message);
          return false;
        }

        if (buffer == null || buffer.length == 0)
          break;

        for (i = 0; i < buffer.length; i++)
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
      while (true);

      flushemit ();
      flushmacro ();
    return true;
    }
  }
}
