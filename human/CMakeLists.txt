file(GLOB HUMAN_SOURCES "Human*")
message(STATUS "human > HUMAN_SOURCES: ${HUMAN_SOURCES}")

add_executable(human-test MACOSX_BUNDLE test.cpp ${HUMAN_SOURCES} ${FSSIMPLE_SOURCES} ${FSSIMPLE_HEADER} )
target_link_libraries(human-test ${COCOA_LIBRARY} ${OGL_LIBRARY})