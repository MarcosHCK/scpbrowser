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
  [GtkTemplate (ui = "/org/hck/scpbrowser/ui/notebook_page.ui")]
  public class NotebookPage : Gtk.Box
  {
    [GtkChild]
    private Gtk.Box box1;
    [GtkChild]
    private Gtk.Label label1;
    [GtkChild]
    private Gtk.Spinner spinner1;
    private WebKit.WebView webview1 = null;

    /*
     * Properties
     *
     */

    public bool can_go_back {
      get {
        return webview1.can_go_back ();
      }}
    public bool can_go_forward {
      get {
        return webview1.can_go_forward ();
      }}
    public bool is_loading {
      get {
        return webview1.is_loading;
      }}

    /*
     * Signals
     *
     */

    private void on_close (WebKit.WebView webview)
    {
      var notebook = ((Gtk.Widget) this).parent;
      ((Gtk.Notebook) notebook).detach_tab (this);
    }

    private Gtk.Widget on_create (WebKit.WebView webview, WebKit.NavigationAction action)
    {
      var request = action.get_request ();
      unowned Gtk.Widget window =  this.get_toplevel ();
      assert (window is Gtk.Window);
      Gtk.Widget page = null;

      ((Scp.Window) window).try_open (request.get_uri ());
    return page;
    }

    private bool on_decide_policy (WebKit.WebView webview, WebKit.PolicyDecision decision_, WebKit.PolicyDecisionType type)
    {
      switch (type)
      {
      case WebKit.PolicyDecisionType.NAVIGATION_ACTION:
        var decision = (WebKit.NavigationPolicyDecision)decision_;
        var action = decision.navigation_action;
        if (action.is_redirect ())
        {
          var request = action.get_request ();
          if (request == null)
          {
            /* Weird */
            decision_.ignore ();
            break;
          }

          var msg = new Scp.Message.
          question (@"$(webview.get_uri()) is about to redirect to $(request.get_uri())");

          int response =
          ((Gtk.Dialog) msg).run ();
          ((Gtk.Widget) msg).destroy ();

          switch (response)
          {
          case Gtk.ResponseType.NO:
            decision_.ignore ();
            break;
          case Gtk.ResponseType.YES:
            decision_.use ();
            break;
          }
        }
        break;
      case WebKit.PolicyDecisionType.RESPONSE:
        var decision = decision_ as WebKit.ResponsePolicyDecision;
        if (decision.is_mime_type_supported ())
          return false;

        var response = decision.get_response ();
        var request = decision.get_request ();
        var mime_type = response.get_mime_type ();
        var uri = request.get_uri ();

        var main_resource = webview1.get_main_resource ();
        if (main_resource.get_uri () == uri)
        {
          if (mime_type == "application/pdf")
          {
            webview1.load_uri (@"pdf://$uri");
            return false;
          }
        }

        decision_.download ();
        break;
      }
    return true;
    }

    private void on_load_changed (WebKit.WebView webview, WebKit.LoadEvent event)
    {
      switch (event)
      {
      case WebKit.LoadEvent.STARTED:
        ((Gtk.Widget) spinner1).show ();
        ((Gtk.Widget) webview1).hide ();
        break;
      case WebKit.LoadEvent.FINISHED:
        ((Gtk.Widget) spinner1).hide ();
        ((Gtk.Widget) webview1).show ();
        break;
      }

      var notebook_ = ((Gtk.Widget) this).parent;
      var notebook = ((Gtk.Notebook) notebook_);
      var page_n = notebook.get_current_page ();
      var page = notebook.get_nth_page ((int) page_n);
      notebook.switch_page (page, page_n);
    }

    private bool on_load_failed (WebKit.WebView webview, WebKit.LoadEvent event, string failing_uri, GLib.Error error)
    {
      if (error.code == WebKit.NetworkError.CANCELLED)
        return false;

      var msg = new Scp.Message.error_with_gerror (error);
      ((Gtk.Dialog) msg).run ();
      ((Gtk.Widget) msg).destroy ();
    return true;
    }

    private void on_mouse_target_changed (WebKit.WebView webview, WebKit.HitTestResult hitest, uint modifiers)
    {
      if (hitest.context_is_link ())
      {
        label1.label = WebKit.uri_for_display (hitest.link_uri);
        ((Gtk.Widget) label1).show ();
      }
      else
      {
        ((Gtk.Widget) label1).hide ();
      }
    }

    private bool on_permission_request(WebKit.WebView webview, WebKit.PermissionRequest request)
    {
      string message = @"$(webview.get_uri()) request permission for:\r\n";

      if (request is WebKit.DeviceInfoPermissionRequest)
      {
        message += " - access device info\r\n";
      }
      else
      if (request is WebKit.GeolocationPermissionRequest)
      {
        message += " - access device GPS location\r\n";
      }
      else
      if (request is WebKit.InstallMissingMediaPluginsPermissionRequest)
      {
        var request_ = ((WebKit.InstallMissingMediaPluginsPermissionRequest) request);
        message += @" - install a plugin ($(request_.get_description()))\r\n";
      }
      else
      if (request is WebKit.MediaKeySystemPermissionRequest)
      {
        message += " - access MediaKeySystem API\r\n";
      }
      else
      if (request is WebKit.NotificationPermissionRequest)
      {
        message += " - send notifications\r\n";
      }
      else
      if (request is WebKit.PointerLockPermissionRequest)
      {
        message += " - lock pointer device\r\n";
      }
      else
      if (request is WebKit.UserMediaPermissionRequest)
      {
        var request_ = ((WebKit.UserMediaPermissionRequest) request);
        string type;
        if (request_.is_for_audio_device)
          type = "audio";
        else
        if (request_.is_for_video_device)
          type = "video";
        else
        {
          request.deny ();
          return true;
        }

        message += @" - use user's $type device\r\n";
      }
      else
      if (request is WebKit.WebsiteDataAccessPermissionRequest)
      {
        var request_ = ((WebKit.WebsiteDataAccessPermissionRequest) request);
        message += @" - access $(request_.get_current_domain()) cookies form $(request_.get_requesting_domain())\r\n";
      }
      else
      {
        request.deny ();
        return true;
      }

      var msg = new Scp.Message.
      question (message);

      int response =
      ((Gtk.Dialog) msg).run ();
      ((Gtk.Widget) msg).destroy ();

      switch (response)
      {
      case Gtk.ResponseType.NO:
        request.deny ();
        break;
      case Gtk.ResponseType.YES:
        request.allow ();
        break;
      }
    return true;
    }

    private void on_web_process_terminated (WebKit.WebView webview, WebKit.WebProcessTerminationReason reason)
    {
      on_close (webview1);

      var msg = new Scp.Message.error (@"WebProcess crashed:\r\n - code: $reason\r\n");
      ((Gtk.Dialog) msg).run ();
      ((Gtk.Widget) msg).destroy ();
    }

    private void on_title_notify (GLib.Object webview, GLib.ParamSpec pspec)
    {
      var notebook = ((Gtk.Widget) this).parent;
      var tab = ((Gtk.Notebook) notebook).get_tab_label (this);
      ((NotebookTab) tab).label = ((WebKit.WebView) webview).title;
    }

    /*
     * API
     *
     */

    public void try_open (string url)
    {
      webview1.load_uri (url);
    }

    public void try_close ()
    {
      webview1.try_close ();
    }

    public void go_back () { webview1.go_back (); }
    public void go_forward () { webview1.go_forward (); }
    public void stop_loading () { webview1.stop_loading (); }
    public void reload () { webview1.reload (); }

    /*
     * Constructors
     *
     */

    construct
    {
      var app_ = GLib.Application.get_default ();
      var app = ((Scp.Application) app_);
      assert (app != null);

      webview1 = app.browser.create_view ();
      box1.pack_start (webview1, true, true, 0);
      label1.visible = false;

      webview1.close.connect(on_close);
      webview1.create.connect(on_create);
      webview1.decide_policy.connect(on_decide_policy);
      webview1.load_changed.connect(on_load_changed);
      webview1.load_failed.connect(on_load_failed);
      webview1.mouse_target_changed.connect(on_mouse_target_changed);
      webview1.permission_request.connect(on_permission_request);
      webview1.web_process_terminated.connect(on_web_process_terminated);
      webview1.notify["title"].connect(on_title_notify);
    }

    public NotebookPage ()
    {
      Object ();
    }
  }
}
