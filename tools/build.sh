
#!/bin/bash

mkdir dist

# Fetch all firmwares submodules
git submodule update --init --recursive

# Set default build mode
build_mode="standard"

# Use getopts to parse command-line options and assign their values to variables
while getopts ":f:" opt; do
  case $opt in
    f)
      build_mode=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

# Shift the remaining arguments to the left, so that $1, $2, etc. are the non-option arguments
shift $((OPTIND - 1))

cd .$build_mode-firmware

api_version=$(awk -F',' 'NR == 2 {print $3}' firmware/targets/f7/api_symbols.csv);

app_suffix="${build_mode}_${api_version}"

echo "$concatenated_string"  # Outputs "hello world"

export FBT_NO_SYNC=1
cp -r ../flipp_pomodoro/. applications_user/flipp_pomodoro

./fbt fap_flipp_pomodoro

cp build/f7-firmware-D/.extapps/flipp_pomodoro.fap ../dist/flipp_pomodoro_$app_suffix.fap
