set(TARGET_NAME Chess.Engine.Console)


set(APP_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/App)
set(INPUT_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/Input)
set(VIEW_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/View)


set(APP_FILES
    ${APP_DIR}/ConsoleApp.h     ${APP_DIR}/ConsoleApp.cpp
    ${APP_DIR}/GameSetup.h      ${APP_DIR}/GameSetup.cpp
)

set(INPUT_FILES
    ${INPUT_DIR}/Command.h
    ${INPUT_DIR}/CommandParser.h    ${INPUT_DIR}/CommandParser.cpp
)

set(VIEW_FILES
    ${VIEW_DIR}/GameView.h      ${VIEW_DIR}/GameView.cpp
)

set(ALL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${APP_FILES}
    ${INPUT_FILES}
    ${VIEW_FILES}
)


add_executable(${TARGET_NAME} ${ALL_FILES})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${APP_DIR}
    ${INPUT_DIR}
    ${VIEW_DIR}
)

target_link_libraries(${TARGET_NAME} PRIVATE Chess.Engine.Core Chess.Engine.AppCommon)

if(MSVC)
    # The figurine glyphs in BitboardTypes.h are UTF-8 source literals.
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
        _WINDOWS
        WIN32_LEAN_AND_MEAN
        _WINSOCK_DEPRECATED_NO_WARNING
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
        NTDDI_VERSION=NTDDI_WIN10
    )
endif()

if(ENABLE_FORMAT)
    Format(${TARGET_NAME} .)
endif()
