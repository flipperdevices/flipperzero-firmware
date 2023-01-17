#!/bin/sh

application_name="flipp_pomodoro"
repo_root=$(dirname $0)/..

mkdir -p ${repo_root}/dist

# Fetch all firmwares submodules
git submodule update --init --recursive

# Set default build mode
build_mode="standard"
is_run=false

# Use getopts to parse command-line options and assign their values to variables
while getopts "f:i" opt; do
  case $opt in
    f)
      build_mode=$OPTARG
      ;;
    i)
      is_run=true
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

cd "${repo_root}/.${build_mode}-firmware"

api_version=$(awk -F',' 'NR == 2 {print $3}' firmware/targets/f7/api_symbols.csv);

app_suffix="${build_mode}_${api_version}"

export FBT_NO_SYNC=1

rm -rf applications_user/$application_name
rm -rf build/f7-firmware-D/.extapps

cp -r ../$application_name/. applications_user/$application_name

if $is_run; then
  ./fbt launch_app APPSRC=$application_name  
else
  ./fbt "fap_${application_name}"
fi

cp "build/f7-firmware-D/.extapps/${application_name}.fap" "../dist/${application_name}_${app_suffix}.fap"
