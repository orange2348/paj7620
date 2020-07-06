
from building import *

cwd     = GetCurrentDir()
src     = Glob('src/*.c')
path    = [cwd]

group = DefineGroup('PAJ7620', src, depend = ['PKG_USING_PAJ7620'], CPPPATH = path)

Return('group')
