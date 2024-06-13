set(LIB_NAME lib_awe)
set(LIB_VERSION 0.1.0)

set(LIB_ARCH xs3a)
set(LIB_INCLUDES api)

set(LIB_OPTIONAL_HEADERS awe_conf.h)

set(LIB_ADD_SRC_DIRS src)
set(LIB_ADD_INC_DIRS inc src src/include_internal)

XMOS_REGISTER_STATIC_LIB()
