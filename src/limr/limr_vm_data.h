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
#ifndef __LIMR_VM_DATA__
#define __LIMR_VM_DATA__ 1
#include <gio/gio.h>
#include <stdio.h>

typedef struct _VmData VmData;
typedef struct _VmStream VmStream;

#define LVMDATA "__LIMR_VMDATA"

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

struct _VmData
{
  FILE* stdout_;
  char* buffer;
  size_t buffsz;
};

struct _VmStream
{
  FILE* handle;

  /*<private>*/
  /* lua implementation specific */
  int dummy;
};

G_GNUC_INTERNAL
int
_limr_data_init (lua_State* L);
G_GNUC_INTERNAL
VmData*
_limr_data_fetch (lua_State* L);
G_GNUC_INTERNAL
int
_limr_data_reset (lua_State* L);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_VM_DATA__
