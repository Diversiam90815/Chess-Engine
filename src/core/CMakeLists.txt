set(TARGET_NAME Chess.Engine.Core)

set (SOURCE_DIR						${CMAKE_CURRENT_SOURCE_DIR})
set (SUBMODULES_DIR					${CMAKE_CURRENT_SOURCE_DIR}/submodule)
	
set (MOVE_DIR						${SOURCE_DIR}/Moves)
set (GAME_MANAGER_DIR				${SOURCE_DIR}/GameManager)
set (GAME_ENGINE_DIR				${SOURCE_DIR}/GameEngine)
set (GAME_CONTROLLER_DIR			${SOURCE_DIR}/GameController)
set (BOARD_DIR						${SOURCE_DIR}/Board)
set (ATTACKTABLES_DIR				${SOURCE_DIR}/AttackTables)
set (PLAYER_DIR						${SOURCE_DIR}/Player)
set (HELPER_DIR						${SOURCE_DIR}/Helper)
set (FILEMANAGER_DIR				${SOURCE_DIR}/FileManager)
set (LOGGER_DIR						${SOURCE_DIR}/Logger)
set (USERSETTINGSCACHE_DIR			${SOURCE_DIR}/UserSettingsCache)
set (INPUTSOURCE_DIR				${SOURCE_DIR}/InputSource)
set (STATEMACHINE_DIR				${SOURCE_DIR}/StateMachine)
set (EVALUATION_DIR					${SOURCE_DIR}/Evaluation)
set (MULTIPLAYER_DIR				${SOURCE_DIR}/Multiplayer)


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
			${FILEMANAGER_DIR}
			${USERSETTINGSCACHE_DIR}
			${INPUTSOURCE_DIR}
			${STATEMACHINE_DIR}
			${EVALUATION_DIR}
			${MULTIPLAYER_DIR}
)

include_directories(${ALL_PROJECT_DIRS})


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
	${BOARD_DIR}/Chessboard.h			${BOARD_DIR}/Chessboard.cpp
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
	${PLAYER_DIR}/PlayerName.h			${PLAYER_DIR}/PlayerName.cpp
	${PLAYER_DIR}/CPUPlayer.h      		${PLAYER_DIR}/CPUPlayer.cpp
)

set(HELPER_FILES
	${HELPER_DIR}/SystemInfo.h
	${HELPER_DIR}/PieceValues.h
	${HELPER_DIR}/Parameters.h
	${HELPER_DIR}/ThreadBase.h
	${HELPER_DIR}/Conversion.h
	${HELPER_DIR}/IObserver.h
	${HELPER_DIR}/IObservable.h
	${HELPER_DIR}/MagicGenerator.h
)

set(LOGGER_FILES
	${LOGGER_DIR}/Logging.h             ${LOGGER_DIR}/Logging.cpp
)

set(FILEMANAGER_FILES
	${FILEMANAGER_DIR}/FileManager.h    ${FILEMANAGER_DIR}/FileManager.cpp
)

set(USERSETTINGSCACHE_FILES
	${USERSETTINGSCACHE_DIR}/UserSettingsCache.h	${USERSETTINGSCACHE_DIR}/UserSettingsCache.cpp
)

set(INPUTSOURCE_FILES
	${INPUTSOURCE_DIR}/PlainCDefines.h   
	${INPUTSOURCE_DIR}/IInputSource.h
	${INPUTSOURCE_DIR}/WinUIInputSource.h    ${INPUTSOURCE_DIR}/WinUIInputSource.cpp
)

set(EVALUATION_FILES
	${EVALUATION_DIR}/Evaluation.h  	${EVALUATION_DIR}/Evaluation.cpp
)

set(MULTIPLAYER_FILES
	${MULTIPLAYER_DIR}/MultiplayerTypes.h
	${MULTIPLAYER_DIR}/MultiplayerManager.h		${MULTIPLAYER_DIR}/MultiplayerManager.cpp
)

set(ALL_FILES
	${GAMEENGINE_FILES}
	${GAMECONTROLLER_FILES}
	${BOARD_FILES}
	${ATTACKTABLES_FILES}
	${MOVE_FILES}
	${GAME_MANAGER_FILES}
	${PLAYER_FILES}
	${HELPER_FILES}
	${LOGGER_FILES}
	${FILEMANAGER_FILES}
	${USERSETTINGSCACHE_FILES}
	${INPUTSOURCE_FILES}
	${STATEMACHINE_FILES}
	${EVALUATION_FILES}
	${MULTIPLAYER_FILES}
)

include(cpm)

CPMAddPackage(
  NAME Logger
  GITHUB_REPOSITORY Diversiam90815/Logger
  VERSION 2.0.86
  GIT_TAG v2.0.86
)

CPMAddPackage(
  NAME NetLink
  GITHUB_REPOSITORY Diversiam90815/NetLink
  GIT_TAG v0.1.0
)

add_library(${TARGET_NAME} STATIC ${ALL_FILES})

include(BuildInfo)
BuildInfo(${TARGET_NAME})

source_group(TREE ${SOURCE_DIR} FILES ${ALL_FILES})

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
	_CRT_SECURE_NO_WARNINGS
	_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
	_WINDOWS
	WIN32_LEAN_AND_MEAN
	_WINSOCK_DEPRECATED_NO_WARNING
	_WIN32_WINNT=0x0A00
	WINVER=0x0A00
	NTDDI_VERSION=NTDDI_WIN10 
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
	/WX-        # Don't treat warnings as errors
)

set(Include_Dirs 
		${PROJECT_BINARY_DIR}
		${ALL_PROJECT_DIRS}
		${LOGGER_INCLUDE_FILE}
)

target_include_directories(${TARGET_NAME} PUBLIC ${Include_Dirs})

set(TARGET_LIBS		Logger
					windowscodecs
					WindowsApp
					nlohmann_json::nlohmann_json
					NetLink
)

target_link_libraries(${TARGET_NAME} PUBLIC ${TARGET_LIBS})
