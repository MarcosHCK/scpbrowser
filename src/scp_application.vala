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

namespace Scp
{
  public class Application : Gtk.Application, GLib.Initable
  {
    private Scp.Browser _browser = null;
    private Scp.Window window = null;
    private Scp.Settings settings = null;
    private GLib.Settings gsettings = null;

    public Scp.Browser browser {
      get {
        return _browser;
      }
    private set {
        _browser = value;
      }}

    /*
     * Signals
     *
     */

    private void on_parsing_error(Gtk.CssProvider provider,
                                  Gtk.CssSection section,
                                  GLib.Error error)
    {
      var file_ = section.get_file ();
      unowned string file = null;

      if (file_ != null)
        file = file_.peek_path ();
      else
        file = "string";

      var message = @"CSS parsing error:\r\n"
                  + @" - file: $file\r\n"
                  + @" - line: $(section.get_start_line())\r\n"
                  + @" - column: $(section.get_start_position())\r\n"
                  + @" - type: $(section.get_section_type())\r\n"
                  + @" - from: $(error.domain)\r\n"
                  + @" - code: $(error.code)\r\n"
                  + @"\r\n\r\n"
                  + @"$(error.message)\r\n";

      var dialog = new Scp.Message.error (message);
      ((Gtk.Dialog) dialog).run ();
      ((Gtk.Widget) dialog).destroy ();
    }

    /*
     * Overrides
     *
     */

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      /*
       * Settings
       *
       */

      settings = new Scp.Settings (cancellable);
      gsettings = settings.get_settings (Config.GAPPNAME);
    
      /*
       * Load theme patch
       *
       */

      browser = new Scp.Browser ();
      var provider = new Gtk.CssProvider();
      provider.parsing_error.connect (on_parsing_error);
      provider.load_from_resource (Config.GRESNAME + "/css/gtk.css");
      Gtk.StyleContext.add_provider_for_screen
      (Gdk.Screen.get_default (),
       (Gtk.StyleProvider) provider,
       Gtk.STYLE_PROVIDER_PRIORITY_USER);

      /*
       * Apply settings
       *
       */

      gsettings.bind ("use-sandbox", browser, "sandboxed", GLib.SettingsBindFlags.DEFAULT);
    return false;
    }

    public override void activate ()
    {
      GLib.File home = GLib.File.new_for_uri ("scpbrowser:///home");
      GLib.File[] vector = new GLib.File[1] {home};
      this.open (vector, "url");
    }

    public override void open (GLib.File[] files, string hint)
    {
      if (unlikely (window == null))
      {
        window = new Scp.Window ();
        window.set_application (this);
        window.present ();
      }

      foreach (var file in files)
      {
        window.try_open (file.get_uri ());
      }
    }

    /*
     * Constructors
     *
     */

    construct
    {
    }

    public Application (string application_id, GLib.ApplicationFlags flags)
    {
      Object (application_id : application_id, flags : flags);
    }
  }
}
