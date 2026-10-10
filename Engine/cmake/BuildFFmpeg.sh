#!/usr/bin/env bash
set -euo pipefail

# Native CMake launches bash without the usual MSYS2 login environment.
export PATH="/usr/bin:$PATH"
action=$1
shift
to_shell_path() {
    if command -v cygpath >/dev/null 2>&1; then
        cygpath -u "$1"
    else
        printf '%s\n' "$1"
    fi
}

source_dir=$(to_shell_path "$1")
build_dir=$(to_shell_path "$2")
install_dir=$(to_shell_path "$3")
compiler=$(to_shell_path "$4")
archiver=$(to_shell_path "$5")
ranlib=$(to_shell_path "$6")
strip=$(to_shell_path "$7")
make_program=$(to_shell_path "$8")
jobs=$9
nasm=${10:-}
export PATH="$(dirname "$compiler"):/usr/bin:$PATH"
# Do not inherit the native MinGW Makefiles jobserver in MSYS GNU Make.
unset MAKEFLAGS MFLAGS

# ExternalProject creates this directory before invoking the script.
cd "$build_dir"
case "$action" in
    configure)
        options=(
            "--prefix=$install_dir"
            "--cc=$compiler" "--ar=$archiver" "--ranlib=$ranlib" "--strip=$strip"
            --enable-shared --disable-static --enable-pic
            --disable-programs --disable-doc --disable-debug --disable-autodetect
        )
        case "$($compiler -dumpmachine)" in
            *mingw*) options+=(--target-os=mingw32) ;;
        esac
        if [[ -n "$nasm" ]]; then
            options+=("--x86asmexe=$(to_shell_path "$nasm")")
        else
            options+=(--disable-x86asm)
        fi
        bash "$source_dir/configure" "${options[@]}"
        ;;
    build) "$make_program" -r -j"$jobs" ;;
    install) "$make_program" -r install ;;
    *) printf 'Unknown FFmpeg build action: %s\n' "$action" >&2; exit 1 ;;
esac
