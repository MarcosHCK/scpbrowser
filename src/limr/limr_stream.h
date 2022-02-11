/* Copyright 2021-2025 MarcosHCK
 * This file is part of libLimr.
 *
 * libLimr is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libLimr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libLimr.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __LIMR_STREAM__
#define __LIMR_STREAM__ 1
#include <gmodule.h>
#include <private.h>
#include <stdio.h>

typedef struct _LimrStream LimrStream;
typedef struct _LStream LStream;

#if __cplusplus
extern "C" {
#endif // __cplusplus

struct _LStream
{
  FILE* handle;

  /* Lua-specific data */
  int _private_;
};

G_GNUC_INTERNAL
void
_limr_stream_new (lua_State* L);
G_GNUC_INTERNAL
void
_limr_stream_setup (lua_State* L, int env);
G_GNUC_INTERNAL
void
_limr_stream_close (lua_State* L, int env);
G_GNUC_INTERNAL
GBytes*
_limr_stream_dump (lua_State* L);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIMR_STREAM__
