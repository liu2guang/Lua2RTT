# :tada: Lua2rtt :tada: #

[![Build Status](https://travis-ci.org/liu2guang/lua2rtt.svg?branch=master)](https://travis-ci.org/liu2guang/lua2rtt)
[![release](https://img.shields.io/badge/Release-v0.0.1-orange.svg)](https://github.com/liu2guang/LittlevGL2RTT/releases)

lua2rtt 是在RTThread3.0版本移植的elua5.1.4版本, 目的是无缝嵌入RTT, 无需开发者去移植. 如果您觉得该库看得顺眼舒服，请捐赠颗小星星. 小星星就是更新的动力!!! 

## 1. 安装lua2rtt

目前安装lua2rtt库未添加到rtt官方pkgs包中, 需要手动下载menuconfig相关文件. 

1. 下载menuconfig所需文件: [下载地址](https://github.com/liu2guang/mypackages/tree/master/lua2rtt "下载地址").
2. 将下载的`Kconfig`和`package.json`文件放到`env\packages\packages\language\lua2rtt\`目录下. 
3. 打开`env\packages\packages\language\Kconfig\`，在文件中添加`source "$PKGS_DIR/packages/language/lua2rtt/Kconfig"`.
4. 在env中运行menuconfig. 
5. 进入RT-Thread online packages -> language packages目录. 
6. 开启lua2rtt, 选择version为lateset最新版本.
7. 执行pkgs --update更新lua2rtt包到你的bsp下面. 
8. 执行scons/scons --target=xxx, 进行编译生成工程, 下载运行.

## 2. 卸载lua2rtt

1. 在env中运行menuconfig. 
2. 进入RT-Thread online packages -> language packages目录. 
3. 关闭lua2rtt.
4. 执行pkgs --update, 并输入`Y`表示同意删除pkg包文件. 
5. 执行scons/scons --target=xxx, 进行编译生成工程, 下载运行.

## 3. 开发情况

1. 开发环境: RT1050-Fire + MDK5.22.0 + ENV0.7. 

## 4. 教程推荐

1. AlbertS 作者简书文章: [点击跳转](https://www.jianshu.com/u/8fad76e7e05c).
2. Lua 5.1 参考手册中文版: [点击跳转](https://www.codingnow.com/2000/download/lua_manual.html). 
3. Lua 5.3 参考手册中文版: [点击跳转](http://cloudwu.github.io/lua53doc/contents.html). 
  