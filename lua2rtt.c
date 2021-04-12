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
 * 2018-07-03     liu2guang    Update lua5.1.2->lua5.3.4.
 */

#include "lua2rtt.h"
#include "shell.h"

static struct lua2rtt handle = {0};

static rt_uint8_t lua2rtt_getc(void)
{
    rt_uint8_t ch = 0;
    while(rt_device_read(handle.device, (-1), &ch, 1) != 1)
    {
        rt_sem_take(&(handle.rx_sem), RT_WAITING_FOREVER);
    }
    return ch;
}

static rt_err_t lua2rtt_rxcb(rt_device_t dev, rt_size_t size)
{
    return rt_sem_release(&(handle.rx_sem));
}

static rt_bool_t lua2rtt_handle_history(const char *prompt)
{
    rt_kprintf("\033[2K\r");
    rt_kprintf("%s%s", prompt, handle.line);
    return RT_FALSE;
}

static void lua2rtt_push_history(void)
{
    if(handle.line_position > 0)
    {
        if(handle.history_count >= LUA2RTT_HISTORY_LINES)
        {
            if(rt_memcmp(&handle.lua_history[LUA2RTT_HISTORY_LINES-1], handle.line, LUA2RTT_CMD_SIZE))
            {
                int index;
                for(index = 0; index < FINSH_HISTORY_LINES - 1; index ++)
                {
                    rt_memcpy(&handle.lua_history[index][0], &handle.lua_history[index+1][0], LUA2RTT_CMD_SIZE);
                }
                rt_memset(&handle.lua_history[index][0], 0, LUA2RTT_CMD_SIZE);
                rt_memcpy(&handle.lua_history[index][0], handle.line, handle.line_position);

                handle.history_count = LUA2RTT_HISTORY_LINES;
            }
        }
        else
        {
            if(handle.history_count == 0 || rt_memcmp(&handle.lua_history[handle.history_count-1], handle.line, LUA2RTT_CMD_SIZE))
            {
                handle.history_current = handle.history_count;
                rt_memset(&handle.lua_history[handle.history_count][0], 0, LUA2RTT_CMD_SIZE);
                rt_memcpy(&handle.lua_history[handle.history_count][0], handle.line, handle.line_position);

                handle.history_count++;
            }
        }
    }

    handle.history_current = handle.history_count;
}

int lua2rtt_readline(const char *prompt, char *buffer, int buffer_size)
{
    rt_uint8_t ch;

start:
    rt_kprintf(prompt);

    while(1)
    {
        ch = lua2rtt_getc();

        if(ch == 0x1b)
        {
            handle.stat = LUA2RTT_WAIT_SPEC_KEY;
            continue;
        }
        else if(handle.stat == LUA2RTT_WAIT_SPEC_KEY)
        {
            if(ch == 0x5b)
            {
                handle.stat = LUA2RTT_WAIT_FUNC_KEY;
                continue;
            }
            handle.stat = LUA2RTT_WAIT_NORMAL;
        }
        else if(handle.stat == LUA2RTT_WAIT_FUNC_KEY)
        {
            handle.stat = LUA2RTT_WAIT_NORMAL;

            if(ch == 0x41)
            {
                if(handle.history_current > 0)
                {
                    handle.history_current--;
                }
                else
                {
                    handle.history_current = 0;
                    continue;
                }

                rt_memcpy(handle.line, &handle.lua_history[handle.history_current][0], LUA2RTT_CMD_SIZE);
                handle.line_curpos = handle.line_position = rt_strlen(handle.line);
                lua2rtt_handle_history(prompt);

                continue;
            }
            else if(ch == 0x42)
            {
                if(handle.history_current < (handle.history_count-1))
                {
                    handle.history_current++;
                }
                else
                {
                    if(handle.history_count != 0)
                    {
                        handle.history_current = handle.history_count-1;
                    }
                    else
                    {
                        continue;
                    }
                }

                rt_memcpy(handle.line, &handle.lua_history[handle.history_current][0], LUA2RTT_CMD_SIZE);
                handle.line_curpos = handle.line_position = rt_strlen(handle.line);
                lua2rtt_handle_history(prompt);

                continue;
            }
            else if(ch == 0x44)
            {
                if(handle.line_curpos)
                {
                    rt_kprintf("\b");
                    handle.line_curpos--;
                }
                continue;
            }
            else if(ch == 0x43)
            {
                if(handle.line_curpos < handle.line_position)
                {
                    rt_kprintf("%c", handle.line[handle.line_curpos]);
                    handle.line_curpos++;
                }
                continue;
            }
        }

        if(ch == '\0' || ch == 0xFF)
        {
            continue;
        }

        else if(ch == 0x7f || ch == 0x08)
        {
            if(handle.line_curpos == 0)
            {
                continue;
            }

            handle.line_curpos--;
            handle.line_position--;

            if(handle.line_position > handle.line_curpos)
            {
                rt_memmove(&handle.line[handle.line_curpos], &handle.line[handle.line_curpos+1],
                    handle.line_position-handle.line_curpos);
                handle.line[handle.line_position] = 0;

                rt_kprintf("\b%s  \b", &handle.line[handle.line_curpos]);

                int index;
                for(index = (handle.line_curpos); index <= (handle.line_position); index++)
                {
                    rt_kprintf("\b");
                }
            }
            else
            {
                rt_kprintf("\b \b");
                handle.line[handle.line_position] = 0;
            }
            continue;
        }

        else if(ch == '\r' || ch == '\n')
        {
            lua2rtt_push_history();

            rt_kprintf("\n");
            if(handle.line_position == 0)
            {
                goto start;
            }
            else
            {
                rt_uint8_t temp = handle.line_position;

                rt_strncpy(buffer, handle.line, handle.line_position);
                rt_memset(handle.line, 0x00, sizeof(handle.line));
                buffer[handle.line_position] = 0;
                handle.line_curpos = handle.line_position = 0;
                return temp;
            }
        }

        else if(ch == 0x04)
        {
            if(handle.line_position == 0)
            {
                return 0;
            }
            else
            {
                continue;
            }
        }

        else if(ch == '\t')
        {
            continue;
        }

        else if(ch < 0x20 || ch >= 0x80)
        {
            continue;
        }

        if(handle.line_position >= LUA2RTT_CMD_SIZE)
        {
            continue;
        }

        if(handle.line_curpos < handle.line_position)
        {
            rt_memmove(&handle.line[handle.line_curpos+1], &handle.line[handle.line_curpos],
                handle.line_position-handle.line_curpos);
            handle.line[handle.line_curpos] = ch;

            rt_kprintf("%s", &handle.line[handle.line_curpos]);

            int index;
            for(index = (handle.line_curpos); index < (handle.line_position); index++)
            {
                rt_kprintf("\b");
            }
        }
        else
        {
            handle.line[handle.line_position] = ch;
            rt_kprintf("%c", ch);
        }

        ch = 0;
        handle.line_curpos++;
        handle.line_position++;
    }
}

