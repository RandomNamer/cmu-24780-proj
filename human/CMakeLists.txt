file(GLOB HUMAN_SOURCES "Human*")
message(STATUS "human > HUMAN_SOURCES: ${HUMAN_SOURCES}")

add_executable(human-test MACOSX_BUNDLE test.cpp ${HUMAN_SOURCES} ${FSSIMPLE_SOURCES} ${FSSIMPLE_HEADER} RenderContext.h)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(X11 REQUIRED)
    find_library(ALSA_LIBRARY asound REQUIRED)

    include_directories(${X11_INCLUDE_DIR})
    list(APPEND PLATFORM_LIBRARIES ${X11_LIBRARIES} ${ALSA_LIBRARY})

    target_link_libraries(human-test
        pthread
        ${PLATFORM_LIBRARIES}
        GL
        GLU
        ${OGL_LIBRARY}
    )
else()
    target_link_libraries(human-test ${COCOA_LIBRARY} ${OGL_LIBRARY})
endif()