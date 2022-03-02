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
local callable = require ('html.callable')
local utils = require ('html.utils')
local tag = {}

do
  local nullfunc = function () end
  local tag_mt

  tag_mt =
  {
    __index = function (tag, tagname)
      local opener = '<' .. tagname
      local closer = '</' .. tagname .. '>'

      local func = function (attrs, filler)
        checkArg (1, attrs, 'table')
        filler = callable.create (filler)

        utils.printf (opener)
        for key, value in pairs (attrs) do
          value = callable.create (value)
          utils.printf (' %s="', key)
          value ()
          utils.printf ('"')
        end

        utils.printf (' >')
        do
          filler ()
        end
        utils.printf (closer)
      end

      rawset (tag, tagname, func)
    return func
    end,
  }

  setmetatable (tag, tag_mt)
end
return tag