static void lua2rtt_run(void *p)
{
    const char *device_name = RT_CONSOLE_DEVICE_NAME;

    handle.device = rt_device_find(device_name);
    if(handle.device == RT_NULL)
    {
        LUA2RTT_DBG("The msh device find failed.\n");
        return;
    }

    handle.rx_indicate = handle.device->rx_indicate;
    rt_device_set_rx_indicate(handle.device, lua2rtt_rxcb);

    if(handle.argc == 1)
    {
        rt_kprintf("\nPress CTRL+D to exit Lua.\n");
    }
    extern int lua_main(int argc, char **argv);
    lua_main(handle.argc, handle.argv);

    if(handle.argc > 1)
    {
        rt_free(handle.argv[1]);
    }

    rt_sem_detach(&(handle.rx_sem));
    rt_device_set_rx_indicate(handle.device, handle.rx_indicate);
    // rt_kprintf("Exit Lua interactive mode.\n");
    rt_kprintf(FINSH_PROMPT);
}

static int lua2rtt(int argc, char **argv)
{
    static rt_bool_t history_init = RT_FALSE;

    if(history_init == RT_FALSE)
    {
        rt_memset(&handle, 0x00, sizeof(struct lua2rtt));
        history_init = RT_TRUE;
    }
    else
    {
        handle.thread = RT_NULL;
        handle.stat = LUA2RTT_WAIT_NORMAL;
        handle.argc = 0;
        handle.argv[0] = RT_NULL;
        handle.argv[1] = RT_NULL;
        handle.argv[2] = RT_NULL;
        rt_memset(handle.line, 0x00, LUA2RTT_CMD_SIZE);
        handle.line_position = 0;
        handle.line_curpos = 0;
        handle.device = RT_NULL;
        handle.rx_indicate = RT_NULL;
    }

    rt_sem_init(&(handle.rx_sem), "lua2rtt_rxsem", 0, RT_IPC_FLAG_FIFO);

    handle.argc = argc;

    handle.argv[0] = "lua";
    handle.argv[1] = RT_NULL;
    handle.argv[2] = RT_NULL;

    if(argc > 1)
    {
        rt_size_t len = rt_strlen(argv[1]);
        handle.argv[1] = (char *)rt_malloc(len + 1);
        rt_memset(handle.argv[1], 0x00, len + 1);
        rt_strncpy(handle.argv[1], argv[1], len);
    }

    rt_uint8_t prio = rt_thread_self()->current_priority+1;
    handle.thread = rt_thread_create("lua", lua2rtt_run, RT_NULL, LUA2RTT_THREAD_STACK_SIZE, prio, 10);
    if(handle.thread == RT_NULL)
    {
        rt_sem_detach(&(handle.rx_sem));
        LUA2RTT_DBG("The Lua interpreter thread create failed.\n");
        return RT_ERROR;
    }
    rt_thread_startup(handle.thread);

    return RT_EOK;
}
MSH_CMD_EXPORT_ALIAS(lua2rtt, lua, Execute Lua parser.);
