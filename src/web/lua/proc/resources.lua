--[[
 * Copyright 2021-2025 MarcosHCK
 * This file is part of scpbrowser.
 *
 * scpbrowser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scpbrowser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with scpbrowser.  If not, see <http://www.gnu.org/licenses/>.
 *
 *]]
local config = require ('config')
local lgi = require ('lgi')
local utils = require ('html.utils')
local resources = {}
local GLib = lgi.GLib
local Gio = lgi.Gio

do
  local folder = config.resources.root

  function resources.lookup_as_bytes (name)
    checkArg (1, name, 'string')
    local file = folder:get_child (name)
    local bytes = file:load_bytes ()
  return bytes
  end

  function resources.lookup_as_stream (name)
    checkArg (1, name, 'string')
    local file = folder:get_child (name)
    local stream = file:read ()
  return stream
  end

  function resources.lookup (name)
    checkArg (1, name, 'string')
    local file = folder:get_child (name)
    local bytes = file:load_contents ()
  return bytes
  end

  function resources.dump (name, open_closure, line_closure, close_closure)
    checkArg (1, name, 'string')
    checkArg (2, open_closure, 'function', 'thread', 'table', 'nil')
    checkArg (3, line_closure, 'function', 'thread', 'table', 'nil')
    checkArg (4, close_closure, 'function', 'thread', 'table', 'nil')

    -- Check closures are callable
    if (open_closure == nil) then
      open_closure = function ()
        utils.printf ('<p>')
      end
    elseif (not utils.iscallable (open_closure)) then
      checkArg (2, nil, 'function', 'thread', 'table')
    end
    if (line_closure == nil) then
      line_closure = function (line)
        local line = utils.escape (line)
        utils.printf ('<span>%s</span>', line)
      end
    elseif (not utils.iscallable (line_closure)) then
      checkArg (3, nil, 'function', 'thread', 'table')
    end
    if (close_closure == nil) then
      close_closure = function ()
        utils.printf ('</p>')
      end
    elseif (not utils.iscallable (close_closure)) then
      checkArg (4, nil, 'function', 'thread', 'table')
    end

    -- Open stream
    local fstream = resources.lookup_as_stream (name)
    local stream = Gio.DataInputStream.new (fstream)

    -- Begin dump
    open_closure ()
    repeat
      local line = stream:read_line_utf8 ()
      if (line) then
        if (line == '') then
          close_closure ()
          open_closure ()
        else
          line_closure (line)
        end
      end
    until (line == nil)
    close_closure ()
  end
end
return resources
