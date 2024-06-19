# This files exists as XCCM does not support mixed source and archive libs yet
# Needs to be included after APP_ROOT_DIR is set in host file

# Function to replace a substring in each entry of a list
function(replace_substring_in_list list result substring_to_replace replacement_substring)
    set(temp_list)
    foreach(item ${${list}})
        string(REPLACE ${substring_to_replace} "${replacement_substring}" modified_item ${item})
        list(APPEND temp_list ${modified_item})
    endforeach()
    set(${result} ${temp_list} PARENT_SCOPE)
endfunction()

# Grab the app sources
file(GLOB_RECURSE APP_C_SRCS ${APP_ROOT_DIR}/src/*.c)
file(GLOB_RECURSE APP_ASM_SRCS ${APP_ROOT_DIR}/src/*.S)

# Modify the path to relative
replace_substring_in_list(APP_C_SRCS APP_C_SRCS ${APP_ROOT_DIR}/ "")
replace_substring_in_list(APP_ASM_SRCS APP_ASM_SRCS ${APP_ROOT_DIR}/ "")

# Grab the lib sources
file(GLOB LIB_AWE_C_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
file(GLOB LIB_AWE_ASM_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.S)
set(LIB_AWE_INCLUDES    ${CMAKE_CURRENT_LIST_DIR}/api
                        ${CMAKE_CURRENT_LIST_DIR}/src
                        ${CMAKE_CURRENT_LIST_DIR}/src/include_internal)

# Get relative sandbox dir from app
replace_substring_in_list(XMOS_SANDBOX_DIR RELATIVE_XMOS_SANDBOX_DIR ${APP_ROOT_DIR}/ "")

# modify path of lib sources relative to app
replace_substring_in_list(LIB_AWE_C_SRCS LIB_AWE_C_SRCS ${CMAKE_CURRENT_LIST_DIR} ${RELATIVE_XMOS_SANDBOX_DIR}/lib_awe/lib_awe)
replace_substring_in_list(LIB_AWE_ASM_SRCS LIB_AWE_ASM_SRCS ${CMAKE_CURRENT_LIST_DIR} ${RELATIVE_XMOS_SANDBOX_DIR}/lib_awe/lib_awe)


# Add them all together for XCCM
list(APPEND APP_C_SRCS ${LIB_AWE_C_SRCS})
list(APPEND APP_ASM_SRCS ${LIB_AWE_ASM_SRCS})
list(APPEND APP_INCLUDES ${LIB_AWE_INCLUDES})



# Here pending optional_includes support for lib
list(APPEND APP_COMPILER_FLAGS "-D __awe_conf_h_exists__=1")













