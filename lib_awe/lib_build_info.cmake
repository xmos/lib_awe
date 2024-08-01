set(LIB_NAME lib_awe)
set(LIB_VERSION 1.0.0)

set(LIB_INCLUDES api src src/include_internal)
set(LIB_OPTIONAL_HEADERS awe_conf.h)
set(LIB_COMPILER_FLAGS -O3 -g -mcmodel=large -mno-dual-issue)
set(LIB_DEPENDENT_MODULES   "lib_xud(2.3.1)"
                            "lib_logging(3.2.0)"
                            "lib_xassert(4.2.0)"
                            )

set(LIB_ARCHIVES libAWECore)

XMOS_REGISTER_STATIC_LIB()
