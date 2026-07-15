#!/usr/bin/env python3
"""Migrate the legacy TC397 HighTec project into the OpenAutosar v3 layout.

The legacy tree keeps supplier source, generated configuration, and project
integration in one source hierarchy.  This tool makes the separation required
by Projects/<P>/Docs: static modules use <Layer>/<Module>/inc + src, tool input
lives under Projects/Tc397_Hightec/Cfg, and generated code lives exclusively
under Projects/Tc397_Hightec/Gen.
"""
from __future__ import annotations

import argparse
import shutil
from collections import defaultdict
from pathlib import Path


PROJECT = "Tc397_Hightec"
CODE_EXTENSIONS = {".c", ".h", ".S", ".s", ".asm"}
SOURCE_EXTENSIONS = {".c", ".S", ".s", ".asm"}
HEADER_EXTENSIONS = {".h", ".hpp", ".inc"}
IGNORE_NAMES = {"SConscript", "__pycache__", ".git", ".settings"}


def copy_file(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, destination)


def copy_tree(source: Path, destination: Path) -> None:
    if not source.is_dir():
        return
    shutil.copytree(source, destination, dirs_exist_ok=True,
                    ignore=shutil.ignore_patterns("__pycache__", ".git"))


def module_names(root: Path) -> list[str]:
    if not root.is_dir():
        return []
    return sorted(item.name for item in root.iterdir()
                  if item.is_dir() and not item.name.startswith("_") and item.name not in IGNORE_NAMES)


def copy_module_source(source: Path, destination: Path, generated: Path | None = None) -> bool:
    """Copy legacy api/inc and src trees; root-level C/H files are generated code."""
    copied_source = False
    for header_dir in ("api", "inc", "include", "Include"):
        copy_tree(source / header_dir, destination / "inc")
    for source_dir in ("src", "Src"):
        if (source / source_dir).is_dir():
            copy_tree(source / source_dir, destination / "src")
            copied_source = True

    for item in source.iterdir():
        if item.name in IGNORE_NAMES or item.is_dir() or item.suffix not in CODE_EXTENSIONS:
            continue
        leaf = "src" if item.suffix in SOURCE_EXTENSIONS else "inc"
        # BSW module-root C/H files are generator output in the legacy tree;
        # ASW/MCAL module-root C/H files are static source and stay in-layer.
        copy_file(item, (generated or destination) / leaf / item.name)
        copied_source = copied_source or item.suffix in SOURCE_EXTENSIONS
    return copied_source


def matching_module(filename: str, names: list[str]) -> str:
    """Associate generated files with the longest matching static module name."""
    stem = Path(filename).stem.lower()
    for name in sorted(names, key=len, reverse=True):
        if stem == name.lower() or stem.startswith(name.lower() + "_"):
            return name
    # MCAL module directories often use generic names while generated names
    # retain the vendor derivative, for example Mcu -> Mcu_17_TimerIp_Cfg.h.
    head = stem.split("_", 1)[0]
    for name in names:
        if name.lower() == head:
            return name
    return "Common"


def copy_mcal_generated(source: Path, generated_root: Path, modules: list[str]) -> set[str]:
    seen: set[str] = set()
    for kind in ("inc", "src"):
        folder = source / kind
        if not folder.is_dir():
            continue
        for item in folder.rglob("*"):
            if not item.is_file() or item.suffix not in CODE_EXTENSIONS:
                continue
            module = matching_module(item.name, modules)
            destination = generated_root / module / ("src" if item.suffix in SOURCE_EXTENSIONS else "inc") / item.name
            copy_file(item, destination)
            seen.add(module)
    return seen


def copy_integration(source: Path, destination: Path, domain: str) -> None:
    if not source.is_dir():
        return
    for item in source.rglob("*"):
        if not item.is_file() or item.name in IGNORE_NAMES or item.suffix not in CODE_EXTENSIONS:
            continue
        relative_parent = item.parent.relative_to(source)
        if item.suffix in HEADER_EXTENSIONS:
            target = destination / "inc" / domain / relative_parent / item.name
        elif item.suffix in SOURCE_EXTENSIONS:
            target = destination / "src" / domain / relative_parent / item.name
        else:
            continue
        copy_file(item, target)


