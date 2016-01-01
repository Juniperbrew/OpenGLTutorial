set(TARGET Lesson1)
# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable (${TARGET} main.cpp)

# Creates a folder "Lessons" and adds target 
# project (Lesson1.vcxproj) under it
set_property(TARGET ${TARGET} PROPERTY FOLDER "Lessons")

install (TARGETS ${TARGET} RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)

target_link_libraries(${TARGET} ${ALL_LIBS})