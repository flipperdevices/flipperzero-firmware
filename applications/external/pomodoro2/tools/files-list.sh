#!/bin/bash

# Check if directory is passed as argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 directory"
    exit 1
fi

# Root directory to start the search
root_dir="$1"

# Extensions to look for
extensions=("c" "h")

# Function to process files
process_file() {
  file="$1"
  echo "# ${file}"
  echo ""
  cat "${file}"
  echo ""
  echo ""
}

export -f process_file

# Loop over extensions
for ext in "${extensions[@]}"; do
  find "$root_dir" -name "*.${ext}" -exec bash -c 'process_file "$0"' {} \;
done
