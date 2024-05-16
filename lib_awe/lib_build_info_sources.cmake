# This files exists as XCCM does not support mixed source and archive libs yet
# Needs to be included after APP_ROOT_DIR APP_C_SRCS APP_ASM_SRCS anbd APP_INCLUDES set in app cmake

# Function to replace a substring in each entry of a list
function(replace_substring_in_list list result substring_to_replace replacement_substring)
    set(temp_list)
    foreach(item ${${list}})
        string(REPLACE ${substring_to_replace} "${replacement_substring}" modified_item ${item})
        list(APPEND temp_list ${modified_item})
    endforeach()
    set(${result} ${temp_list} PARENT_SCOPE)
endfunction()


file(GLOB_RECURSE APP_C_SRCS ${APP_ROOT_DIR}/src/*.c)
file(GLOB_RECURSE APP_ASM_SRCS ${APP_ROOT_DIR}/src/*.S)

replace_substring_in_list(APP_C_SRCS APP_C_SRCS ${APP_ROOT_DIR}/ "")
replace_substring_in_list(APP_ASM_SRCS APP_ASM_SRCS ${APP_ROOT_DIR}/ "")

file(GLOB LIB_AWE_C_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.c)
file(GLOB LIB_AWE_ASM_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.S)
set(LIB_AWE_INCLUDES    ${CMAKE_CURRENT_LIST_DIR}/api
                        ${CMAKE_CURRENT_LIST_DIR}/src/include_internal)

replace_substring_in_list(LIB_AWE_C_SRCS LIB_AWE_C_SRCS ${CMAKE_CURRENT_LIST_DIR}/ "../lib_awe/")
replace_substring_in_list(LIB_AWE_ASM_SRCS LIB_AWE_ASM_SRCS ${CMAKE_CURRENT_LIST_DIR}/ "../lib_awe/")
# replace_substring_in_list(LIB_AWE_INCLUDES LIB_AWE_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ "../lib_awe/")

list(APPEND APP_C_SRCS ${LIB_AWE_C_SRCS})
list(APPEND APP_ASM_SRCS ${LIB_AWE_ASM_SRCS})
list(APPEND APP_INCLUDES ${LIB_AWE_INCLUDES})
list(APPEND APP_COMPILER_FLAGS -D__awe_conf_h_exists__=1)















