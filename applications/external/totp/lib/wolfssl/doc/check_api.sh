#!/bin/sh

ls ./dox_comments/header_files/ |
while read h_file; do
    grep -h --null -o 'WOLFSSL_API(\n|\s|[^;])*;' ./dox_comments/header_files/$h_file |
    tr '\n' ' ' |
    sed 's/\\n//g' |
    sed 's/ \+/ /g' |
    sed 's/\x00/\n/g' > dox_api.txt

    find ../ -not -path '../doc/*' -name $h_file |
    while read -r h_file_path; do
        echo "Checking: $h_file_path"
        grep -h --null -o 'WOLFSSL_API(\n|\s|[^;])*;' "$h_file_path" |
        sed 's/#.*/ /g' |
        tr '\n' ' ' |
        sed 's/\\n//g' |
        sed 's/ \+/ /g' |
        sed 's/\x00/\n/g' > wolf_api.txt

        api_count="$(wc -l < dox_api.txt)"
        match_count="$(grep -Ff dox_api.txt wolf_api.txt | wc -l)"
        if [ "$api_count" != "$match_count" ]; then
            echo "Mismatch"
            echo "Dox_api: $api_count"
            echo "Matched_api: $match_count"
            echo "Header file: $h_file"
            echo "Check API: "
            sort dox_api.txt -o dox_api.txt
            sort wolf_api.txt -o wolf_api.txt
            comm -23 dox_api.txt wolf_api.txt
            exit 1
        else
            echo "$h_file is all good"
            break
        fi
    done || exit 1
    echo 'Next...\n'

done || exit 1

rm dox_api.txt
rm wolf_api.txt
