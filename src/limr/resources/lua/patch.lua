--[[
 * Copyright 2021-2025 MarcosHCK
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
 *]]
local patch = {}

do
  --
  -- Replacements and
  -- additional functions
  --

  local function check (have, want, ...)
    if (not want) then
      return false
    else
      return have == want or check (have, ...)
    end
  end

  local function checkArg (n, have, ...)
    have = type(have)
    if (not check(have, ...)) then
      local msg = string.format ("bad argument #%d (%s expected, got %s)", n, table.concat ({...}, " or "), have)
      error (msg, 3)
    end
  end

  local function getproxy (source)
    return setmetatable ({}, {
      __metatable = "metatable",
      __index = source,
    })
  end

  --
  -- Global environment used inside sketches
  --

  local global;
  global = {
    assert = assert,
    error = error,
    getmetatable = getmetatable,
    setmetatable = setmetatable,
    ipairs = ipairs,
    pairs = pairs,
    next = next,
    load = load,
    pcall = pcall,
    xpcall = xpcall,
    rawequal = rawequal,
    rawget = rawget,
    rawgeti = rawgeti,
    rawlen = rawlen,
    rawseti = rawseti,
    rawset = rawset,
    select = select,
    tonumber = tonumber,
    tostring = tostring,
    type = type,
    require = require,
    print = print,
    printerr = printerr,

    bit32 = getproxy (bit32),
    coroutine = getproxy (coroutine),
    debug = getproxy (debug),
    io = getproxy (io),
    math = getproxy (math),
    os = getproxy (os),
    string = getproxy (string),
    table = getproxy (table),
    checkArg = checkArg,
  }

  global._G = global;

  if (not global.table.unpack) then
    if (_G.unpack) then
      global.table.unpack = _G.pack
    else
      error ("Can't find a suitable 'unpack' function")
    end
  end

  if (not global.table.pack) then
    if (_G.pack) then
      global.table.pack = _G.pack
    else
      if (limr.pack) then
        global.table.pack = limr.pack
      else
        error ("Can't find a suitable 'unpack' function")
      end
    end
  end

  --
  -- Custom 'require' loader
  --

  table.insert (
   package.searchers or package.loaders,
   function (modname)
    local exists = limr.resources.exists
    local load = limr.resources.load
    local msgs = {}

    for item in limr.searchpath (modname, './lua/?.lua') do
      if (exists (item)) then
        local func, reason = load (item, nil, nil, global)
        return (func or reason)
      else
        table.insert (msgs, '\r\n\tno file ' .. item)
      end
    end
  return (#msgs > 0) and table.concat (msgs)
  end)

  --
  -- Sketch executor
  --

  function patch.executor (sketch)
    checkArg (1, sketch, "function");
    local function invoker ()
      sketch ()
    end

    local _g = getproxy (global)
    limr.setfenv (invoker, _g)
    invoker ()
  end
end

-------------------------------------------------------------------------------
return patch;
