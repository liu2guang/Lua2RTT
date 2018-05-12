# 依赖： 
```
#define RT_USING_DFS
#define RT_USING_LIBC
#define RT_USING_POSIX
#define RT_USING_MODULE
#define RT_USING_RTC
```

# loslib.c module: os

1. os.date对!格式化暂时不支持. 因为MDK下gmtime()没有实现. 导致os.date("!c")会返回nil. 这个由RTT来解决.  
2. os.clock中的实现秒计数是采用了time.h中的CLOCKS_PER_SEC宏, 但是RTT没有将CLOCKS_PER_SEC和RT_TICK_PER_SECOND进行关联.导致单位出错, 目前是由修改os.clock中实现(将源码中的CLOCKS_PER_SEC直接修改成RT_TICK_PER_SECOND来实现的), 待RTT解决这个问题再处理. 
3. 
os.execute("ps"): 调用错误: 
(0) assertion failed at function:system, line number:302
依赖于: RT_USING_MODULE 后解决
4. lua命令行中再次执行lua会出错: 
> os.execute("lua main.lua")
> (dev != RT_NULL) assertion failed at function:rt_device_read, line number:337 

5. os.difftime底层c函数的difftime没有实现, MDK下armlibc. 
