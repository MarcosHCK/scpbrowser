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
local tag = require ('html.tag')
local utils = require ('html.utils')
local navbar = {}

do
  function navbar.begin (title)
    checkArg (1, title, 'string', 'nil')
    local title = title or 'Scp Foundation'

    utils.printf ('<div class="container">')
    utils.printf ('<div class="d-flex flex-wrap justify-content-center py-3 mb-4 border-bottom">')
    do
      local backlink = utils.escape (config.links.home)
      utils.printf ('<a class="d-flex align-items-center mb-3 mb-md-0 me-md-auto text-dark text-decoration-none" href="%s">', backlink)
      do
        local iconlink = utils.escape (config.resources.icon)
        local title = utils.escape (title)
        utils.printf ('<img class="bi me-2" src="%s" style="height: 70px;" />', iconlink)
        utils.printf ('<span class="fs-4">%s</span>', title)
      end
      utils.printf ('</a>')
    end
  end

  function navbar.finish ()
    utils.printf ('</div>')
    utils.printf ('</div>')
  end

  function navbar.item (content)
    content = callable.create (content)
    tag.li ({ class = 'nav-item', },
    function ()
      content ()
    end)
  end

  function navbar.link (text, target)
    checkArg (1, text, 'string')
    checkArg (2, target, 'string')

    local title = utils.escape (text)
    local link = utils.escape (target)

    tag.a ({ class = 'nav-link', href = link, },
    function ()
      tag.span ({}, title)
    end)
  end

  function navbar.spawn (title, iterator)
    checkArg (1, title, 'string', 'nil')
    if (type (iterator) ~= 'table'
      and not callable.is (iterator)) then
      checkArg (2, nil, 'table', 'function')
    end

    if (not callable.is (iterator)) then
      local iter, tab, last = ipairs (iterator)
      local value

      iterator = function ()
        last, value = iter (tab, last)
        return value
      end
    end

    navbar.begin (title)

    repeat
      local value = iterator ()
      if (not value) then
        break
      else
        utils.printf ('<ul class="nav nav-pills">')
        do
          navbar.item (value)
        end
        utils.printf ('</ul')
      end
    until (false)

    navbar.finish ()
  end
end
return navbar
