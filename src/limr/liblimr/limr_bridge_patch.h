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
#ifndef __LIMR_BRIDGE_PATCH__
#define __LIMR_BRIDGE_PATCH__ 1
#include <gio/gio.h>

#if __cplusplus
extern "C" {
#endif // __cplusplus

G_GNUC_INTERNAL
GInputStream*
limr_bridge_patch_get_stdin ();
G_GNUC_INTERNAL
GOutputStream*
limr_bridge_patch_get_stdout ();
G_GNUC_INTERNAL
GOutputStream*
limr_bridge_patch_get_stderr ();

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_BRIDGE_PATCH__
