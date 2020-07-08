
from building import *

cwd     = GetCurrentDir()
src     = Glob('src/*.c')
path    = [cwd]
path   += [cwd + '/src']

if GetDepend('PAJ7620_USING_SAMPLES'):
    src    += Glob('examples/paj7620_samples.c')

group = DefineGroup('PAJ7620', src, depend = ['PKG_USING_PAJ7620'], CPPPATH = path)

Return('group')
