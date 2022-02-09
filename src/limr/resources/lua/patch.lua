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
local limr = limr
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

  local function proxify (source)
    return
    (source or nil) and
    setmetatable ({}, {
      __metatable = "protected",
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
    print = print,
    printerr = printerr,
    require = require,
    checkArg = checkArg,

    bit32 = proxify (bit32),
    coroutine = proxify (coroutine),
    debug = proxify (debug),
    io = proxify (io),
    math = proxify (math),
    os = proxify (os),
    package = proxify (package),
    string = proxify (string),
    table = proxify (table),
  }

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

  global._G = global;
  patch.environ = global;

  do
    local function searcher (existsfunc, loadfunc, pathfrom, pathidx)
      return
      function (modname)
        local template = pathfrom[pathidx]
        local msgs = {}

        for file in limr.searchpath (modname, template) do
          if (existsfunc (file)) then
            local func, reason = loadfunc (file, nil, 't', global)
            if (not func) then
              error (reason)
            else
              return func
            end
          else
            table.insert (msgs, '\r\n\tno file ')
            table.insert (msgs, file)
          end
        end

      return (#msgs > 0) and (table.concat (msgs))
      end
    end

    table.insert(
     package.searchers
     or package.loaders,
     searcher(
      limr.resources.exists,
      limr.resources.parse,
      limr, 'rpath'))

    table.insert(
     package.searchers
     or package.loaders,
     searcher(
      function (file)
        local handle = io.open (file, 'r')
        if (not handle) then
          return false
        else
          handle:close ()
          return true
        end
      end,
      function (file, _, mode, env)
        local handle = assert (io.open (file, 'rb'))
        local data = handle:read ('*a')
        handle:close ()
        return limr.parse (data, '=' .. file, mode, env)
      end,
      limr, 'rpath'))
  end
end

-------------------------------------------------------------------------------
return patch;
