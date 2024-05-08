add_executable(vsgPhysXHelloWorld helloworld.cpp)
target_link_libraries(vsgPhysXHelloWorld vsgPhysX)

if (WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    add_custom_command(TARGET vsgPhysXHelloWorld POST_BUILD
        COMMAND cmake -E copy_if_different $<TARGET_RUNTIME_DLLS:vsgPhysXHelloWorld> $<TARGET_FILE_DIR:vsgPhysXHelloWorld>
        COMMAND_EXPAND_LISTS
    )
endif()

install(TARGETS vsgPhysXHelloWorld)
