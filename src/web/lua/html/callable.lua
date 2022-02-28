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
local utils = require ('html.utils')
local callable = {}

do
  local nullfunc
  nullfunc = function ()
  end

  function callable.is (argument, nullable)
    checkArg (2, nullable, 'boolean', 'nil')
    if (type (argument) == 'nil') then
      return nullable
    elseif (type (argument) == 'thread') then
      return coroutine.status (argument) ~= 'dead'
    elseif (type (argument) == 'table') then
      local mt = getmetatable (argument)
      if (type (mt) == 'table') then
        local method = mt.__call
        return callable.is (argument, nullable)
      end
    else
      return type (argument) == 'function'
    end
  return false
  end

  function callable.check (argn, argument, nullable)
    checkArg (1, argn, 'number')
    checkArg (3, nullable, 'boolean', 'nil')
    local is = callable.is (argument, nullable)
    if (not is) then
      local type_ = type (argument)
      local msg = ('bad argument #%i (expected a callable object, got %s)'):format (argn, type_)
      error (msg, 2)
    end
  end

  function callable.create (argument)
    if (type (argument) == 'nil') then
      argument = nullfunc
    elseif (type (argument) == 'thread') then
      if (coroutine.status (argument) ~= 'dead') then
        argument = function ()
          coroutine.resume (argument)
        end
      else
        error ('can\'t resume a dead coroutine', 2)
      end
    elseif (type (argument) == 'table') then
      local mt = getmetatable (argument)
      if (type (mt) == 'table') then
        local method = mt.__call
        if (not method) then
          error ('non callable source', 2)
        end
      else
        error ('non callable source', 2)
      end
    elseif (type (argument) ~= 'function') then
      local value = argument
      argument = function ()
        local value = utils.escape (value)
        utils.printf ('%s', value)
      end
    end
  return argument
  end
end
return callable
