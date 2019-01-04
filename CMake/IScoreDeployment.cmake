if(NOT DEPLOYMENT_BUILD)
    return()
endif()

set(CPACK_PACKAGE_NAME "ossia-score")
set(CPACK_PACKAGE_VENDOR "ossia")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An intermedia sequencer for the precise and flexible scripting of interactive scenarios.")
set(CPACK_PACKAGE_VERSION "${SCORE_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${SCORE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SCORE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SCORE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_EXTRA "${SCORE_VERSION_EXTRA}")
set(CPACK_RESOURCE_FILE_LICENSE "${SCORE_ROOT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "ossia-score")

set(CPACK_INSTALL_CMAKE_PROJECTS)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
include(IScoreDeploymentOSX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
include(IScoreDeploymentAndroid)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
if(GENERIC_LINUX_DEPLOYMENT_BUILD)
include(IScoreDeploymentGenericLinux)
else()
include(IScoreDeploymentLinux)
endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
include(IScoreDeploymentWindows)
endif()


include(CPack)
