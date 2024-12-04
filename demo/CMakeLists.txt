file(GLOB DEMO_SOURCES "demo*")
message(STATUS "demo > DEMO_SOURCES: ${DEMO_SOURCES}")

add_executable(demo MACOSX_BUNDLE ${DEMO_SOURCES} ${FSSIMPLE_SOURCES} ${FSSIMPLE_HEADER} ${SIMPLESOUND_HEADER} ${SIMPLESOUND_SOURCES} ${BITMAP_FONT_HEADER} ${BITMAP_FONT_SOURCES})
#target_link_libraries(demo ${COCOA_LIBRARY} ${OGL_LIBRARY} ${AVFOUNDATION_FRAMEWORK})
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(X11 REQUIRED)
    find_library(ALSA_LIBRARY asound REQUIRED)

    include_directories(${X11_INCLUDE_DIR})
    list(APPEND PLATFORM_LIBRARIES ${X11_LIBRARIES} ${ALSA_LIBRARY})


    target_link_libraries(demo
        pthread
        ${PLATFORM_LIBRARIES}
        GL
        GLU
        ${OGL_LIBRARY}
    )
else()
    target_link_libraries(demo ${COCOA_LIBRARY} ${OGL_LIBRARY} ${AVFOUNDATION_FRAMEWORK})
endif()
copy_resources(demo)