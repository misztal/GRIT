FIND_PATH(
  CUSP_INCLUDE_DIRS  cusp/blas.h
  ${CUSP_DIR}
  DOC "What is the path where the cusp/blas.h file can be found"
  )

IF(CUSP_INCLUDE_DIRS)
  SET( CUSP_FOUND 1 CACHE STRING "Set to 1 if Cusp is found, 0 otherwise")
ELSE()
  SET( CUSP_FOUND 0 CACHE STRING "Set to 1 if Cusp is found, 0 otherwise")
ENDIF()

MARK_AS_ADVANCED( CUSP_FOUND           )
MARK_AS_ADVANCED( CUSP_INCLUDE_DIRS    )
