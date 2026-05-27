set(MoveTest_Files
    MoveTests/MoveTests.cpp
    MoveTests/MoveListTests.cpp
    MoveTests/MoveIntentTests.cpp
    MoveTests/MoveGenerationTests.cpp
    MoveTests/MoveValidationTests.cpp
    MoveTests/MoveExecutionTests.cpp
    MoveTests/CastlingTests.cpp
    MoveTests/EnPassantTests.cpp
    MoveTests/MoveHistoryTests.cpp
    MoveTests/MoveNotationTests.cpp
)

set(PlayerTest_Files
    PlayerTests/PlayerTests.cpp
    PlayerTests/CPUPlayerTests.cpp
)

set(BoardTest_Files
    BoardTests/ChessboardTests.cpp
)

set(Test_Files
    ${MoveTest_Files}
    ${BoardTest_Files}
    ${PlayerTest_Files}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Test_Files})


#==========================================
#   Setup Test Project
#==========================================

add_executable(Engine.Core.Tests ${Test_Files})

target_link_libraries(Engine.Core.Tests PRIVATE Chess.Engine.Core)

AddTests(Engine.Core.Tests)
