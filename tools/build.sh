#!/bin/sh

tools_root=$(dirname $0)

# Include functions
source ${tools_root}/_functions.sh

# Include configuration
source ${tools_root}/_config.sh

# Create build directory
mkdir -p ${tools_root}/../build/{dist,info}

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

case $build_mode in
  standard | unleashed)
    firmware_path="f7-firmware-D"
    ;;

  xtreme | rogue-master)
    firmware_path="f7-firmware-C"
    ;;

  *)
    echo -n "Unknown firmware ${build_mode}"
    exit 1
    ;;
esac

cd "${tools_root}/../.${build_mode}-firmware"

api_version=$(awk -F',' 'NR == 2 {print $3}' firmware/targets/f7/api_symbols.csv);

app_suffix="${build_mode}_${api_version}"

export FBT_NO_SYNC=1

rm -rf applications_user/$APPLICATION_NAME
rm -rf build/${firmware_path}/.extapps

cp -r ../${APPLICATION_NAME}/. applications_user/${APPLICATION_NAME}

if $is_run; then
  ./fbt launch_app APPSRC=$APPLICATION_NAME
else
  ./fbt "fap_${APPLICATION_NAME}"
fi

cp "build/${firmware_path}/.extapps/${APPLICATION_NAME}.fap" "../build/dist/${APPLICATION_NAME}_${app_suffix}.fap"

# Write Information About Build
echo "${APPLICATION_NAME}_${app_suffix}.fap" >> "../build/info/${APPLICATION_NAME}_${app_suffix}.txt"
echo "${api_version}" >> "../build/info/${APPLICATION_NAME}_${app_suffix}.txt"
echo "${build_mode}" >> "../build/info/${APPLICATION_NAME}_${app_suffix}.txt"
echo "${firmware_path}" >> "../build/info/${APPLICATION_NAME}_${app_suffix}.txt"
echo "${APPLICATION_NAME}" >> "../build/info/${APPLICATION_NAME}_${app_suffix}.txt"
echo "$(md5sum "../build/dist/${APPLICATION_NAME}_${app_suffix}.fap" | cut -d ' ' -f 1)" >> "../build/info/${APPLICATION_NAME}_${app_suffix}.txt"