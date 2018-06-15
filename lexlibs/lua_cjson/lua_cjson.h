#ifndef __LUA_CJSON_H_
#define __LUA_CJSON_H_

#include <stdio.h>
#include <stdlib.h>

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include "cJSON.h"

#define FLOAT_ENABLE
#define DBG_PRINT       rt_kprintf
#define DEFAULT_STRING  "default"

enum ADD_TO_TYPE
{
    JSON_OBJECT,
    JSON_ARRAY
};

enum BOOL
{
    FALSE = 0,
    TRUE = 1
};

#ifndef BOOL
#define BOOL    int
#endif // BOOL

typedef struct node
{
    cJSON *json_obj;
    struct node *next;
} node_t;

typedef struct list
{
    struct node *head;
    struct node *tail;
    int length;
} list_t;


// prototypes
int json_decode(lua_State *l);
int json_encode(lua_State *l);

#endif // __LUA_CJSON_H_
