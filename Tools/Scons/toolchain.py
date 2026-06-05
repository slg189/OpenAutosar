"""TriCore 工具链定义 - 供各工程 SConstruct 复用。"""
def tricore_environment(SConsEnvironment, mcu='tc387'):
    env = SConsEnvironment(
        CC='tricore-gcc', AR='tricore-ar',
        CCFLAGS=['-mcpu=%s' % mcu, '-Wall', '-O2', '-g'])
    return env
