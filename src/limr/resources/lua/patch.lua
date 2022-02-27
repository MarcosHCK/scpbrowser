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
local limr = ...

--- Patch limr
do
  local environ
  local env_mt

  assert (type (limr) == 'table')
  local function deepcopy (table_)
    local result = {}
    local function copy (src, dst)
      for key, value in pairs (src) do
        if (type == 'table') then
          dst[value] = {}
          copy (value, dst[value])
        else
          dst[key] = value
        end
      end
    end
    copy (table_, result)
  return result
  end

  environ =
  {
    assert = assert,
    bit = bit,
    bits = bits,
    bit32 = bit32,
    bit64 = bit64,
    checkArg = checkArg,
    collectgarbage = collectgarbage,
    coroutine = coroutine,
    error = error,
    getfenv = getfenv,
    getmetatable = getmetatable,
    include = limr.include,
    io = io,
    ipairs = ipairs,
    math = math,
    macros = limr.macros,
    next = next,
    package = package,
    pairs = pairs,
    pcall = pcall,
    print = print,
    rawequal = rawequal,
    rawget = rawget,
    rawlen = rawlen,
    rawset = rawset,
    require = require,
    select = select,
    setfenv = setfenv,
    setmetatable = setmetatable,
    string = string,
    table = table,
    tostring = tostring,
    tonumber = tonumber,
    type = type,
    xpcall = xpcall,
  }

  package.loaded.limr = limr

  setmetatable (limr,
  {
    __index = function (_, key)
      if (key == 'environ') then
        local env = deepcopy (environ)
        env._G = env
        return env
      end
    end,
  })
end

-- Detect lgi installation
do
  local success, lgi = pcall (require, 'lgi')
  if (not success) then
    print ('Missing lgi installation, you may expect some errors')
  else
    package.loaded.lgi = lgi
  end
end
