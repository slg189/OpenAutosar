# ============================================================================
# module_config.py — 模块化「灵活配置 / 灵活编译」声明
# ----------------------------------------------------------------------------
# 本文件由 SConstruct 导入。每个模块可独立声明:
#   mode    : 'source' 从源码编译 (需源码可见) | 'lib' 使用已释放的 .a
#   enabled : 是否参与本次构建 (可被命令行 only=/skip=/layers= 覆盖)
#   cflags  : 模块级附加编译选项 (追加在全局选项之后, 实现按模块灵活编译)
#   defines : 模块级宏定义
#
# 命令行可覆盖 (见 SConstruct):
#   scons variant=Debug                # Debug | Release
#   scons chip=Tc367                   # 目标芯片
#   scons only=Adc,CddPwm              # 仅编译这些模块
#   scons skip=EnergyManagement        # 跳过这些模块
#   scons layers=ASW,MCAL              # 仅编译这些层
# ============================================================================

# ---- 工程级默认 ----
PROJECT = 'Demo_Tc387'
CHIP    = 'Tc387'          # Tc387 | Tc367
VARIANT = 'Release'        # Debug | Release

# ---- 全局编译选项 (按变体) ----
GLOBAL_CFLAGS = {
    'Debug':   ['-O0', '-g3', '-DDEBUG'],
    'Release': ['-O2', '-g',  '-DNDEBUG'],
}
GLOBAL_DEFINES = ['AUTOSAR_CP']

# ---- 各层 / 各模块配置 ----
# 信息安全说明:
#   - ASW 工程: ASW 模块 mode=source(可见), 其余层 mode=lib
#   - 基础工程: 仅 Integration 源码可见, 所有层 mode=lib
#   - 完整工程(本文件): 可按需将任意模块切回 source 进行联调
MODULES = {
    'MCAL': {
        'Adc':  {'mode': 'source', 'enabled': True, 'cflags': ['-O3']},
        'Can':  {'mode': 'lib',    'enabled': True},
        'Port': {'mode': 'source', 'enabled': True},
    },
    'CDD': {
        'CddPwm':   {'mode': 'lib', 'enabled': True},
        'CddMotor': {'mode': 'lib', 'enabled': True},
        'CddSent':  {'mode': 'lib', 'enabled': True},
    },
    'ASW': {
        'VehicleControl':   {'mode': 'source', 'enabled': True},
        'EnergyManagement': {'mode': 'lib',    'enabled': True},
    },
    'BSW': {
        'Os':  {'mode': 'lib', 'enabled': True},   # ETAS RTA-OS
        'Com': {'mode': 'lib', 'enabled': True},   # Vector COM
    },
}
