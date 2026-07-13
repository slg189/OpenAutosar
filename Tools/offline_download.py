#!/usr/bin/env python3
"""Download redistributable offline assets into Tools/Offline.

This script prepares the files needed to recreate the project Python
environment without internet access. It also delegates CMake/GoogleTest
downloads to Tools/fetch_tools.py, which stores them under Tools/.toolcache.

Default target: Windows x64 + CPython 3.11, matching the project's tested
baseline. Vendor licensed tools such as HighTec/TASKING/EB/Vector/ETAS are
not downloaded here.
"""
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import urllib.request
import zipfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
OFFLINE = os.path.join(TOOLS, 'Offline')
PYTHON_VERSION = '3.11.9'
PYTHON_PLATFORM = 'windows-x86_64'
PYTHON_INSTALLER = f'python-{PYTHON_VERSION}-amd64.exe'
PYTHON_URL = f'https://www.python.org/ftp/python/{PYTHON_VERSION}/{PYTHON_INSTALLER}'
WHEELHOUSE = os.path.join(OFFLINE, 'wheelhouse', 'windows-x86_64-cp311')
SOFTWARE = os.path.join(OFFLINE, 'software', 'windows-x86_64')
INSTALLERS = os.path.join(OFFLINE, 'installers', 'windows-x86_64')
TOOLS_DIR = os.path.join(OFFLINE, 'tools', 'windows-x86_64')
MANIFEST = os.path.join(OFFLINE, 'manifest.json')
W64DEVKIT_REPO = 'skeeto/w64devkit'
DOXYGEN_REPO = 'doxygen/doxygen'
CPPCHECK_REPO = 'danmar/cppcheck'
# 固定版本 (与项目 pinned 依赖同一原则; 重新打包不漂移)。--latest 可临时改抓最新。
PINNED_TAGS = {
    W64DEVKIT_REPO: 'v2.8.0',
    DOXYGEN_REPO: 'Release_1_17_0',
    CPPCHECK_REPO: '2.21.0',
}
USE_LATEST = False


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def download(url, dest, force=False):
    os.makedirs(os.path.dirname(dest), exist_ok=True)
    if os.path.exists(dest) and not force:
        print(f'[skip] {os.path.relpath(dest, TOOLS)}')
        return dest
    print(f'[fetch] {url}')
    with urllib.request.urlopen(url, timeout=180) as r, open(dest, 'wb') as f:
        while True:
            chunk = r.read(1024 * 1024)
            if not chunk:
                break
            f.write(chunk)
    print(f'[ok] {os.path.relpath(dest, TOOLS)}')
    return dest


def latest_asset(repo, predicate):
    """按 PINNED_TAGS 固定版本取 release 资产; --latest 时回退抓最新。"""
    tag = None if USE_LATEST else PINNED_TAGS.get(repo)
    url = (f'https://api.github.com/repos/{repo}/releases/tags/{tag}' if tag
           else f'https://api.github.com/repos/{repo}/releases/latest')
    data = json.load(urllib.request.urlopen(url, timeout=60))
    for asset in data.get('assets', []):
        name = asset.get('name', '')
        if predicate(name):
            return data.get('tag_name'), name, asset.get('browser_download_url')
    raise SystemExit(f'[fatal] no matching release asset found for {repo}'
                     + (f' tag {tag}' if tag else ''))


def find_file(root, name):
    if not os.path.isdir(root):
        return None
    lname = name.lower()
    for dirpath, _, files in os.walk(root):
        for f in files:
            if f.lower() == lname:
                return os.path.join(dirpath, f)
    return None


def extract_w64devkit(force=False):
    tag, name, url = latest_asset(W64DEVKIT_REPO, lambda n: n.startswith('w64devkit-x64-') and n.endswith('.7z.exe'))
    arc = download(url, os.path.join(INSTALLERS, name), force)
    target = os.path.join(TOOLS_DIR, 'w64devkit')
    if os.path.exists(os.path.join(target, 'bin', 'gcc.exe')) and not force:
        print(f'[skip] w64devkit {tag} -> {os.path.relpath(target, TOOLS)}')
        return target, tag
    if os.path.isdir(target):
        shutil.rmtree(target)
    os.makedirs(TOOLS_DIR, exist_ok=True)
    print(f'[extract] {os.path.relpath(arc, TOOLS)}')
    subprocess.check_call([arc, '-y', f'-o{TOOLS_DIR}'])
    extracted = os.path.join(TOOLS_DIR, os.path.splitext(os.path.splitext(name)[0])[0])
    if os.path.isdir(extracted) and extracted != target:
        os.replace(extracted, target)
    print(f'[ok] w64devkit {tag} -> {os.path.relpath(target, TOOLS)}')
    return target, tag


def extract_doxygen(force=False):
    tag, name, url = latest_asset(DOXYGEN_REPO, lambda n: n.endswith('.windows.x64.bin.zip'))
    arc = download(url, os.path.join(INSTALLERS, name), force)
    target = os.path.join(TOOLS_DIR, 'doxygen')
    if find_file(target, 'doxygen.exe') and not force:
        print(f'[skip] doxygen {tag} -> {os.path.relpath(target, TOOLS)}')
        return target, tag
    if os.path.isdir(target):
        shutil.rmtree(target)
    os.makedirs(target, exist_ok=True)
    print(f'[extract] {os.path.relpath(arc, TOOLS)}')
    with zipfile.ZipFile(arc) as z:
        z.extractall(target)
    print(f'[ok] doxygen {tag} -> {os.path.relpath(target, TOOLS)}')
    return target, tag


