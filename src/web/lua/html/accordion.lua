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
local callable = require ('html.callable')
local tag = require ('html.tag')
local utils = require ('html.utils')
local accordion = {}

do
  function accordion.item (title, content, parentid, collapsed)
    checkArg (1, title, 'string')
    checkArg (3, parentid, 'string')
    checkArg (4, collapsed, 'boolean', 'nil')
    content = callable.create (content)

    local headerid = utils.uniqueid ()
    local contentid = utils.uniqueid ()

    tag.div ({ class = 'accordion-item', },
    function ()
      tag.h2 ({ class = 'accordion-header', id = headerid, },
      function ()
        local attrs
        do
          attrs =
          {
            class = 'accordion-button collapsed',
            type = 'button',
            ['data-bs-toggle'] = 'collapse',
            ['data-bs-target']= '#' .. contentid,
            ['aria-expanded']= 'false',
            ['aria-controls']= contentid,
          }
        end

        tag.button (attrs, function () tag.span ({}, title) end)
      end)

      local attrs
      do
        attrs =
        {
          id = contentid,
          class= 'accordion-collapse collapse',
          ['aria-labelledby'] = headerid,
          ['data-bs-parent'] = '#' .. parentid,
        }
      end

      tag.div (attrs,
      function ()
        tag.div ({ class = 'accordion-body', },
        function ()
          content (parentid)
        end)
      end)
    end)
  end

  -- Piece it together

  function accordion.spawn (iterator)
    if (type (iterator) ~= 'table'
      and not callable.is (iterator)) then
      checkArg (1, nil, 'table', 'function')
    end

    if (not callable.is (iterator)) then
      local iter, tab, last = ipairs (iterator)
      local value

      iterator = function ()
        last, value = iter (tab, last)
        return value
      end
    end

    local parentid = utils.uniqueid ()
    tag.div ({ class = 'accordion', id = parentid, },
    function ()
      repeat
        local value = iterator ()
        if (not value) then
          break
        else
          local title = value.title
          local content = value.content
          accordion.item (title, content, parentid)
        end
      until (false)
    end)
  end
end
return accordion
