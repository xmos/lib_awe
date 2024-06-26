set(LIB_NAME lib_awe)
set(LIB_VERSION 0.1.0)

set(LIB_INCLUDES api src src/include_internal)
set(LIB_OPTIONAL_HEADERS awe_conf.h)
set(LIB_COMPILER_FLAGS -O3 -g)
set(LIB_DEPENDENT_MODULES "lib_xud(2.3.1)")

XMOS_REGISTER_STATIC_LIB()
