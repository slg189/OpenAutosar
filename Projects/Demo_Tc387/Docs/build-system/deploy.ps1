# =============================================================
# OpenAutosar SCons v2 一键部署脚本（PowerShell）
#
# 用法：
#   ./deploy.ps1                  # 部署全部 + 演示模块
#   ./deploy.ps1 -NoDemo          # 不部署演示模块（已自有 BSW 时）
#   ./deploy.ps1 -DryRun          # 只显示会做什么，不实际复制
#   ./deploy.ps1 -Force           # 覆盖已存在文件
# =============================================================
param(
    [switch]$NoDemo,
    [switch]$DryRun,
    [switch]$Force
)

$ErrorActionPreference = 'Stop'

$PROTO = Split-Path -Parent $MyInvocation.MyCommand.Definition
$ROOT  = Split-Path -Parent (Split-Path -Parent $PROTO)

Write-Host "源目录: $PROTO" -ForegroundColor Cyan
Write-Host "目标根: $ROOT"  -ForegroundColor Cyan
Write-Host ""

function Copy-Item-Logged($src, $dst, [switch]$Recurse) {
    if (-not (Test-Path $src)) {
        Write-Warning "源不存在：$src（跳过）"
        return
    }
    Write-Host "  $src" -ForegroundColor DarkGray
    Write-Host "   → $dst"
    if ($DryRun) { return }
    $parent = Split-Path -Parent $dst
    if ($parent -and -not (Test-Path $parent)) {
        New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }
    $args = @{ Path = $src; Destination = $dst }
    if ($Recurse) { $args['Recurse'] = $true }
    if ($Force)   { $args['Force']   = $true }
    Copy-Item @args
}

Write-Host "① 部署顶层 SConstruct" -ForegroundColor Yellow
Copy-Item-Logged "$PROTO\SConstruct" "$ROOT\SConstruct"

Write-Host "`n② 部署 site_scons (Python 包)" -ForegroundColor Yellow
Copy-Item-Logged "$PROTO\site_scons" "$ROOT\site_scons" -Recurse

Write-Host "`n③ 部署 scripts" -ForegroundColor Yellow
Copy-Item-Logged "$PROTO\scripts" "$ROOT\scripts" -Recurse

Write-Host "`n④ 部署项目配置（Projects/DemoProject/）" -ForegroundColor Yellow
$projDst = "$ROOT\Projects\DemoProject"
if (-not (Test-Path $projDst) -and -not $DryRun) {
    New-Item -ItemType Directory -Force -Path $projDst | Out-Null
}
Copy-Item-Logged "$PROTO\projects\DemoProject\build.yaml"      "$projDst\build.yaml"
Copy-Item-Logged "$PROTO\projects\DemoProject\build.lite.yaml" "$projDst\build.lite.yaml"
Copy-Item-Logged "$PROTO\projects\DemoProject\build.qemu.yaml" "$projDst\build.qemu.yaml"
Copy-Item-Logged "$PROTO\projects\DemoProject\SConscript"      "$projDst\SConscript"

Write-Host "`n⑤ 部署 Doxygen 模板" -ForegroundColor Yellow
Copy-Item-Logged "$PROTO\doxygen\Doxyfile.in" "$ROOT\docs\Doxyfile.in"

if (-not $NoDemo) {
    Write-Host "`n⑥ 部署演示模块 BSW/Crc（用于冒烟验证）" -ForegroundColor Yellow
    Copy-Item-Logged "$PROTO\demo_bsw\Crc" "$ROOT\BSW\Crc" -Recurse
} else {
    Write-Host "`n⑥ 跳过演示模块（-NoDemo）" -ForegroundColor DarkGray
}

Write-Host ""
if ($DryRun) {
    Write-Host "[DRY-RUN] 未实际复制任何文件" -ForegroundColor Magenta
} else {
    Write-Host "✓ 部署完成" -ForegroundColor Green
    Write-Host ""
    Write-Host "下一步：" -ForegroundColor Cyan
    Write-Host "  1) pip install -r docs\v2\requirements.txt"
    Write-Host "  2) cd $ROOT"
    Write-Host "  3) scons -j8                  # 冒烟构建"
    Write-Host "  4) scons doc                  # 生成模块文档"
    Write-Host "  5) scons -c                   # 清理"
    Write-Host ""
    Write-Host "构建产物：Projects\DemoProject\Out\DemoProject_CYT4BF-A0.elf"
    Write-Host "文档产物：docs\api\index.html"
}
