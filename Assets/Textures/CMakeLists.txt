file(GLOB_RECURSE TEXTURE_FILES CONFIGURE_DEPENDS "*.png")
foreach(FILE ${TEXTURE_FILES})
    # Get the file name without directory
    get_filename_component(FILE_NAME ${FILE} NAME)
    # Copy each file to the build directory
    configure_file(${FILE} ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME} COPYONLY)
endforeach()