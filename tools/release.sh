#!/bin/sh

tools_root=$(dirname $0)

# Include functions
source ${tools_root}/_functions.sh

# Include configuration
source ${tools_root}/_config.sh

cp ${tools_root}/release_template.md ${tools_root}/release.md

# Add New Lines
echo "" >> ${tools_root}/release.md
echo "" >> ${tools_root}/release.md

# Count File Hashes
echo "# File Hashes" >> ${tools_root}/release.md

# Generate table
echo "| File | Firmware name | Firmware version | md5 hash |" >> ${tools_root}/release.md
echo "|------|---------------|------------------|----------|" >> ${tools_root}/release.md

for file in "$tools_root/../build/info/"*
do
    if [ -f "$file" ]; then
      app_name=$(sed -n '1p' "$file")
      app_hash=$(sed -n '6p' "$file")
      firmware_version=$(sed -n '2p' "$file")
      firmware_name=$(sed -n '3p' "$file")

      echo "| ${app_name} | $(firmware_link_by_name $firmware_name) | ${firmware_version} | ${app_hash} |" >> ${tools_root}/release.md
    fi
done

# Replace App Name
sed -i -e "s|#APP_NAME#|$APPLICATION_NAME|g" $tools_root/release.md