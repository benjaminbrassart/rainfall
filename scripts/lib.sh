get_previous_level() {
    case "$1" in
        level0)
            ;;
        level[1-9])
            previous_level_num=$(( "${level#level}" - 1 ))
            echo "level${previous_level_num}"
            ;;
        bonus0)
            echo "level9"
            ;;
        bonus[1-3])
            previous_level_num=$(( "${level#bonus}" - 1 ))
            echo "bonus${previous_level_num}"
            ;;
        end)
            echo "bonus3"
            ;;
        *)
            printf -- 'unknown level: %s\n' "${level}" >&2
            return 1
            ;;
    esac
}

sshpass_level() {
    level="$1"
    shift

    if [[ "${level}" == "level0" ]]; then
        sshpass_input_type="-p"
        sshpass_input="level0"
    else
        previous_level="$(get_previous_level "${level}")" || return
        sshpass_input_type="-f"
        sshpass_input="${previous_level}/flag"
    fi

    sshpass "${sshpass_input_type}" "${sshpass_input}" "$@"
}
