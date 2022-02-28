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
local lgi = require ('lgi')
local utils = {}
local GLib = lgi.GLib

do
  local idsn = 1

  function utils.uniqueid ()
    local nxt = idsn
    idsn = idsn + 1
  return ('scp-uniqueid-%i'):format (nxt)
  end

  function utils.escape (text)
    checkArg (1, text, 'string')
  return GLib.markup_escape_text (text, text:len ())
  end

  function utils.printf (fmt, ...)
    checkArg (1, fmt, 'string')
    local output = fmt:format (...)
    io.stdout:write (output)
  return output
  end
end
return utils
