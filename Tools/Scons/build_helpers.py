# ============================================================================
# build_helpers.py — OpenAutosar 灵活编译辅助 (供各工程 SConstruct 复用)
# ----------------------------------------------------------------------------
# 职责:
#   - 解析命令行覆盖 (variant / chip / only / skip / layers)
#   - 判断模块是否参与本次编译
#   - 定位模块源码目录 与 已释放 .a 库
# ============================================================================
import os

# 各层源码根 (源码可见时编译)
LAYER_SRC_ROOT = {
    'ASW':  '#ASW',
    'CDD':  '#CDD',
    'MCAL': '#MCAL',
}

# 各层已释放库根 (mode=lib 时链接)
LAYER_LIB_ROOT = {
    'ASW':  '#ASW_Libs',
    'BSW':  '#BSW_Libs',
    'CDD':  '#CDD_Libs',
    'MCAL': '#MCAL_Libs',
}


def _split(s):
    return set(x.strip() for x in s.split(',') if x.strip())


def parse_cli(arguments, defaults):
    """解析命令行参数, 缺省回落到 module_config 的默认值。"""
    return {
        'variant': arguments.get('variant', defaults.get('VARIANT', 'Release')),
        'chip':    arguments.get('chip',    defaults.get('CHIP', 'Tc387')),
        'only':    _split(arguments.get('only', '')),
        'skip':    _split(arguments.get('skip', '')),
        'layers':  _split(arguments.get('layers', '')),
    }


def is_selected(layer, name, cfg, opts):
    """根据 enabled 与命令行过滤判断模块是否参与本次构建。"""
    if not cfg.get('enabled', True):
        return False
    if opts['layers'] and layer not in opts['layers']:
        return False
    if opts['only'] and name not in opts['only']:
        return False
    if name in opts['skip']:
        return False
    return True


def source_dir(layer, name, chip):
    """返回模块源码目录 (用于 mode=source)。MCAL 按芯片再分一级。"""
    root = LAYER_SRC_ROOT.get(layer)
    if root is None:
        return None
    if layer == 'MCAL':
        return '%s/%s/%s' % (root, chip, name)
    return '%s/%s' % (root, name)


def lib_file(layer, name, chip, project=None):
    """返回已释放的 .a 路径 (用于 mode=lib)。
    ASW / CDD / BSW / MCAL 全部与项目相关 (MCAL 配置+静态合并为库), 各自 git 子库名
    为 <layer>_libs_<project>; 但工作区中库直接位于 <Layer>_Libs/ 下 (拉取时项目已确定,
    工作区路径不带项目子目录)。MCAL 内部仍按芯片再分一级 <Chip>/<Mod>。
    保留 project 参数仅为调用方签名兼容。"""
    root = LAYER_LIB_ROOT[layer]
    if layer == 'MCAL':
        return '%s/%s/%s/lib%s.a' % (root, chip, name, name)
    if layer == 'BSW':
        # BSW 按模块名子目录组织 (供应商/芯片/交付版本在 git 子库名中, 不再分供应商目录)
        libname = {'Os': 'Os/libRtaOs', 'Com': 'Com/libCom'}.get(name, '%s/lib%s' % (name, name))
        return '%s/%s.a' % (root, libname)
    # ASW / CDD
    return '%s/%s/lib%s.a' % (root, name, name)


def lib_incdirs(layer, name, chip, project=None):
    """返回某模块在 *_Libs 下的头文件目录 (供 CPPPATH)。"""
    root = LAYER_LIB_ROOT[layer]
    if layer == 'MCAL':
        return ['%s/inc' % root, '%s/%s/%s/inc' % (root, chip, name)]
    if layer == 'BSW':
        # 按模块名 (Os/Com/...) 取头文件目录
        return ['%s/inc' % root, '%s/%s/inc' % (root, name)]
    # ASW / CDD
    return ['%s/inc' % root, '%s/%s/inc' % (root, name)]
