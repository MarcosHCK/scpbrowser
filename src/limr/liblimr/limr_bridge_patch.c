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
#include <config.h>
#include <limr_bridge_patch.h>

#ifdef G_PLATFORM_WIN32
# include <gio-win32-2.0/gio/gwin32inputstream.h>
# include <gio-win32-2.0/gio/gwin32outputstream.h>
# include <windows.h>
#else // !G_PLATFORM_WIN32
# include <gio-unix-2.0/gio/gunixinputstream.h>
# include <gio-unix-2.0/gio/gunixoutputstream.h>
# include <stdio.h>
#endif // G_PLATFORM_WIN32

G_GNUC_INTERNAL
GInputStream*
limr_bridge_patch_get_stdin ()
{
#if G_PLATFORM_WIN32
  return (GInputStream*) g_win32_input_stream_new ((void*) STD_INPUT_HANDLE, FALSE);
#else // !G_PLATFORM_WIN32
  return (GInputStream*) g_unix_input_stream_new (STDIN_FILENO, FALSE);
#endif // G_PLATFORM_WIN32
}

G_GNUC_INTERNAL
GOutputStream*
limr_bridge_patch_get_stdout ()
{
#if G_PLATFORM_WIN32
  return (GOutputStream*) g_win32_output_stream_new ((void*) STD_OUTPUT_HANDLE, FALSE);
#else // !G_PLATFORM_WIN32
  return (GOutputStream*) g_unix_output_stream_new (STDOUT_FILENO, FALSE);
#endif // G_PLATFORM_WIN32
}

G_GNUC_INTERNAL
GOutputStream*
limr_bridge_patch_get_stderr ()
{
#if G_PLATFORM_WIN32
  return (GOutputStream*) g_win32_output_stream_new ((void*) STD_ERROR_HANDLE, FALSE);
#else // !G_PLATFORM_WIN32
  return (GOutputStream*) g_unix_output_stream_new (STDERR_FILENO, FALSE);
#endif // G_PLATFORM_WIN32
}
