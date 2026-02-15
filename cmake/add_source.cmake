include_guard()

function(add_source FILENAME)
    target_sources(${MAIN_LIB_NAME} PRIVATE ${FILENAME})
endfunction()

