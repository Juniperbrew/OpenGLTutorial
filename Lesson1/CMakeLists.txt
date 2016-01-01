
# Set Properties->General->Configuration Type to Application(.exe)
# Creates app.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable (Lesson1 main.cpp)

# Creates a folder "Lessons" and adds target 
# project (Lesson1.vcxproj) under it
set_property(TARGET Lesson1 PROPERTY FOLDER "Lessons")

install (TARGETS Lesson1 RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)

target_link_libraries(Lesson1
	${ALL_LIBS}
)