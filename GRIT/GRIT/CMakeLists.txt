INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${OpenTisue_INCLUDE_DIRS} 
  ${PROJECT_SOURCE_DIR}/UTIL/include
  include
  )

FILE(GLOB
  GRIT_SOURCES
  src/*.cpp
  )

FILE(GLOB
  GRIT_HEADERS
  include/*.h
  include/attribute_assignment/*.h
  include/quality_measures/*.h
  include/mesh_operations/*.h
  include/monitors/*.h
  include/io/*.h
  include/utilities/*.h
  )

ADD_LIBRARY(
  grit
  ${GRIT_HEADERS}
  ${GRIT_SOURCES}
  )

TARGET_LINK_LIBRARIES(
  grit
  util
  )

FILE(GLOB
  ATTRIBUTE_ASSIGNMENT_HEADERS
  include/attribute_assignment/*.h
  )

SOURCE_GROUP("Header Files\\Attribute Assignments"    FILES ${ATTRIBUTE_ASSIGNMENT_HEADERS})

FILE(GLOB
  QUALITY_MEASURES_HEADERS
  include/quality_measures/*.h
  )

SOURCE_GROUP("Header Files\\Quality Measures"    FILES ${QUALITY_MEASURES_HEADERS})

FILE(GLOB
  MESH_OPERATIONS_HEADERS
  include/mesh_operations/*.h
  )

SOURCE_GROUP("Header Files\\Mesh Operations"    FILES ${MESH_OPERATIONS_HEADERS})

FILE(GLOB
  MONITORS_HEADERS
  include/monitors/*.h
#  include/io/*.h
#  include/utilities/*.h
  )

SOURCE_GROUP("Header Files\\Monitors"    FILES ${MONITORS_HEADERS})

FILE(GLOB
  IO_HEADERS
  include/io/*.h
#  include/utilities/*.h
  )

SOURCE_GROUP("Header Files\\IO"    FILES ${IO_HEADERS})

FILE(GLOB
  UTILITIES_HEADERS
  include/utilities/*.h
  )

SOURCE_GROUP("Header Files\\Utilities"    FILES ${UTILITIES_HEADERS})

SET_TARGET_PROPERTIES(grit PROPERTIES DEBUG_POSTFIX "D" )

SET_TARGET_PROPERTIES(grit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_SOURCE_DIR}/lib/")
SET_TARGET_PROPERTIES(grit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_SOURCE_DIR}/lib/")
SET_TARGET_PROPERTIES(grit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_SOURCE_DIR}/lib/")
SET_TARGET_PROPERTIES(grit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_SOURCE_DIR}/lib/")
SET_TARGET_PROPERTIES(grit PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/lib/")

