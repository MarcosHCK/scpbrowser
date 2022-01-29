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
  public class Settings : GLib.Object, GLib.Initable
  {
    GLib.SettingsSchemaSource source = null;
    GLib.HashTable<string, GLib.SettingsSchema> schemas = null;

    public bool init (GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      try
      {
#if DEVELOPER == 1
        source = new GLib.SettingsSchemaSource.from_directory
        (Config.ABSTOPBUILDDIR + "/settings/",
         GLib.SettingsSchemaSource.get_default (),
         true);
#else
        source = new GLib.SettingsSchemaSource.from_directory
        (Config.SCHEMASDIR,
         GLib.SettingsSchemaSource.get_default (),
         true);
#endif // DEVELOPER
      }
      catch (GLib.Error e)
      {
        throw e;
      }

      schemas = new GLib.HashTable<string, GLib.SettingsSchema> (GLib.str_hash, GLib.str_equal);
    return true;
    }

    public GLib.Settings? get_settings (string schema_id)
    {
      var schema = schemas.lookup (schema_id);
      if (unlikely (schema == null))
      {
        schema = source.lookup (schema_id, true);
        if (unlikely (schema == null))
          return null;

        schemas.insert (schema_id, schema);
        return this.get_settings (schema_id);
      }
    return new GLib.Settings.full (schema, null, null);
    }

    public Settings(GLib.Cancellable? cancellable = null) throws GLib.Error
    {
      Object();
      this.init (cancellable);
    }
  }
}
