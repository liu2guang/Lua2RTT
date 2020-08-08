import os
import shutil
from building import * 

# get current dir path
cwd = GetCurrentDir()

# init src and inc vars
src = []
inc = []

objs = []

# add lua common include
inc = inc + [cwd]

# add lua basic code
src = src + ['./lua2rtt.c']

# add group to IDE project
if GetDepend('LUA_USING_PORTING_V514'):
    objs = DefineGroup('lua-5.1.4', src, depend = ['PKG_USING_LUA', 'LUA_USING_PORTING_V514'], CPPPATH = inc)
    #delate non-used files
    try:
        shutil.rmtree(os.path.join(cwd,'lua-5.3.4'))    
    except:
        pass
    
if GetDepend('LUA_USING_PORTING_V534'):
    objs = DefineGroup('lua-5.3.4', src, depend = ['PKG_USING_LUA', 'LUA_USING_PORTING_V534'], CPPPATH = inc)
    #delate non-used files
    try:
        shutil.rmtree(os.path.join(cwd,'lua-5.1.4'))
    except:
        pass
        
# traversal subscript
list = os.listdir(cwd)
if GetDepend('PKG_USING_LUA'):
    for d in list:
        path = os.path.join(cwd, d)
        if os.path.isfile(os.path.join(path, 'SConscript')):
            objs = objs + SConscript(os.path.join(d, 'SConscript'))

Return('objs')
