<?lua
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
local body = module ('body')
local config = require ('config')

function body.begin ()
?>
  <div class="container">
    <main>
<?lua
end

function body.finish ()
?>
    </main>
  </div>
  <script src="resources:///js/bootstrap.bundle.min.js"></script>
<?lua
end

function body.headerbar ()
?>
  <div class="container">
    <header class="d-flex flex-wrap justify-content-center py-3 mb-4 border-bottom">
      <a class="d-flex align-items-center mb-3 mb-md-0 me-md-auto text-dark text-decoration-none" href="<?lua io.write (config.links.home) ?>">
        <img class="bi me-2" src="resources:///res/logo_symbolic.svg" style="height: 70px;"/>
        <span class="fs-4">Scp Foundation</span>
      </a>

      <ul class="nav nav-pills">
        <li class="nav-item">
          <a href="<?lua io.write (config.links.home) ?>" class="nav-link">Home</a>
        </li>
        <li class="nav-item">
          <a href="<?lua io.write (config.links.about) ?>" class="nav-link">About</a>
        </li>
      </ul>
    </header>
  </div>
<?lua
end

?>
