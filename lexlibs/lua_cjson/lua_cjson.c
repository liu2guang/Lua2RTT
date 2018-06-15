#include "lua_cjson.h"

struct list json_obj_list;

/*
 * Support function: strchr (for embedded system)
 */
static char *my_strchr(char *s, char c)
{
    while (*s != '\0' && *s != c)
    {
        s++;
    }
    return (*s == c ? s : NULL);
}

static unsigned int my_strlen(char *s)
{
    unsigned int count = 0;
    while (*s != '\0')
    {
        s++;
        count++;
    }
    return count;
}

/*
 * List init
 */
static void list_init(list_t *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

/*
 * Create node for list
 */
static struct node *create_node(cJSON *new_data)
{
    struct node *n = malloc(sizeof(struct node));
    if (n == NULL)
    {
        DBG_PRINT("Node create failed\n");
    }
    n->json_obj = new_data;
    n->next = NULL;
    return n;
}

/*
 * Check list is empty
 */
static int list_is_empty(list_t *list)
{
    return (list->head == NULL);
}

/*
 * Add new node to list tail
 */
static void list_add(list_t *list, cJSON *new_data)
{
    struct node *n = create_node(new_data);
    n->next = NULL;

    if (list_is_empty(list) == TRUE)
    {
        list->head = n;         // an empty list, add this node directly
        list->tail = n;
    }
    else
    {
        list->tail->next = n;   // add to tail
        list->tail = n;
    }

    list->length ++;
}

/*
 * Remove all nodes in a list
 */
static void list_destroy(list_t *list)
{
    struct node *n;    // remove from head
    while (list_is_empty(list) != FALSE)
    {
        n = list->head;
        list->head = n->next;
        cJSON_Delete(n->json_obj);
        n->json_obj = NULL;
        n->next = NULL;
        free(n);
        n = NULL;
        list->length --;
    }
    list->tail = NULL;
}

static cJSON *get_data_from_table(lua_State *l, cJSON *pJsonObj, cJSON *pJsonArray);
static void json_parser(lua_State *l, cJSON *pRoot);

static int get_num_length(int num)
{
    int count = 0;
    while (num >= 1)
    {
        num = num / 10;
        count ++;
    }
    return count;
}

/*
 * Check whether a json object is empty
 */
static BOOL is_cjson_empty(cJSON *pRoot)
{
    if (pRoot->child != NULL)
    {
        return FALSE;
    }
    return TRUE;
}

/*
 * Parse data
 */
static int get_value(lua_State *l, const char *key, cJSON *pJsonObj, cJSON *pJsonArray, int key_is_number, int *add_to_arry)
{
    switch (lua_type(l, -1))
    {
    case LUA_TSTRING:
    {
        *add_to_arry = TRUE;
        if (key_is_number == TRUE)
        {
            cJSON_AddStringToObject(pJsonArray, key, lua_tostring(l, -1));
            return JSON_ARRAY;
        }
        cJSON_AddStringToObject(pJsonObj, key, lua_tostring(l, -1));
    }
    break;
    case LUA_TNUMBER:
    {
        *add_to_arry = TRUE;
        if (key_is_number == TRUE)
        {
            cJSON_AddNumberToObject(pJsonArray, key, lua_tonumber(l, -1));
            return JSON_ARRAY;
        }
        cJSON_AddNumberToObject(pJsonObj, key, lua_tonumber(l, -1));
    }
    break;
    case LUA_TBOOLEAN:
    {
        *add_to_arry = TRUE;
        if (key_is_number == TRUE)
        {
            cJSON_AddBoolToObject(pJsonArray, key, lua_toboolean(l, -1));
            return JSON_ARRAY;
        }
        cJSON_AddBoolToObject(pJsonObj, key, lua_toboolean(l, -1));
    }
    break;
    case LUA_TFUNCTION:
        // function is not supported yet
        break;
    case LUA_TTABLE:
    {
        cJSON *pSubJsonObj = cJSON_CreateObject(); // get key-value recursively
        list_add(&json_obj_list, pSubJsonObj);
        cJSON *pSubJsonArray = cJSON_CreateArray();
        list_add(&json_obj_list, pSubJsonArray);
        cJSON *pRes = get_data_from_table(l, pSubJsonObj, pSubJsonArray);
        if (*add_to_arry == TRUE)
        {
            cJSON_AddItemToObject(pJsonArray, key, pRes);
            return JSON_ARRAY;
        }
        cJSON_AddItemToObject(pJsonObj, key, pRes);
    }
    break;
    default:
        break;
    }

    return JSON_OBJECT;
}

/*
 * Get key-value from table
 */
static cJSON *get_data_from_table(lua_State *l, cJSON *pJsonObj, cJSON *pJsonArray)
{
    int t = lua_gettop(l);
    lua_pushnil(l);
    int ret;
    int add_to_array = FALSE;
    while (lua_next(l, t) != 0)
    {
        // get key
        if (lua_isnumber(l, -2))
        {
            char buff[32];
            int key = lua_tonumber(l, -2);
            sprintf(buff, "%d", key);
            ret = get_value(l, buff, pJsonObj, pJsonArray, TRUE, &add_to_array);
        }
        else if (lua_isstring(l, -2))
        {
            ret = get_value(l, lua_tostring(l, -2), pJsonObj, pJsonArray, FALSE, &add_to_array);
        }

        lua_pop(l, 1);
    }

    if (!is_cjson_empty(pJsonArray) && !is_cjson_empty(pJsonObj))
    {
        cJSON_AddItemToObject(pJsonArray, DEFAULT_STRING, pJsonObj);
        return pJsonArray;
    }

    if (ret == JSON_ARRAY)
    {
        return pJsonArray;
    }

    return pJsonObj;
}

/*
 * Whether a number is float
 */
static BOOL is_float(float num)
{
    BOOL res = FALSE;
    char buff[32];
    sprintf(buff, "%f", num);
    char *pChar = my_strchr(buff, '.');
    while (pChar != &buff[my_strlen(buff)])
    {
        if (*pChar > '0' && *pChar <= '9')
        {
            return TRUE;
        }
        pChar++;
    }
    return FALSE;
}

/*
 * parser json string recursively if needed
 */
static void json_parser(lua_State *l, cJSON *pRoot)
{
    lua_newtable(l);    // create a new table, on top of stack

    int level_size = cJSON_GetArraySize(pRoot);

    for (int i = 0; i < level_size; i++)
    {
        cJSON *pSub = cJSON_GetArrayItem(pRoot, i);
        if (pSub == NULL)
        {
            return;
        }

        switch (pSub->type)
        {
        case cJSON_False:
        case cJSON_True:
        {
            // push key
            if (pSub->string == NULL)
            {
                lua_pushinteger(l, i + 1);
            }
            else
            {
                lua_pushstring(l, pSub->string);
            }
            // push value
            if (pSub->type == cJSON_False)
            {
                lua_pushboolean(l, 0);
            }
            else
            {
                lua_pushboolean(l, 1);
            }
            lua_settable(l, -3);
        }
        break;
        case cJSON_NULL:
            // do nothing
            break;
        case cJSON_Number:
        {
            // push key
            if (pSub->string == NULL)
            {
                lua_pushinteger(l, i + 1);
            }
            else
            {
                lua_pushstring(l, pSub->string);
            }

            // push value
#ifdef FLOAT_ENABLE
            if (is_float(pSub->valuedouble))
            {
                lua_pushnumber(l, pSub->valuedouble);
            }
            else
            {
                lua_pushinteger(l, pSub->valueint);
            }
#else
            lua_pushinteger(l, pSub->valueint);
#endif // FLOAT_ENABLE
            lua_settable(l, -3);
        }
        break;
        case cJSON_String:
        {
            // push key
            if (pSub->string == NULL)
            {
                lua_pushinteger(l, i + 1);
            }
            else
            {
                lua_pushstring(l, pSub->string);
            }
            // push value
            lua_pushstring(l, pSub->valuestring);
            lua_settable(l, -3);
        }
        break;
        case cJSON_Array:
        case cJSON_Object:
        {
            // recursive parser
            if (pSub->string == NULL)
            {
                lua_pushinteger(l, i + 1);
            }
            else
            {
                lua_pushstring(l, pSub->string);
            }
            json_parser(l, pSub);
            lua_settable(l, -3);
        }
        break;
        default:
            break;
        }

    }
}

/*
 * Decode json string
 */
int cjson_decode(lua_State *l)
{
    if (!lua_isstring(l, -1))
    {
        DBG_PRINT("The input is not a string\n");
        return 0;
    }

    const char *pJson = lua_tostring(l, -1);
    cJSON *pRoot = cJSON_Parse(pJson);
    json_parser(l, pRoot);

    return 1;
}

/*
 * Encode table into json string
 */
int cjson_encode(lua_State *l)
{
    list_init(&json_obj_list);
    cJSON *pJsonObj = cJSON_CreateObject();
    list_add(&json_obj_list, pJsonObj);
    cJSON *pJsonArray = cJSON_CreateArray();
    list_add(&json_obj_list, pJsonArray);

    if (pJsonObj == NULL || pJsonArray == NULL)
    {
        DBG_PRINT("cJSON create failed\n");
        return 0;
    }

    cJSON *pRes = get_data_from_table(l, pJsonObj, pJsonArray);

    char *json = cJSON_PrintUnformatted(pRes);
    lua_pushlstring(l, json, my_strlen(json));

    list_destroy(&json_obj_list);

    return 1;
}

// Module function maps
#define MIN_OPT_LEVEL 2 
#include "lrodefs.h" 
static const LUA_REG_TYPE cjson_map[] =
{
    {LSTRKEY("encode"), LFUNCVAL(cjson_encode)}, 
    {LSTRKEY("decode"), LFUNCVAL(cjson_decode)}, 
    {LNILKEY, LNILKEY} 
}; 

LUALIB_API int luaopen_cjson(lua_State *L)
{
    LREGISTER(L, "cjson", cjson_map); 
} 
