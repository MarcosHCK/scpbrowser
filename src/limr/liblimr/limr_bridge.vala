/* Copyright 2021-2025 MarcosHCK
 * This file is part of liblimr.
 *
 * liblimr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liblimr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liblimr.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

namespace Limr
{
  const uint8 stopchars[] = {'\0'};

  public abstract class Bridge : GLib.Object
  {
    private GLib.Cancellable? watchdog = null;
    private GLib.Task? watcher = null;
    private int hold;

    public bool running {
      get {
        return (watcher != null) && (!watchdog.is_cancelled ());
      }}

    /*
     * Watcher
     *
     */

    protected abstract void watch_inner (GLib.Cancellable? cancellable = null) throws GLib.Error;

    private static void watch (GLib.Task task, GLib.Object source_object, void* task_data, GLib.Cancellable? cancellable = null)
    {
      var instance = (Limr.Bridge) task_data;

      try
      {
        GLib.AtomicInt.set (ref instance.hold, (int) true);
        instance.watch_inner (cancellable);
      } catch (GLib.Error e)
      {
        task.return_error (e);
      }
      finally
      {
        GLib.AtomicInt.set (ref instance.hold, (int) false);
      }

      task.return_boolean (true);
    }

    private static void watch_finished (GLib.Object? source, GLib.AsyncResult res)
    {
      var task = res as GLib.Task;
      var cancellable = task.get_cancellable ();
      if (cancellable.is_cancelled () == false)
      {
        try
        {
          task.propagate_boolean ();
          critical ("Watcher thread was unexpectelly stopped");
          assert_not_reached ();
        } catch (GLib.Error e)
        {
          critical (@"Watcher thread throws an error: $(e.domain):$(e.code):$(e.message)");
          assert_not_reached ();
        }
      }
    }

    /*
     * API
     *
     */

    public void begin ()
    {
      watchdog = new GLib.Cancellable ();
      watcher = new GLib.Task (watchdog, watchdog, watch_finished);

      watcher.set_name (@"[Task] $(this.get_type ().name ()).watcher");
      watcher.set_priority (GLib.Priority.DEFAULT);
      watcher.set_return_on_cancel (true);
      watcher.set_task_data (this, null);

      GLib.AtomicInt.set (ref hold, (int) false);
      watcher.run_in_thread (watch);
    }

    public void end ()
    {
      watchdog.cancel ();
      while (GLib.AtomicInt.get (ref hold) == (int) true)
        GLib.Thread.yield ();
      watchdog = null;
      watcher = null;
    }
  }
}
