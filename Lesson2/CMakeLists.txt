set(TARGET Lesson2)
# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable (${TARGET} 
main.cpp 	
../common/shader.cpp
../common/shader.hpp
SimpleFragmentShader.fragmentshader
SimpleVertexShader.vertexshader)

# Creates a folder "Lessons" and adds target 
# project (Lesson1.vcxproj) under it
set_property(TARGET ${TARGET} PROPERTY FOLDER "Lessons")

install (TARGETS ${TARGET} RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)

target_link_libraries(${TARGET} ${ALL_LIBS})

# Xcode and Visual working directories, not sure if this does anything?
set_target_properties(${TARGET} PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
create_target_launcher(${TARGET} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

#Possible to get this in main makefile?
SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )