#!/usr/bin/env bash

build_level() {
    level="$1"

    case "${level}" in
        level0)
            sshpass_input_type='-p'
            sshpass_input='level0'
            ;;

        level[1-9])
            level_num="${level#level}"
            previous_level_num=$(( level_num - 1 ))

            sshpass_input_type='-f'
            sshpass_input="level${previous_level_num}/flag"
            ;;

        bonus0)
            sshpass_input_type='-f'
            sshpass_input="level9/flag"
            ;;

        bonus[1-4])
            level_num="${level#bonus}"
            previous_level_num=$(( level_num - 1 ))

            sshpass_input_type='-f'
            sshpass_input="bonus${previous_level_num}/flag"
            ;;

        *)
            printf -- 'Unknown level: %s\n' "${level}" >&2
            return 1
            ;;
    esac

    make_dir="${level}/resources"
    make -C "${make_dir}" || return

    sshpass "${sshpass_input_type}" "${sshpass_input}" scp "${make_dir}/payload.bin" "${level}@rainfall:/tmp/payload-${level}.bin" || return
}

main() {
    if (( $# == 0 )); then
        printf -- 'Usage: %s <level...>\n' "${0##*/}" >&2
        return 1
    fi

    command -V ssh scp sshpass make >/dev/null || return

    for level in "$@"; do
        build_level "${level}" || return
    done
}

main "$@"
