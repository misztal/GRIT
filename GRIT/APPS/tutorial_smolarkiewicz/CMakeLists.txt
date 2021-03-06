INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${OpenTissue_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/GRIT/include
  ${PROJECT_SOURCE_DIR}/GLUE/include
  ${PROJECT_SOURCE_DIR}/UTIL/include
  include
  )

FILE(GLOB
  TUTORIAL_SMOLARKIEWICZ_SOURCES 
  src/*.cpp
  )

FILE(GLOB
  TUTORIAL_SMOLARKIEWICZ_HEADERS
  include/*.h
  )

FILE(GLOB
  TUTORIAL_SMOLARKIEWICZ_CFGS
  ${PROJECT_SOURCE_DIR}/bin/tutorial_smolarkiewicz.cfg
  ${PROJECT_SOURCE_DIR}/bin/remeshing_params/*.cfg
  )

ADD_EXECUTABLE(
  demo_tutorial_smolarkiewicz
  ${TUTORIAL_SMOLARKIEWICZ_SOURCES} 
  ${TUTORIAL_SMOLARKIEWICZ_HEADERS}
  ${TUTORIAL_SMOLARKIEWICZ_CFGS}
  )

TARGET_LINK_LIBRARIES(
  demo_tutorial_smolarkiewicz
  glue
  util
  grit
  ${CMAKE_THREAD_LIBS_INIT}
  )

SOURCE_GROUP("Config Files"    FILES ${TUTORIAL_SMOLARKIEWICZ_CFGS})

SET_TARGET_PROPERTIES(demo_tutorial_smolarkiewicz PROPERTIES DEBUG_POSTFIX "D" )
SET_TARGET_PROPERTIES(demo_tutorial_smolarkiewicz PROPERTIES RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_SOURCE_DIR}/bin/")
SET_TARGET_PROPERTIES(demo_tutorial_smolarkiewicz PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_SOURCE_DIR}/bin/")
SET_TARGET_PROPERTIES(demo_tutorial_smolarkiewicz PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_SOURCE_DIR}/bin/")
SET_TARGET_PROPERTIES(demo_tutorial_smolarkiewicz PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_SOURCE_DIR}/bin/")
SET_TARGET_PROPERTIES(demo_tutorial_smolarkiewicz PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/bin/")
