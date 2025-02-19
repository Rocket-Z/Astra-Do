# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\vvv_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\vvv_autogen.dir\\ParseCache.txt"
  "vvv_autogen"
  )
endif()
