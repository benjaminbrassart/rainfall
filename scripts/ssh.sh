#!/usr/bin/env bash

cd "$(dirname "$0")" || exit
source lib.sh || exit
cd .. || exit

main() {
    if (( $# < 1 )); then
        printf -- 'Usage: %s <level> [command...]\n' "${0##*/}" >&2
        return 1
    fi

    level="$1"
    shift

    sshpass_level "${level}" ssh "${level}@rainfall" "$@"
}

main "$@"
