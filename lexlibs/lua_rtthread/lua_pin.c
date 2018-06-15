/*
 * @File:   lua_pin.c
 * @Author: liu2guang
 * @Date:   2018-05-09 02:21:05
 *
 * @LICENSE: https://github.com/liu2guang/lua2rtt/blob/master/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-09     liu2guang    The first version.
 */

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <rtdevice.h>

static int pin_mode(lua_State *L)
{
    uint32_t pin  = (uint32_t)luaL_checknumber(L, 1); 
    uint32_t mode = (uint32_t)luaL_checknumber(L, 2); 
    rt_pin_mode(pin, mode); 
    return 1;
}

static int pin_write(lua_State *L)
{
    uint32_t pin   = (uint32_t)luaL_checknumber(L, 1); 
    uint32_t value = (uint32_t)luaL_checknumber(L, 2); 
    rt_pin_write(pin, value); 
    return 1;
}

static int pin_read(lua_State *L)
{
    uint32_t value = 0; 
    uint32_t pin = (uint32_t)luaL_checknumber(L, 1); 
    value = rt_pin_read(pin); 
    lua_pushnumber(L, (lua_Number)(value)); 
    return 1;
}

// Module function map s
#define MIN_OPT_LEVEL 2 
#include "lrodefs.h" 
static const LUA_REG_TYPE pin_map[] =
{
    {LSTRKEY("mode") , LFUNCVAL(pin_mode) }, 
    {LSTRKEY("write"), LFUNCVAL(pin_write)}, 
    {LSTRKEY("read") , LFUNCVAL(pin_read) }, 
    {LNILKEY, LNILKEY} 
}; 

LUALIB_API int luaopen_pin(lua_State *L)
{
    LREGISTER(L, "pin", pin_map); 
} 
