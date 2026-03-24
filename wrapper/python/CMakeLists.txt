cmake_minimum_required(VERSION 3.22)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/BqCommon/CMake_utils.txt")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/BqCommon/CMake_utils.txt)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake_utils.txt")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake_utils.txt)
endif()
bq_parse_version_from_cpp("${CMAKE_CURRENT_SOURCE_DIR}/../../src/bq_log/global/version.cpp")

project(BqLogPythonWrapper VERSION ${BQ_VERSION})

set(ARTIFACTS_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../artifacts/wrapper/python")
set(INSTALL_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../install/wrapper_python")

# Copy Python source files to artifacts
add_custom_target(python_wrapper ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTIFACTS_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src" "${ARTIFACTS_OUTPUT_DIR}/src"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml" "${ARTIFACTS_OUTPUT_DIR}/pyproject.toml"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/README.md" "${ARTIFACTS_OUTPUT_DIR}/README.md"
    COMMENT "Preparing Python wrapper artifacts"
)

install(DIRECTORY "${ARTIFACTS_OUTPUT_DIR}/"
    DESTINATION "."
)
