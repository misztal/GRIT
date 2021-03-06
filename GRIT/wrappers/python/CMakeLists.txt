INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${OpenTissue_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/GRIT/include
  ${PROJECT_SOURCE_DIR}/GLUE/include
  ${PROJECT_SOURCE_DIR}/UTIL/include
  include
  )

ADD_SUBDIRECTORY(pybind11)

PYBIND11_ADD_MODULE(pygrit SHARED python.cpp)

TARGET_LINK_LIBRARIES(
  pygrit
  PRIVATE
  glue
  util
  grit
  ${CMAKE_THREAD_LIBS_INIT}
  )

SET_TARGET_PROPERTIES(pygrit PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")
SET_TARGET_PROPERTIES(pygrit PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin/")
SET_TARGET_PROPERTIES(pygrit PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin/")

INSTALL( 
         TARGETS pygrit EXPORT pygritTargets
         LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}/bin"
         ARCHIVE DESTINATION "${PROJECT_SOURCE_DIR}/bin"
       )
