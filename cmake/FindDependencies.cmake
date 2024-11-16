set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(INCLUDE_DIRS /usr/include /usr/local/include)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(LIB_DIRS /usr/lib /usr/local/lib)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
# GLFW
find_path(GLFW_INCLUDE_DIRS NAMES GLFW/glfw3.h HINTS ${INCLUDE_DIR} PATHS ${INCLUDE_DIRS})
find_library(GLFW_LIBRARY NAMES glfw3 glfw glfw3dll HINTS ${LIB_DIR} PATHS ${LIB_DIRS})
if(GLFW_INCLUDE_DIRS AND GLFW_LIBRARY)
  add_library(glfw INTERFACE)
  target_include_directories(glfw INTERFACE ${GLFW_INCLUDE_DIRS})
  target_link_libraries(glfw INTERFACE ${GLFW_LIBRARY})
endif()
set(GLFW_DLL ${BIN_DIR}/glfw3.dll)
# GLM
find_path(GLM_INCLUDE_DIRS NAMES glm/glm.hpp HINTS ${INCLUDE_DIR} PATHS ${INCLUDE_DIRS})
if(GLM_INCLUDE_DIRS)
  add_library(glm INTERFACE)
  target_include_directories(glm INTERFACE ${GLM_INCLUDE_DIRS})
endif()
# STB
find_path(STB_INCLUDE_DIRS NAMES stb_image.h HINTS ${INCLUDE_DIR} PATHS ${INCLUDE_DIRS})
if(STB_INCLUDE_DIRS)
  add_library(stb INTERFACE)
  target_include_directories(stb INTERFACE ${STB_INCLUDE_DIRS})
endif()
# GLAD
find_path(GLAD_INCLUDE_DIRS NAMES glad/glad.h HINTS ${INCLUDE_DIR} PATHS ${INCLUDE_DIRS})
if(GLAD_INCLUDE_DIRS)
  add_library(glad INTERFACE)
  target_include_directories(glad INTERFACE ${GLAD_INCLUDE_DIRS})
  target_sources(glad INTERFACE ${SRC_DIR}/glad.c)
endif()
# OpenGL
find_package(OpenGL REQUIRED)
