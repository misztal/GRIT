#--------------------------------------------------------------------------------

SET(3RDPARTY_LIBRARIES      "")
SET(3RDPARTY_INCLUDE_DIRS   "")
SET(3RDPARTY_DEF            "")

#--------------------------------------------------------------------------------

FIND_PACKAGE(OpenTissue PATHS "${3RDPARTY_DIR}/OpenTissue" REQUIRED)
IF( OpenTissue_FOUND )
  SET(3RDPARTY_INCLUDE_DIRS ${3RDPARTY_INCLUDE_DIRS} ${OpenTissue_INCLUDE_DIRS} )
ELSE()
  MESSAGE("Could not find OpenTissue try adding a path entry named OpenTissue_ROOT giving the path to your OpenTissue installation")
ENDIF()

#--------------------------------------------------------------------------------

FIND_PACKAGE(CUSP PATHS "${3RDPARTY_DIR}/cusplibrary-master" REQUIRED)
IF( CUSP_FOUND )
  SET(3RDPARTY_INCLUDE_DIRS ${3RDPARTY_INCLUDE_DIRS} ${CUSP_INCLUDE_DIRS} )
ELSE()
  MESSAGE("Could not find CUSP try adding a path entry named CUSP_ROOT giving the path to your Cusp installation")
ENDIF()

#--------------------------------------------------------------------------------

IF(3RDPARTY_INCLUDE_DIRS)
  SET( 3RDPARTY_FOUND 1 CACHE STRING "Set to 1 if 3RDPARTY is found, 0 otherwise")
ELSE()
  SET( 3RDPARTY_FOUND 0 CACHE STRING "Set to 1 if 3RDPARTY is found, 0 otherwise")
ENDIF()

MARK_AS_ADVANCED( 3RDPARTY_DEF             )
MARK_AS_ADVANCED( 3RDPARTY_FOUND           )
MARK_AS_ADVANCED( 3RDPARTY_LIBRARIES       )
MARK_AS_ADVANCED( 3RDPARTY_INCLUDE_DIRS    )

#--------------------------------------------------------------------------------
