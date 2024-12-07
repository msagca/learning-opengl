cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project("LearningOpenGL")
include(FindDependencies)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
if(CMAKE_GENERATOR MATCHES "Visual Studio")
  foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  endforeach()
endif()
file(GLOB CPP_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB C_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${C_SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE glad glfw glm imgui OpenGL::GL stb msagca
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
file(GLOB GLSL_SOURCES "${CMAKE_SOURCE_DIR}/src/shader/*.glsl")
target_sources(${PROJECT_NAME} PRIVATE ${GLSL_SOURCES})
file(GLOB PNG_SOURCES "${CMAKE_SOURCE_DIR}/src/image/*.png")
set(OTHER_SOURCES ${GLSL_SOURCES} ${PNG_SOURCES})
foreach(FILE ${OTHER_SOURCES})
  cmake_path(APPEND FULL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${FILE})
  list(APPEND OTHER_FILES ${FULL_PATH})
endforeach()
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OTHER_FILES} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
if(WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLFW_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  )
endif()
