import os
from building import * 

# get current dir path
cwd = GetCurrentDir()

# init src and inc vars
src = []
inc = []

# add lua2rtt common include
inc = inc + [cwd]

# add lua2rtt basic code
src = src + ['./lua2rtt.c']

# add group to IDE project
objs = DefineGroup('lua2rtt', src, depend = ['PKG_USING_LUA2RTT'], CPPPATH = inc)

# traversal subscript
list = os.listdir(cwd)
if GetDepend('PKG_USING_LUA2RTT'):
    for d in list:
        path = os.path.join(cwd, d)
        if os.path.isfile(os.path.join(path, 'SConscript')):
            objs = objs + SConscript(os.path.join(d, 'SConscript'))

Return('objs')
