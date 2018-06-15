# cJSON4Lua
This is a `JSON` library for `Lua`, using in `Embedded System`. It can easily port to various platform. This is part of `LuaNode`, and based on cJSON wrote by Dave Gamble

----------------------------

# How To Use

Follow the steps bellow to use cJSON4Lua.

* Include the following files, cJSON.h cjson.c handler.h and handler.c, to your project.

* Include the header: handler.h to your source.

* Register `json_encode` and `json_decode` for Lua.

* Call `json_encode/json_decode` to encode/decode data respectively.

Easy to use, right? check the example we provide for details.

----------------------------

# Encode & Decode

Before utilizing cJSON4Lua, you have to register encode/decode function for Lua first.

```c
/**
 * The second argument is the function name used in Lua, you
 * can change it to other name you like, but you'll never
 * change the third name. It is the C function name.
 **/
lua_register(L, "json_encode", json_encode);
lua_register(L, "json_decode", json_decode);
```

In Lua script, you can encode data as follow,

```lua
data = {12, 3.14, false, "hello", {SubObject="test"}};
json = json_encode(data);
```

Decode data as follow,

```lua
decode_result = json_decode(json);
```

The decoded data is then saved in `decode_result`, a Lua table.
