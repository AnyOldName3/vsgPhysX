set(PARENT_SCOPE_HEADERS "${HEADERS}")

set(HEADERS
    Actor.h
    ActorBuilder.h
    Convert.h
    Engine.h
    Export.h
    PhysicsTransform.h
    unique_ptr.h
    ref_ptr.h
    Scene.h
    SimulateOperation.h
)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

list(APPEND PARENT_SCOPE_HEADERS ${HEADERS})
set(HEADERS ${PARENT_SCOPE_HEADERS} PARENT_SCOPE)
