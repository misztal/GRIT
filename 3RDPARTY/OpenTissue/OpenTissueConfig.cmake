FIND_PATH(
  OpenTissue_INCLUDE_DIRS  OpenTissue/configuration.h
  ${OpenTissue_DIR}
  DOC "What is the path where the configuration.h file can be found"
  )

IF(OpenTissue_INCLUDE_DIRS)
  SET( OpenTissue_FOUND 1 CACHE STRING "Set to 1 if OpenTissue is found, 0 otherwise")
ELSE()
  SET( OpenTissue_FOUND 0 CACHE STRING "Set to 1 if OpenTissue is found, 0 otherwise")
ENDIF()

MARK_AS_ADVANCED( OpenTissue_FOUND           )
MARK_AS_ADVANCED( OpenTissue_INCLUDE_DIRS    )
