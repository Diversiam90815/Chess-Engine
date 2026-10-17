set(TARGET_NAME Chess.Engine.Core)

set (SOURCE_DIR						${CMAKE_CURRENT_SOURCE_DIR})
	
set (MOVE_DIR						${SOURCE_DIR}/Moves)
set (GAME_MANAGER_DIR				${SOURCE_DIR}/GameManager)
set (GAME_ENGINE_DIR				${SOURCE_DIR}/GameEngine)
set (GAME_CONTROLLER_DIR			${SOURCE_DIR}/GameController)
set (BOARD_DIR						${SOURCE_DIR}/Board)
set (ATTACKTABLES_DIR				${SOURCE_DIR}/AttackTables)
set (PLAYER_DIR						${SOURCE_DIR}/Player)
set (HELPER_DIR						${SOURCE_DIR}/Helper)
set (LOGGER_DIR						${SOURCE_DIR}/Logger)
set (EVENTS_DIR						${SOURCE_DIR}/Events)
set (STATEMACHINE_DIR				${SOURCE_DIR}/StateMachine)
set (EVALUATION_DIR					${SOURCE_DIR}/Evaluation)
set (CLOCK_DIR						${SOURCE_DIR}/Clock)


set(ALL_PROJECT_DIRS 
			${SOURCE_DIR}
			${MOVE_DIR}
			${GAME_MANAGER_DIR}
			${GAME_ENGINE_DIR}
			${GAME_CONTROLLER_DIR}
			${BOARD_DIR}
			${ATTACKTABLES_DIR}
			${PLAYER_DIR}
			${HELPER_DIR}
			${LOGGER_DIR}
			${EVENTS_DIR}
			${STATEMACHINE_DIR}
			${EVALUATION_DIR}
			${CLOCK_DIR}
)


set(STATEMACHINE_FILES
	${STATEMACHINE_DIR}/InputEvent.h
	${STATEMACHINE_DIR}/StateMachine.h	${STATEMACHINE_DIR}/StateMachine.cpp
)

set(GAMEENGINE_FILES
	${GAME_ENGINE_DIR}/GameEngine.h		${GAME_ENGINE_DIR}/GameEngine.cpp
)

set(GAMECONTROLLER_FILES
	${GAME_CONTROLLER_DIR}/GameConfiguration.h
	${GAME_CONTROLLER_DIR}/IGameController.h
	${GAME_CONTROLLER_DIR}/GameController.h		${GAME_CONTROLLER_DIR}/GameController.cpp
)

set(BOARD_FILES
	${BOARD_DIR}/BitboardUtils.h
	${BOARD_DIR}/BitboardTypes.h
	${BOARD_DIR}/ChessBoard.h			${BOARD_DIR}/ChessBoard.cpp
	${BOARD_DIR}/ZobristHash.h			${BOARD_DIR}/ZobristHash.cpp
)

set(ATTACKTABLES_FILES
	${ATTACKTABLES_DIR}/AttackTables.h			${ATTACKTABLES_DIR}/AttackTables.cpp
)
	
set(MOVE_FILES
	${MOVE_DIR}/Move.h
	${MOVE_DIR}/Evaluation/MoveEvaluation.h		${MOVE_DIR}/Evaluation/MoveEvaluation.cpp
	${MOVE_DIR}/Generation/MoveGeneration.h		${MOVE_DIR}/Generation/MoveGeneration.cpp
	${MOVE_DIR}/Validation/MoveValidation.h		${MOVE_DIR}/Validation/MoveValidation.cpp
	${MOVE_DIR}/Execution/MoveExecution.h			${MOVE_DIR}/Execution/MoveExecution.cpp
	${MOVE_DIR}/Notation/MoveNotation.h			${MOVE_DIR}/Notation/MoveNotation.cpp
)

set(GAME_MANAGER_FILES
	${GAME_MANAGER_DIR}/GameManager.h		${GAME_MANAGER_DIR}/GameManager.cpp
)

set(PLAYER_FILES
	${PLAYER_DIR}/Player.h      		${PLAYER_DIR}/Player.cpp
	${PLAYER_DIR}/CPUPlayer.h      		${PLAYER_DIR}/CPUPlayer.cpp
)

set(HELPER_FILES
	${HELPER_DIR}/SystemInfo.h
	${HELPER_DIR}/PieceValues.h
	${HELPER_DIR}/Parameters.h
	${HELPER_DIR}/ThreadBase.h
	${HELPER_DIR}/MagicGenerator.h
)

set(LOGGER_FILES
	${LOGGER_DIR}/LogSink.h             ${LOGGER_DIR}/LogSink.cpp
	${LOGGER_DIR}/EngineLog.h
)


set(EVENTS_FILES
	${EVENTS_DIR}/EngineEvent.h
	${EVENTS_DIR}/EventQueue.h		${EVENTS_DIR}/EventQueue.cpp
)

set(EVALUATION_FILES
	${EVALUATION_DIR}/Evaluation.h  	${EVALUATION_DIR}/Evaluation.cpp
)

set(PCH_FILE						${SOURCE_DIR}/pch.h)

set(CLOCK_FILES
	${CLOCK_DIR}/TimeControl.h
	${CLOCK_DIR}/ClockSnapshot.h
	${CLOCK_DIR}/GameClock.h
	${CLOCK_DIR}/GameClock.cpp
)

set(ALL_FILES
	${PCH_FILE}
	${GAMEENGINE_FILES}
	${GAMECONTROLLER_FILES}
	${BOARD_FILES}
	${ATTACKTABLES_FILES}
	${MOVE_FILES}
	${GAME_MANAGER_FILES}
	${PLAYER_FILES}
	${HELPER_FILES}
	${LOGGER_FILES}
	${EVENTS_FILES}
	${STATEMACHINE_FILES}
	${EVALUATION_FILES}
	${CLOCK_FILES}
)

add_library(${TARGET_NAME} STATIC ${ALL_FILES})

include(BuildInfo)
BuildInfo(${TARGET_NAME})

source_group(TREE ${SOURCE_DIR} FILES ${ALL_FILES})

target_precompile_headers(${TARGET_NAME} PRIVATE ${PCH_FILE})

if(ENABLE_CPPCHECK)
	AddCppCheck(${TARGET_NAME})
endif()

if(ENABLE_FORMAT)
	Format(${TARGET_NAME} .)
endif()

if(ENABLE_DOXYGEN)
	Doxygen(Chess_Engine ${SOURCE_DIR})
endif()

target_compile_definitions(${TARGET_NAME} PUBLIC
	ENV_DEVELOPMENT
)

if(MSVC)
	target_compile_definitions(${TARGET_NAME} PUBLIC
		_CRT_SECURE_NO_WARNINGS
		_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
	)

	target_compile_options(${TARGET_NAME} PRIVATE
		$<$<CONFIG:Release>:
			/Oi
			/Gy
		>
		$<$<CONFIG:Debug>:
			/Od
			/Z7
		>
		/sdl
		/MP
		/utf-8       
		/WX-        # Don't treat warnings as errors
	)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(${TARGET_NAME} PRIVATE
		-Wall
		-Wextra
	)
endif()

set(Include_Dirs 
		${PROJECT_BINARY_DIR}
		${ALL_PROJECT_DIRS}
)

target_include_directories(${TARGET_NAME} PUBLIC ${Include_Dirs})

