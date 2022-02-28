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
local ffi = require ('ffi')
local lgi = require ('lgi')
local version = {}

do
  local namespaces
  local suffixes

  namespaces =
  {
    { id = 'gio', name = 'Gio', preffix = 'glib', variable = true, },
    { id = 'glib', name = 'GLib', preffix = 'glib', variable = true, },
    { id = 'gmodule', name = 'GModule', preffix = 'glib', variable = true, },
    { id = 'gobject', name = 'GObject', preffix = 'glib', variable = true, },
    { id = 'gtk', name = 'Gtk', preffix = 'gtk', variable = false, },
    { id = 'limr', name = 'Limr', preffix = 'limr', variable = false, },
    { id = 'suop', name = 'Soup', preffix = 'soup', variable = false, },
    { id = 'webkit', name = 'WebKit2', preffix = 'webkit', variable = false, },
  }

  suffixes =
  {
    '_major_version',
    '_minor_version',
    '_micro_version',
  }

  local GModule = lgi.GModule
  local Module = GModule.Module
  local symbols

  for _, namespace in ipairs (namespaces) do
    local variable = namespace.variable
    local preffix = namespace.preffix
    local library = namespace.library
    local name = namespace.name
    local id = namespace.id
    local ver = {}

    if (library) then
      local libname = Module.build_path (nil, library)
      symbols = ffi.load (libname)
    else
      symbols = ffi.C
    end

    for _, suffix in ipairs (suffixes) do
      local thing, expression

      if (variable) then
        thing = ('%s' .. suffix):format (preffix)
        expression = ('const unsigned int ' .. thing .. ';')
      else
        thing = ('%s_get' .. suffix):format (preffix)
        expression = ('unsigned int  ' .. thing .. ' ();')
      end

      ffi.cdef (expression)

      if (not symbols[thing]) then
        error ('undefined ' .. thing)
      else
        if (variable) then
          local value = tonumber (symbols[thing]);
          table.insert (ver, value)
        else
          local value = tonumber (symbols[thing] ());
          table.insert (ver, value)
        end
      end
    end

    local mayor = ver [1]
    local minor = ver [2]
    local micro = ver [3]
    local full = ("%i.%i.%i"):format (mayor, minor, micro)
    version[id] = ('%s %s'):format (name, full)
  end
end
return version
