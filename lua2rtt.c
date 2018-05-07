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
 
#include "lua2rtt.h" 
#include "shell.h"

#if defined(RT_USING_POSIX) 
#include <stdio.h> 
#endif

static struct lua2rtt handle = {0}; 

//static rt_err_t lua2rtt_putc(rt_uint8_t ch)
//{
//#if defined(RT_USING_POSIX) 
//    putchar(ch); 
//#else 
//    rt_err_t ret = RT_EOK; 
//    if(rt_device_write(handle.device, (-1), &ch, 1) != 1)
//    {
//        ret = RT_EFULL; 
//    }
//    return ret;
//#endif /* RT_USING_POSIX */ 
//}

static rt_uint8_t lua2rtt_getc(void)
{
#if defined(RT_USING_POSIX) 
    return getchar(); 
#else 
    rt_uint8_t ch = 0; 
    while(rt_device_read(handle.device, (-1), &ch, 1) != 1)
    {
        rt_sem_take(&(handle.rx_sem), RT_WAITING_FOREVER);
    }
    return ch;
#endif /* RT_USING_POSIX */ 
}

#if !defined(RT_USING_POSIX) 
static rt_err_t lua2rtt_rxcb(rt_device_t dev, rt_size_t size)
{
    return rt_sem_release(&(handle.rx_sem)); 
}
#endif /* RT_USING_POSIX */ 

/* Lua回调函数 */ 
int lua2rtt_readline(const char *prompt, char *buffer, int buffer_size)
{
    rt_uint8_t ch; 
    
start: 
    rt_kprintf(prompt); 
    
    while(1)
    {
        ch = lua2rtt_getc(); 
        
        /* 处理方向键: 方向键为3个字节: 0x1B 0x5B 0x41/42/43/44 */ 
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
            
            if(ch == 0x41)      /* 键盘UP键 */ 
            {
                continue;
            }
            else if(ch == 0x42) /* 键盘DOWN键 */ 
            {
                continue;
            }
            else if(ch == 0x44) /* 键盘LEFT键 */ 
            {
                if(handle.line_curpos > 0)
                {
                    rt_kprintf("\b");
                    handle.line_curpos--; 
                }
                continue; 
            }
            else if(ch == 0x43) /* 键盘RIGHT键 */ 
            {
                if(handle.line_curpos < handle.line_position)
                {
                    rt_kprintf("%c", handle.line[handle.line_curpos]);
                    handle.line_curpos++;
                }
                continue;
            }
        }
        
        /* 处理空字符和错误 */ 
        if(ch == '\0' || ch == 0xFF) 
        {
            continue; 
        }
        
        /* 处理方向键删除键 */
        else if(ch == 0x7f || ch == 0x08)
        {
            /* 输入的命令字符串为空, 不对字符串进行删除处理 */ 
            if(handle.line_curpos == 0)
            {
                continue;
            }
            
            handle.line_curpos--; 
            handle.line_position--;
            
            if(handle.line_position > handle.line_curpos)
            {
                /* 删除当前光标所在处的字符 */ 
                rt_memmove(&handle.line[handle.line_curpos], &handle.line[handle.line_curpos+1], 
                    handle.line_position-handle.line_curpos); 
                handle.line[handle.line_position] = 0; 
                
                /* 重新打印移动后的字符串 */ 
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
        
        /* 处理回车键 */ 
        else if(ch == '\r' || ch == '\n')
        {
            /* 存入历史记录 */ 
            
            rt_kprintf("\n"); 
            if(handle.line_position == 0)
            {
                goto start;
            }
            else
            {
                rt_uint8_t temp = handle.line_position; 
                
                /* 拷贝输入命令 */ 
                rt_strncpy(buffer, handle.line, handle.line_position); 
                rt_memset(handle.line, 0x00, sizeof(handle.line));
                handle.line_curpos = handle.line_position = 0;
                return temp; 
            }
        }
        
        /* 处理ctrl+D退出键 */ 
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
        
        /* 处理Tab按键 */ 
        else if(ch == '\t')
        {
            continue; 
        }
        
        /* 处理其他控制字符 */ 
        else if(ch < 0x20 || ch >= 0x80)
        {
            continue;
        }
        
        /* 判断输出命令是否过长 */
        if(handle.line_position >= buffer_size)
        {
            handle.line_position = 0; 
        }
        
        /* 处理普通字符, 将普通字符添加到缓存中 */ 
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
        if(handle.line_position >= buffer_size)
        {
            handle.line_curpos = 0;
            handle.line_position = 0;
        }
    }
}

static void lua2rtt_run(void *p)
{
#if !defined(RT_USING_POSIX) 
    const char *device_name = finsh_get_device(); 
    
    handle.device = rt_device_find(device_name);
    if(handle.device == RT_NULL)
    {
        LUA2RTT_DBG("The msh device find failed.\n"); 
        return; 
    }
    
    /* 缓存msh使用的串口回调函数 */ 
    handle.rx_indicate = handle.device->rx_indicate; 
    
    /* 设置Lua2RTT的串口回调函数 */ 
    rt_device_set_rx_indicate(handle.device, lua2rtt_rxcb); 
#endif 
    
    /* 阻塞式运行lua解析器 */ 
    extern int lua_main(int argc, char **argv); 
    lua_main(handle.argc, handle.argv); 
    
    /* 释放参数分配内存 */ 
    if(handle.argc > 1)
    {
        rt_free(handle.argv[1]); 
    }
    
    /* 退出Lua2RTT解析器时恢复msh对串口的控制权 */ 
#if !defined(RT_USING_POSIX) 
    rt_device_set_rx_indicate(handle.device, handle.rx_indicate); 
#endif 
}

/* MSH Lua解析器启动命令 */ 
static int lua2rtt(int argc, char **argv) 
{
    rt_memset(&handle, 0x00, sizeof(struct lua2rtt)); 
    
    /* 初始化Lua2RTT串口数据接收信号量 */ 
    rt_sem_init(&(handle.rx_sem), "lua2rtt_rxsem", 0, RT_IPC_FLAG_FIFO); 
    
    /* 处理输出参数 */ 
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
    
    /* 创建Lua2RTT解析器线程 */ 
    rt_uint8_t prio = rt_thread_self()->current_priority+1; 
    handle.thread = rt_thread_create("lua2rtt_run", lua2rtt_run, RT_NULL, 
        LUA2RTT_THREAD_STACK_SIZE, prio, 10); 
    if(handle.thread == RT_NULL)
    {
        LUA2RTT_DBG("The Lua interpreter thread create failed.\n"); 
        return RT_ERROR; 
    }
    rt_thread_startup(handle.thread); 
    
    return RT_EOK; 
} 
MSH_CMD_EXPORT_ALIAS(lua2rtt, lua, The lua5.1.4 shell cmd.); 
