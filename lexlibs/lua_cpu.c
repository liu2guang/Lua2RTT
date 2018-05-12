/*
 * @File:   lua_cpu.c
 * @Author: liu2guang
 * @Date:   2018-05-06 09:16:56
 *
 * @LICENSE: https://github.com/liu2guang/lua2rtt/blob/master/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-06     liu2guang    The first version.
 */

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define TO_32BIT(L, n) (((signed long long)luaL_checknumber((L), (n))) & 0x0ffffffff) 

/* Lua: data = r08(address) */ 
static int cpu_r08(lua_State *L)
{
    uint32_t addr = 0;
    luaL_checkinteger(L, 1);
    addr = (uint32_t)TO_32BIT(L, 1);
    lua_pushnumber(L, (lua_Number)(*(volatile uint8_t *)addr)); 
    return 1;
}

/* Lua: w08(address, data) */ 
static int cpu_w08(lua_State *L)
{
    uint32_t addr;
    uint8_t data = (uint8_t)TO_32BIT(L, 2); 

    luaL_checkinteger(L, 1);
    addr = (uint32_t)TO_32BIT(L, 1);
    *(volatile uint8_t*)addr = data; 
    
    return 0; 
}

/* Lua: data = r16(address) */
static int cpu_r16(lua_State *L)
{
    uint32_t addr = 0;
    luaL_checkinteger(L, 1);
    addr = (uint32_t)TO_32BIT(L, 1);
    lua_pushnumber(L, (lua_Number)(*(volatile uint16_t *)addr)); 
    return 1;
}

/* Lua: w16(address, data) */ 
static int cpu_w16(lua_State *L)
{
    uint32_t addr;
    uint16_t data = (uint16_t)TO_32BIT(L, 2);

    luaL_checkinteger(L, 1);
    addr = (uint32_t)TO_32BIT(L, 1);
    *(volatile uint16_t*)addr = data; 
    
    return 0; 
}

/* Lua: data = r32(address) */
static int cpu_r32(lua_State *L)
{
    uint32_t addr = 0;
    luaL_checkinteger(L, 1);
    addr = (uint32_t)TO_32BIT(L, 1);
    lua_pushnumber(L, (lua_Number)(*(volatile uint32_t *)addr)); 
    return 1;
}

/* Lua: w32(address, data) */ 
static int cpu_w32(lua_State *L)
{
    uint32_t addr;
    uint32_t data = (uint32_t)TO_32BIT(L, 2);

    luaL_checkinteger(L, 1);
    addr = (uint32_t)TO_32BIT(L, 1);
    *(volatile uint32_t*)addr = data; 
    
    return 0; 
}

/* Lua: sleep(tick) */ 
static int cpu_sleep(lua_State *L)
{
    rt_thread_delay((uint32_t)TO_32BIT(L, 1)); 
    return 0; 
}

// Module function map 
#define MIN_OPT_LEVEL 2 
#include "lrodefs.h" 
static const LUA_REG_TYPE cpu_map[] =
{
    {LSTRKEY("r08")  , LFUNCVAL(cpu_r08)  }, 
    {LSTRKEY("w08")  , LFUNCVAL(cpu_w08)  }, 
    {LSTRKEY("r16")  , LFUNCVAL(cpu_r16)  }, 
    {LSTRKEY("w16")  , LFUNCVAL(cpu_w16)  }, 
    {LSTRKEY("r32")  , LFUNCVAL(cpu_r32)  }, 
    {LSTRKEY("w32")  , LFUNCVAL(cpu_w32)  }, 
    {LSTRKEY("sleep"), LFUNCVAL(cpu_sleep)}, 
    {LNILKEY, LNILKEY} 
}; 

/* ´ò¿ªCPU¿â */ 
LUALIB_API int luaopen_cpu(lua_State *L)
{
    LREGISTER(L, "cpu", cpu_map); 
} 
