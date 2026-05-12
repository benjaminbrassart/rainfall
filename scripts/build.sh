#!/usr/bin/env bash

cd "$(dirname "$0")" || exit
source lib.sh || exit
cd .. || exit

build_level() {
    level="$1"

    make_dir="${level}/resources"
    make -C "${make_dir}" || return

    sshpass_level "${level}" scp "${make_dir}/payload.bin" "${level}@rainfall:/tmp/payload-${level}.bin" || return
}

main() {
    if (( $# != 1 )); then
        printf -- 'Usage: %s <level>\n' "${0##*/}" >&2
        return 1
    fi

    command -V ssh scp sshpass make >/dev/null || return

    build_level "$1" || return
}

main "$@"
