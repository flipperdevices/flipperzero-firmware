#!/bin/sh

if test $# -eq 0; then
    echo "user_settings_asm.sh requires one argument specifying compiler flags to pull include directories from." 1>&2
    exit 1
fi

# Compress multiple spaces to single spaces, then replace instances of
# "-I " with "-I" (i.e. remove spaces between -I and the include path).
search_string=$(echo "$1" | sed -e 's/  */ /g' -e 's/-I /-I/g')

for token in $search_string
do
    case "$token" in
    -I*)
        # Trim off the leading "-I".
        path="${token#-I}"
        # Trim off the trailing "/".
        path="${path%/}"
        if test -e "$path/user_settings.h"; then
            user_settings_dir="$path"
            user_settings_path="$path/user_settings.h"
            break
        fi
        ;;
    *)
        ;;
    esac
done

# Fall back to user_settings.h in the current directory.
if test -z "${user_settings_path-}"; then
    if test -e "user_settings.h"; then
        user_settings_dir="."
        user_settings_path="user_settings.h"
    fi
fi

if test -z "${user_settings_path-}"; then
    echo "Unable to find user_settings.h." 1>&2
    exit 1
else
    # Strip out anything from user_settings.h that isn't a preprocessor
    # directive (i.e. any lines not starting with #). Put the result in
    # user_settings_asm.h in the same directory as user_settings.h.
    # user_settings_asm.h is safe to include in assembly files (e.g. .S
    # files).
    sed -e '/^ *#/!d' -e :a -e '$!N;s/\\\n/ /;ta' -e 'P;D' < "$user_settings_path" > "$user_settings_dir/user_settings_asm.h"
fi
