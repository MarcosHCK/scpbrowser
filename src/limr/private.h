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
#ifndef __LIBLIMR_PRIVATE__
#define __LIBLIMR_PRIVATE__ 1
#include <gmodule.h>

#if __cplusplus
extern "C" {
#endif // __cplusplus

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

G_MODULE_EXPORT
int
luaopen_liblimr (lua_State* L);

#if __cplusplus
}
#endif // __cplusplus

#endif // __LIBLIMR_PRIVATE__
