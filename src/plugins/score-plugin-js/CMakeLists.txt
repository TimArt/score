project(score_plugin_js LANGUAGES CXX)

if(NOT TARGET score_plugin_engine)
    return()
endif()

score_common_setup()

# Packages
find_package(${QT_VERSION} OPTIONAL_COMPONENTS Qml)
if(NOT TARGET ${QT_PREFIX}::Qml)
  return()
endif()
if(NOT TARGET score_plugin_engine)
  return()
endif()

# Files & main target
set(HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/score_plugin_js.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/EditScript.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/ScriptMacro.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/JSCommandFactory.hpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/Component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/JSAPIWrapper.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/ProcessScript.hpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Qml/QmlObjects.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Qml/ValueTypes.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Qml/Metatypes.hpp"

  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessMetadata.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessModel.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/ConsolePanel.hpp"
)
set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessModel.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessModelSerialization.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/Component.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/JSAPIWrapper.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/JSCommandFactory.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/JS/Qml/QmlObjects.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/Qml/ValueTypes.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/score_plugin_js.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
# TODO make this automatic in some way, and extend to other components
score_generate_command_list_file(${PROJECT_NAME} "${HDRS}")

target_link_libraries(${PROJECT_NAME} PUBLIC
                     ${QT_PREFIX}::Core ${QT_PREFIX}::Widgets ${QT_PREFIX}::Qml ${QT_PREFIX}::QmlPrivate
                     score_lib_base score_plugin_engine score_lib_process score_lib_inspector score_lib_device score_plugin_deviceexplorer score_plugin_protocols)

setup_score_plugin(${PROJECT_NAME})
