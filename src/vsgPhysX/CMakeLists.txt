set(PARENT_SCOPE_SOURCES "${SOURCES}")

set(SOURCES
    Actor.cpp
    ActorBuilder.cpp
    Engine.cpp
    PhysicsTransform.cpp
    Scene.cpp
    SimulateOperation.cpp
)

list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

list(APPEND PARENT_SCOPE_SOURCES ${SOURCES})
set(SOURCES ${PARENT_SCOPE_SOURCES} PARENT_SCOPE)