def extract_cppcheck(force=False):
    tag, name, url = latest_asset(CPPCHECK_REPO, lambda n: n.endswith('-x64-Setup.msi'))
    msi = download(url, os.path.join(INSTALLERS, name), force)
    target = os.path.join(TOOLS_DIR, 'cppcheck')
    if find_file(target, 'cppcheck.exe') and not force:
        print(f'[skip] cppcheck {tag} -> {os.path.relpath(target, TOOLS)}')
        return target, tag
    if os.path.isdir(target):
        shutil.rmtree(target)
    os.makedirs(target, exist_ok=True)
    print(f'[extract] {os.path.relpath(msi, TOOLS)}')
    subprocess.check_call(['msiexec', '/a', msi, f'TARGETDIR={target}', '/qn'])
    print(f'[ok] cppcheck {tag} -> {os.path.relpath(target, TOOLS)}')
    return target, tag


def pip_download(target, force=False, current=False):
    os.makedirs(target, exist_ok=True)
    marker = os.path.join(target, '.complete')
    if os.path.exists(marker) and not force:
        print(f'[skip] wheelhouse {os.path.relpath(target, TOOLS)}')
        return
    cmd = [sys.executable, '-m', 'pip', 'download', '--dest', target, '--only-binary=:all:']
    if not current:
        cmd += [
            '--platform', 'win_amd64',
            '--implementation', 'cp',
            '--python-version', '3.11',
            '--abi', 'cp311',
        ]
    cmd += ['-r', os.path.join(TOOLS, 'requirements.txt'), 'pip']
    print('[pip] ' + ' '.join(cmd))
    subprocess.check_call(cmd)
    with open(marker, 'w', encoding='utf-8') as f:
        f.write('ok\n')


def write_manifest(python_installer, bundled_tools=None):
    bundled_tools = bundled_tools or []
    files = []
    for root, _, names in os.walk(OFFLINE):
        for name in names:
            path = os.path.join(root, name)
            if os.path.abspath(path) == os.path.abspath(MANIFEST):
                continue
            files.append({
                'path': os.path.relpath(path, TOOLS).replace('\\', '/'),
                'sha256': sha256(path),
                'bytes': os.path.getsize(path),
            })
    data = {
        'python': {
            'version': PYTHON_VERSION,
            'platform': PYTHON_PLATFORM,
            'installer': os.path.relpath(python_installer, TOOLS).replace('\\', '/'),
        },
        'python_packages': {
            'requirements': 'requirements.txt',
            'wheelhouse': os.path.relpath(os.path.dirname(WHEELHOUSE), TOOLS).replace('\\', '/'),
            'packages': ['scons==4.10.1', 'PyYAML==6.0.1'],
        },
        'redistributable_tools': [
            {'name': 'CMake', 'version': '3.28.6', 'location': '.toolcache/cmake-*'},
            {'name': 'GoogleTest', 'version': '1.14.0', 'location': '.toolcache/googletest-*'},
            *bundled_tools,
        ],
        'not_bundled': [
            'HighTec/TASKING TriCore toolchains: vendor licensed',
            'EB tresos / Vector / ETAS tools: vendor licensed',
            'qemu-system-tricore: usually self-built for TriCore support',
            'OpenSSH/rsync: system tools or optional remote-build tools',
        ],
        'files': sorted(files, key=lambda x: x['path']),
    }
    os.makedirs(OFFLINE, exist_ok=True)
    with open(MANIFEST, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
        f.write('\n')
    print(f'[ok] {os.path.relpath(MANIFEST, TOOLS)}')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--force', action='store_true', help='redownload existing assets')
    ap.add_argument('--skip-toolcache', action='store_true', help='do not run Tools/fetch_tools.py')
    ap.add_argument('--skip-extra-tools', action='store_true', help='do not download GCC/Cppcheck/Doxygen bundles')
    ap.add_argument('--latest', action='store_true',
                    help='ignore PINNED_TAGS and fetch latest releases (version may drift)')
    ap.add_argument('--include-current', action='store_true',
                    help='also try to download wheels for the currently running Python')
    a = ap.parse_args()
    if a.latest:
        global USE_LATEST
        USE_LATEST = True

    python_installer = download(PYTHON_URL, os.path.join(SOFTWARE, PYTHON_INSTALLER), a.force)
    pip_download(WHEELHOUSE, a.force)
    if a.include_current:
        current_tag = f'{PYTHON_PLATFORM}-cp{sys.version_info.major}{sys.version_info.minor}'
        current_wheelhouse = os.path.join(OFFLINE, 'wheelhouse', current_tag)
        if current_wheelhouse != WHEELHOUSE:
            try:
                pip_download(current_wheelhouse, a.force, current=True)
            except subprocess.CalledProcessError as exc:
                print(f'[warn] current Python wheelhouse failed: {exc}')
    if not a.skip_toolcache:
        subprocess.check_call([sys.executable, os.path.join(TOOLS, 'fetch_tools.py')])
    bundled_tools = []
    if not a.skip_extra_tools:
        for name, fn in (('w64devkit', extract_w64devkit), ('doxygen', extract_doxygen), ('cppcheck', extract_cppcheck)):
            try:
                path, version = fn(a.force)
                bundled_tools.append({
                    'name': name,
                    'version': version,
                    'location': os.path.relpath(path, TOOLS).replace('\\', '/'),
                })
            except Exception as exc:
                print(f'[warn] failed to bundle {name}: {exc}')
    write_manifest(python_installer, bundled_tools)
    print('\n[done] Offline assets prepared under Tools/Offline.')
    print('       On an offline machine, install Python 3.11 if needed, then run:')
    print('       python Tools/setup_env.py --offline --no-fetch')
    return 0


if __name__ == '__main__':
    sys.exit(main())
