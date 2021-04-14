/*
 * @File:   lua2rtt.c
 * @Author: liu2guang
 * @Date:   2018-05-06 09:16:56
 *
 * @LICENSE: https://github.com/liu2guang/lua2rtt/blob/master/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-06     liu2guang    The first version.
 */
#ifndef __LUA2RTT_H_
#define __LUA2RTT_H_

#include <rtthread.h>
#include "luaconf.h"

#define LUA2RTT_USING_DEBUG
#ifndef LUA2RTT_USING_DEBUG
#define LUA2RTT_DBG(fmt, ...)
#else
#define LUA2RTT_DBG(fmt, ...)                \
do{                                          \
    rt_kprintf("[\033[32mLua2RTT\033[0m] "); \
    rt_kprintf(fmt, ##__VA_ARGS__);          \
}while(0)
#endif

#ifndef LUA2RTT_THREAD_STACK_SIZE
#define LUA2RTT_THREAD_STACK_SIZE 10240
#endif
#ifndef LUA2RTT_CMD_SIZE
#define LUA2RTT_CMD_SIZE 512
#endif
#ifndef LUA2RTT_HISTORY_LINES
#define LUA2RTT_HISTORY_LINES 5
#endif

enum lua2rtt_input_stat
{
    LUA2RTT_WAIT_NORMAL,
    LUA2RTT_WAIT_SPEC_KEY,
    LUA2RTT_WAIT_FUNC_KEY,
};

struct lua2rtt
{
    rt_thread_t thread;
    struct rt_semaphore rx_sem;

    enum lua2rtt_input_stat stat;

    int   argc;
    char *argv[3];

    char lua_history[LUA2RTT_HISTORY_LINES][LUA2RTT_CMD_SIZE];
    rt_uint16_t history_count;
    rt_uint16_t history_current;

    char line[LUA2RTT_CMD_SIZE];
    rt_uint16_t line_position;
    rt_uint8_t line_curpos;

    rt_device_t device;
    rt_err_t (*rx_indicate)(rt_device_t dev, rt_size_t size); /* msh??????? */
};
typedef struct lua2rtt *lua2rtt_t;

#endif
