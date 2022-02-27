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
  [GtkTemplate (ui = "/org/hck/scpbrowser/ui/window.ui")]
  public class Window : Gtk.Window
  {
    [GtkChild]
    private Gtk.Notebook notebook1;
    [GtkChild]
    private Gtk.Button button1;
    [GtkChild]
    private Gtk.Button button2;
    [GtkChild]
    private Gtk.Button button3;

    /*
     * Signals
     *
     */

    [GtkCallback]
    private unowned Gtk.Notebook on_create_window (Gtk.Notebook notebook, Gtk.Widget widget, int x, int y)
    {
      var window = new Scp.Window ();
      var app_ = GLib.Application.get_default ();
      var app = (Gtk.Application) app_;

      ((Gtk.Window) window).set_application (app);
      ((Gtk.Window) window).present ();
    return window.notebook1;
    }

    private void on_page_changed (Gtk.Notebook notebook, bool added)
    {
      int pages = notebook.get_n_pages ();
      notebook.set_show_tabs (pages > 1);

      ((Gtk.Widget) button1).sensitive = (pages > 0);
      ((Gtk.Widget) button2).sensitive = (pages > 0);

      if (pages == 0)
      {
        ((Gtk.Widget) this).destroy ();
      }
    }

    [GtkCallback]
    private void on_page_added (Gtk.Notebook notebook, Gtk.Widget widget, uint n_page)
    {
      on_page_changed (notebook, true);
      ((Gtk.Container) notebook).child_set
      (widget,
       "detachable", true,
       "reorderable", true,
       null);
    }

    [GtkCallback]
    private void on_page_removed (Gtk.Notebook notebook, Gtk.Widget widget, uint n_page)
    {
      on_page_changed (notebook, false);
    }

    [GtkCallback]
    private void on_switch_page (Gtk.Notebook notebook, Gtk.Widget widget, uint n_page)
    {
      Scp.NotebookPage page = widget as Scp.NotebookPage;
      ((Gtk.Widget) button1).sensitive = page.can_go_back;
      ((Gtk.Widget) button2).sensitive = page.can_go_forward;

      var image = ((Gtk.Bin)button3).get_child () as Gtk.Image;
      if (page.is_loading == true)
        image.set_from_icon_name ("process-stop-symbolic", Gtk.IconSize.BUTTON);
      else
        image.set_from_icon_name ("reload-symbolic", Gtk.IconSize.BUTTON);
    }

    [GtkCallback]
    private void on_go_back (Gtk.Button button)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int) page_n) as Scp.NotebookPage;
      page.go_back ();
      on_switch_page (notebook1, page, page_n);
    }

    [GtkCallback]
    private void on_go_next (Gtk.Button button)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int) page_n) as Scp.NotebookPage;
      page.go_forward ();
      on_switch_page (notebook1, page, page_n);
    }

    [GtkCallback]
    private void on_go_home (Gtk.Button button)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int)page_n) as Scp.NotebookPage;
      page.try_open ("scp:///home");
    }

    [GtkCallback]
    private void on_reload (Gtk.Button button)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int) page_n) as Scp.NotebookPage;

      if(page.is_loading == true)
      {
        var cancellable = GLib.Cancellable.get_current();
        if(cancellable == null)
          page.stop_loading();
        else
          cancellable.cancel();
      }
      else
      {
        page.reload();
      }
    }

    private void on_is_loading (GLib.Object webview_, GLib.ParamSpec pspec)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int) page_n);
      on_switch_page (notebook1, page, page_n);
    }

    [GtkCallback]
    private void on_preferences (Gtk.Button button)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int) page_n) as Scp.NotebookPage;
      page.try_open ("scp:///preferences");
    }

    [GtkCallback]
    private void on_about (Gtk.Button button)
    {
      uint page_n = notebook1.get_current_page ();
      var page = notebook1.get_nth_page ((int) page_n) as Scp.NotebookPage;
      page.try_open ("scp:///about");
    }

    [GtkCallback]
    private void on_close (Gtk.Button button)
    {
      this.close ();
    }

    [GtkCallback]
    private bool on_delete_event (Gtk.Widget widget, Gdk.Event event)
    {
      ((Gtk.Container) this.notebook1).@foreach
      (
        (child) =>
        {
          if (child is Scp.NotebookPage)
          {
            var page = child as Scp.NotebookPage;
            page.try_close ();
          }
        });
    return true;
    }

    /*
     * API
     *
     */

    public Gtk.Widget try_open (string url)
    {
      var tab = new Scp.NotebookTab.with_label ("SCP Foundation");
      var page_ = new Scp.NotebookPage ();
      int page_n = notebook1.append_page (page_, tab);
      notebook1.set_current_page (page_n);
      page_.try_open (url);
      page_.notify["is_loading"].connect (on_is_loading);
    return page_ as Gtk.Widget;
    }

    /*
     * Constructors
     *
     */

    public Window ()
    {
      Object ();
      notebook1.set_group_name ("scp-page");
    }
  }
}
