# :tada: Lua2RTT :tada: #

[![Build Status](https://travis-ci.org/liu2guang/Lua2RTT.svg?branch=master)](https://travis-ci.org/liu2guang/Lua2RTT)
[![release](https://img.shields.io/badge/Release-v1.0.0-orange.svg)](https://github.com/liu2guang/Lua2RTT/releases)

Lua2RTT是在RT-Thread3.0及以上版本移植的Lua库, 目的是无缝嵌入RTT, 无需开发者去移植. 如果您觉得该库看得顺眼舒服，请捐赠颗小星星. 小星星就是更新的动力!!! 

## 1. 效果图

![效果图](https://i.imgur.com/NzP2WOe.gif)

## 2. 安装Lua2RTT

目前Lua2RTT库已经添加到RT-Thread官方pkgs包中, 可以直接在menuconfig在线包中直接使能.

1. 在env中运行menuconfig. 
2. 进入RT-Thread online packages -> language目录. 
3. 开启Lua2RTT, 选择Submitted version(Lua2RTT库移植的发布版本)为lateset最新版本, 然后选择Porting Lua version(移植的Lua源码版本)为您想要移植的版本(目前支持5.1.4和5.3.4).
4. 执行pkgs --update更新Lua2RTT包到你的bsp下面. 
5. 执行scons/scons --target=xxx, 进行编译生成工程, 下载运行.

![安装流程](https://i.imgur.com/wOuODbj.gif)

## 3. 卸载Lua2RTT

1. 在env中运行menuconfig. 
2. 进入RT-Thread online packages -> language目录. 
3. 关闭Lua2RTT.
4. 执行pkgs --update, 并输入`Y`表示同意删除pkg包文件. 
5. 执行scons/scons --target=xxx, 进行编译生成工程, 下载运行.

![卸载流程](https://i.imgur.com/idFfFPN.gif)

## 4. 教程推荐

1. [AlbertS 作者简书文章](https://www.jianshu.com/u/8fad76e7e05c).
2. [Lua 5.1 参考手册中文版](https://www.codingnow.com/2000/download/lua_manual.html). 
3. [Lua 5.3 参考手册中文版](http://cloudwu.github.io/lua53doc/contents.html).  

## 5. 欢迎加入. 

非官方讨论腾讯QQ群: [289156309](). 

## 6. 感谢

1. 该库基于 https://github.com/lua/lua 移植. 
2. 感谢Lua团队. 本移植是修改了部分原作者的代码针对RTT在线包实现的版本, 该仓库保留原作者的许可声明! 具体原作者许可请查看 https://www.lua.org/license.html, 移植代码部分保留 https://github.com/liu2guang/Lua2RTT/blob/master/LICENSE 许可. 
