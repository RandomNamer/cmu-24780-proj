message(STATUS "Running human component configs")

add_executable(human-test MACOSX_BUNDLE test.cpp ${FSSIMPLE_SOURCES} ${FSSIMPLE_HEADER} )
target_link_libraries(human-test ${COCOA_LIBRARY} ${OGL_LIBRARY})