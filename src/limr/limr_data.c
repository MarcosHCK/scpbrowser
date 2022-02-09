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
#include <config.h>
#include <limr_data.h>
#include <stdio.h>

typedef struct _VmData VmData;
typedef struct _VmStream VmStream;

#define LVMDATA "__LIMR_VMDATA"

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

static int
__gc (lua_State* L)
{
  VmData* data =
  lua_touserdata (L, 1);
  {
    fflush (data->stdout_);
    fclose (data->stdout_);
    free (data->buffer);
  }
return 0;
}

G_GNUC_INTERNAL
VmData*
_fetch (lua_State* L)
{
  VmData* data = NULL;
  lua_getfield (L, LUA_REGISTRYINDEX, LVMDATA);
  data = lua_touserdata (L, -1);
  lua_pop (L, 1);
return data;
}

/*
 * API
 *
 */



G_GNUC_INTERNAL
int
_limr_data_init (lua_State* L)
{
  VmData* data =
  lua_newuserdata (L, sizeof (VmData));
  lua_createtable (L, 0, 1);
  lua_pushliteral (L, "__gc");
  lua_pushcfunction (L, __gc);
  lua_settable (L, -3);
  lua_setmetatable (L, -2);
  lua_setfield (L, LUA_REGISTRYINDEX, LVMDATA);

  data->stdout_ =
  open_memstream
  (&(data->buffer),
   &(data->buffsz));
return 0;
}

G_GNUC_INTERNAL
int
_limr_data_reset (lua_State* L)
{
  gboolean success = FALSE;
  FILE* stdout_ = NULL;
  VmData* data = NULL;

  data = _fetch (L);
  stdout_ = data->stdout_;

  success =
  (fseek (stdout_, 0, SEEK_SET) == 0);
  if (G_UNLIKELY (success == FALSE))
  {
    int en = errno;
    lua_pushfstring (L, "%s", strerror (en));
    lua_error(L);
  }
return 0;
}

G_GNUC_INTERNAL
int
_limr_data_dump (lua_State* L, GOutputStream* stdout_, GCancellable* cancellable, GError** error)
{
return 0;
}
