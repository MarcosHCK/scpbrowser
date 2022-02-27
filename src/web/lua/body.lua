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

function body.begin ()
?>
  <div class="scp-container">
    <main>
<?lua
end

function body.finish ()
?>
    </main>
  </div>
<?lua
end

function body.headerbar ()
?>
  <div class="scp-navigation-headerbar">
    <img class="scp-logo" src="resources:///res/logo_symbolic.svg" style="height: 70px;"/>
    <div style="margin: 5px;">
      <a class="scp-black-link" href="scp:///home">
        <h1 class="stylized-script">SCP Foundation</h1>
      </a>
      <h2>Sercure, Contain, Protect</h2>
    </div>
  </div>
<?lua
end

?>
