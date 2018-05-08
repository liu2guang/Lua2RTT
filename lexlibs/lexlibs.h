/*
 * @File:   lexlibs.h 
 * @Author: liu2guang
 * @Date:   2018-05-08 15:13:56
 *
 * @LICENSE: https://github.com/liu2guang/lua2rtt/blob/master/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-08     liu2guang    The first version. 
 */
#ifndef __LEXLIBS_H__ 
#define __LEXLIBS_H__ 

#define LUA_EXLIBS_ROM                                    \
    _ROM("cpu", luaopen_cpu, cpu_map) 

#endif
