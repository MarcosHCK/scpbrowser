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
local tag = require ('html.tag')
local utils = require ('html.utils')
local head = {}

do
  local root = config.resources.root
  local builtin_css = (root:get_child ('css/builtin.css')):get_uri ()
  local builtin_js = (root:get_child ('js/builtin.js')):get_uri ()
  local bootstrap = (root:get_child ('js/bootstrap.bundle.min.js')):get_uri ()
  local jquery = (root:get_child ('js/jquery.js')):get_uri ()

  function head.header ()
    tag.meta ({ charset = 'UTF-8' })
    tag.meta ({ ['http-equiv'] = 'content-language', content = 'en', })
    tag.meta ({ ['http-equiv'] = 'content-type', content = 'text/html; charset=UTF-8', })
    tag.meta ({ name = 'author', content = 'MarcosHCK', })
    tag.meta ({ name = 'viewport', content = 'width=device-width, initial-scale=1.0', })
    tag.link ({ type = 'text/css', href = builtin_css, rel = 'stylesheet', charset = 'utf-8', media = 'all', })
    tag.script ({ type = 'text/javascript', src = jquery, })
  end

  function head.footer ()
    tag.script ({ type = 'text/javascript', src = builtin_js, })
    tag.script ({ type = 'text/javascript', src = bootstrap, })
  end
end
return head