def yaml_module(name: str, generated_layer: str | None = None) -> list[str]:
    lines = [f"    {name}:", "      mode: source"]
    if generated_layer:
        base = "${PROJECT_DIR}/Gen/" + generated_layer + "/" + name
        lines.extend([
            "      generated_source_dirs:", f"        - {base}/src",
            "      generated_include_dirs:", f"        - {base}/inc",
        ])
    return lines


def write_build_yaml(project_dir: Path, mcal: list[str], cdd: list[str], asw: list[str], bsw: list[str],
                     mcal_generated: set[str], bsw_generated: set[str]) -> None:
    lines = [
        'schema_version: "3.0"',
        "",
        "project:",
        "  name: Tc397_Hightec",
        "  derivative: Tc397",
        "",
        "target:",
        "  toolchain: hightec",
        "  platform: AURIX2G",
        "  build_type: Release",
        "",
        "defaults:",
        "  visibility: source",
        "  chip: Tc397",
        "",
        "global_defines:",
        "  - AUTOSAR_CP",
        "  - PROJECT_Tc397_Hightec",
        "",
        "global_includes:",
        "  - ${MCAL_DIR}/inc",
        "  - ${PROJECT_DIR}/Gen",
        "",
        "layers:",
        "  MCAL:",
    ]
    for name in sorted(set(mcal) | mcal_generated):
        lines.extend(yaml_module(name, "MCAL" if name in mcal_generated else None))
    lines.append("  CDD:")
    for name in cdd:
        lines.extend(yaml_module(name))
    lines.append("  ASW:")
    for name in asw:
        lines.extend(yaml_module(name))
    lines.append("  BSW:")
    for name in sorted(set(bsw) | bsw_generated | {"Os"}):
        lines.extend(yaml_module(name, "BSW" if name in bsw_generated or name == "Os" else None))
    lines.extend([
        "  Components: {}",
        "",
        "integration:",
        "  enabled: true",
        "  dir: Integration",
        "",
        "linker:",
        "  script: ''",
        "  libraries: []",
        "  extra_flags: []",
        "",
        "output:",
        "  formats: [elf, hex, map]",
        "  size_report: true",
        "  compile_commands_json: true",
        "",
        "codegen:",
        "  enabled: true",
        "  output_dir: ${PROJECT_DIR}/Gen",
        "  generators:",
        "    - name: etas_bsw",
        "      type: etas",
        "      enabled: true",
        "      tool_path: C:/ETAS/RTA-CAR_12.1.0/ISOLAR-AB_12.1.2/ISOLAR-B.cmd",
        "      workspace: ${PROJECT_DIR}/Cfg/BSW/workspace",
        "      project: ${PROJECT_DIR}/Cfg/BSW/Isolar_Project",
        "      target: RTA-BSW",
        "      output_dir: ${PROJECT_DIR}/Gen/BSW",
        "      workspace_option: -data",
        "      generate_option: -generate",
        "      project_option: -p",
        "      project_arg_style: joined",
        "      target_option: -t",
        "      target_arg_style: joined",
        "      output_option: -o",
        "      options: []",
        "",
        "docs:",
        "  enabled: true",
        "  output_dir: ${PROJECT_DIR}/Docs/api",
        "  doxygen:",
        "    enabled: false",
        "    config_file: ${ROOT_DIR}/Tools/Doxygen/Doxyfile.in",
        "  interface_extract: { enabled: true, output_format: markdown }",
        "  dependency_graph: { enabled: true, format: mermaid }",
        "  module_index: { enabled: true, title: Tc397_Hightec module index }",
        "",
        "remote:",
        "  enabled: false",
    ])
    (project_dir / "build.yaml").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", required=True, type=Path, help="legacy AUTOSAR_Project_On_TC397_Hightec root")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2], help="OpenAutosar root")
    args = parser.parse_args()
    source = args.source.resolve()
    root = args.root.resolve()
    legacy = source / "Sourcecode"
    project = root / "Projects" / PROJECT

    for name in ("Cfg/MCAL", "Cfg/CDD", "Cfg/BSW/workspace", "Gen/MCAL", "Gen/CDD", "Gen/BSW",
                 "Integration/inc", "Integration/src", "Ld", "Libs", "Obj", "Out", "Reports", "Test", "Docs"):
        (project / name).mkdir(parents=True, exist_ok=True)
    copy_file(root / "Projects" / "Demo_Tc387" / "SConscript", project / "SConscript")

    # Configuration-tool inputs only: never place these files in Gen/.
    copy_tree(source / "Mcal_Project", project / "Cfg" / "MCAL" / "Mcal_Project")
    copy_tree(source / "Os_Project", project / "Cfg" / "BSW" / "Os_Project")
    copy_tree(source / "Isolar_Project", project / "Cfg" / "BSW" / "Isolar_Project")
    copy_tree(legacy / "linkfile", project / "Ld")

    # Static ASW/CDD modules.
    asw_root = legacy / "asw"
    asw_modules, cdd_modules = [], []
    for name in module_names(asw_root):
        layer = "CDD" if name.upper().startswith("CDD_") else "ASW"
        if copy_module_source(asw_root / name, root / layer / name):
            (cdd_modules if layer == "CDD" else asw_modules).append(name)

    # Static MCAL and its generator output split by module.
    mcal_root = legacy / "mcal" / "MCAL_Modules"
    mcal_modules = module_names(mcal_root)
    for name in mcal_modules:
        copy_module_source(mcal_root / name, root / "MCAL" / "Tc397" / name)
    mcal_generated = copy_mcal_generated(legacy / "mcal" / "MCAL_Gen", project / "Gen" / "MCAL", mcal_modules)

    # BSW static source and the configuration code previously stored in each module root.
    bsw_root = legacy / "bsw"
    bsw_modules: set[str] = set()
    bsw_generated: set[str] = set()
    # Legacy BSW supplies a small set of layer-wide headers next to module
    # directories (for example CodeGen_Version.h).  The v3 convention keeps
    # those in BSW/inc so every BSW module can include them.
    for item in bsw_root.iterdir():
        if item.is_file() and item.suffix in HEADER_EXTENSIONS:
            copy_file(item, root / "BSW" / "inc" / item.name)
    for name in module_names(bsw_root):
        source_dir = bsw_root / name
        base = name.split("_PreCompile", 1)[0]
        generated = project / "Gen" / "BSW" / base
        has_static = copy_module_source(source_dir, root / "BSW" / base, generated)
        if has_static:
            bsw_modules.add(base)
        if generated.exists() and any(generated.rglob("*.c")):
            bsw_generated.add(base)

    # ETAS RTA-OS output is generated BSW code, not supplier static source.
    os_generated = legacy / "os" / "Gen"
    for item in os_generated.rglob("*") if os_generated.is_dir() else []:
        if not item.is_file() or item.suffix not in CODE_EXTENSIONS:
            continue
        kind = "src" if item.suffix in SOURCE_EXTENSIONS else "inc"
        copy_file(item, project / "Gen" / "BSW" / "Os" / kind / item.name)
    bsw_generated.add("Os")

    # The former integration and RTE trees are project glue code.
    for domain in ("ASW", "BSW", "ECU", "MCAL", "OS", "RTE", "TargetIntegration"):
        copy_integration(legacy / "Integration" / domain, project / "Integration", domain)
    copy_integration(legacy / "rte", project / "Integration", "RTE_Generated")

    write_build_yaml(project, mcal_modules, cdd_modules, asw_modules, sorted(bsw_modules),
                     mcal_generated, bsw_generated)
    print(f"[migrate] source : {source}")
    print(f"[migrate] project: {project}")
    print(f"[migrate] modules: MCAL={len(mcal_modules)} CDD={len(cdd_modules)} ASW={len(asw_modules)} BSW={len(bsw_modules | bsw_generated)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
