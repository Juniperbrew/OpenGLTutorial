cmake_minimum_required (VERSION 3.4.1)
project (OpenGLTutorial)

#Probably sets value for ${OPENGL_LIBRARY}
find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

#Needed to suppress warnings from rpavlik module
cmake_policy(SET CMP0026 OLD)

#rpavlik module needed for create_default_target_launcher
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules")
include(CreateLaunchers)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

#Preprocessor tokens?
add_definitions(
#	-DTW_STATIC
#	-DTW_NO_LIB_PRAGMA
#	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
#	-D_CRT_SECURE_NO_WARNINGS
)

add_subdirectory(Lesson1)
add_subdirectory(Lesson2)
add_subdirectory(Lesson3)
add_subdirectory(Lesson4)
add_subdirectory(Lesson5)

#This somehow makes Lesson1 run after building ALL_BUILD in VS
create_default_target_launcher(
    Lesson1
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)