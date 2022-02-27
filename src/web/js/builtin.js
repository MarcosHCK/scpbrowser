/* Copyright 2021-2025 MarcosHCK
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
 */

(function ()
{
  $(window).on ('load', () =>
  {
    var tags = $('div.scp-expandable')
    var closure

    for (i = 0; i < tags.length; i++)
      {
        function mkclosure ()
          {
            var tag = $(tags[i])
            var closure = () =>
              {
                var title = tag.children('.title')
                var content = tag.children('.content')
                var callback =
                () => { content.toggleClass('collapsed') }

                title.on('click', callback)
                callback ()
              }

          return closure
          }

        var closure = mkclosure ();
          closure ()
      }
  })
}) (jQuery)
