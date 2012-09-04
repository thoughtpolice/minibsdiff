cmake_minimum_required (VERSION 2.8.9)
project (minibsdiff C)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build Type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release RelWithDebInfo MinSizeRel
  )
endif ()

include(CheckIncludeFile)

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(stdint.h HAVE_STDINT_H)

if (MSVC)
  set(OPTIONS "/W4 /WL")
  if (MSVC11)
    set(OPTIONS "${OPTIONS} /analyze")
  endif ()
else ()
  set(WARNING_LIST
    -Wall -W -Wmissing-prototypes -Wstrict-prototypes -Wconversion
    -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align
    -Wwrite-strings -Wnested-externs -Wextra)
  foreach (_warning ${WARNING_LIST})
    set(WARNINGS "${WARNINGS} ${_warning}")
  endforeach ()
  set(RELEASE_OPTIONS "-O3 -funroll-loops")
  set(DEBUG_OPTIONS "-g -O0")
  set(OPTIONS "${WARNINGS} -ansi -pedantic -std=c99")
endif ()

set(CMAKE_C_FLAGS_RELEASE "${RELEASE_OPTIONS}")
set(CMAKE_C_FLAGS_DEBUG "${DEBUG_OPTIONS}")
set(CMAKE_C_FLAGS "${OPTIONS}")

add_executable(minibsdiff minibsdiff.c)

install(TARGETS minibsdiff
  RUNTIME DESTINATION bin
)
