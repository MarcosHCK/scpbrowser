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
  public enum MessageType
  {
    QUESTION,
    MESSAGE,
    WARNING,
    ERROR,
  }

  [GtkTemplate (ui = "/org/hck/scpbrowser/ui/message.ui")]
  public class Message : Gtk.Dialog
  {
    [GtkChild]
    private Gtk.Image image1;
    [GtkChild]
    private Gtk.Label label1;

    public Message.question (string text)
    {
      Object ();

      ((Gtk.Dialog) this).add_buttons
      (
        _("No"), Gtk.ResponseType.NO,
        _("Yes"), Gtk.ResponseType.YES,
        null);

      this.label1.label = text;
      this.image1.set_from_icon_name ("dialog-question-symbolic", Gtk.IconSize.DIALOG);
    }

    public Message.message (string text)
    {
      Object ();

      ((Gtk.Dialog) this).add_buttons
      (
        _("OK"), Gtk.ResponseType.OK,
        null);

      this.label1.label = text;
      this.image1.set_from_icon_name ("dialog-information-symbolic", Gtk.IconSize.DIALOG);
    }

    public Message.warning (string text)
    {
      Object();

      ((Gtk.Dialog) this).add_buttons
      (
        _("OK"), Gtk.ResponseType.OK,
        null);

      this.label1.label = text;
      this.image1.set_from_icon_name ("dialog-warning-symbolic", Gtk.IconSize.DIALOG);
    }

    public Message.error (string text)
    {
      Object();

      ((Gtk.Dialog) this).add_buttons
      (
        _("OK"), Gtk.ResponseType.OK,
        null);

      this.label1.label = text;
      this.image1.set_from_icon_name ("dialog-error-symbolic", Gtk.IconSize.DIALOG);
    }

    public Message.error_with_gerror (GLib.Error error)
    {
      Object();

      ((Gtk.Dialog) this).add_buttons
      (
        _("OK"), Gtk.ResponseType.OK,
        null);

      this.label1.label = @"Thrown error:\r\n - from: $(error.domain)\r\n - code: $(error.code)\r\n\r\n$(error.message)";
      this.image1.set_from_icon_name ("dialog-error-symbolic", Gtk.IconSize.DIALOG);
    }
  }
}
