# Creates C resources file from files in given directory recursively
function(create_resources dir output)
    # Create empty output file
    file(WRITE ${output} "#include <stdint.h>\n\n")
    # Collect input files
    file(GLOB bin_paths ${dir}/ESP*/*)
    # Iterate through input files
    foreach(bin ${bin_paths})
        # Get short filenames, by discarding relative path
        file(GLOB name RELATIVE ${dir} ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "[\\./-]" "_" filename ${name})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${output} "const uint8_t  ${filename}[] = {${filedata}};\nconst uint32_t ${filename}_size = sizeof(${filename});\n")
    endforeach()
endfunction()