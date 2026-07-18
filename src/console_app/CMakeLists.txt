set(TARGET_NAME Chess.Engine.ConsoleApp)


set(APP_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/App)
set(PRINTING_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/Printing)
set(SCENARIOS_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/Scenarios)


set(APP_FILES
    ${APP_DIR}/ConsoleApp.h       ${APP_DIR}/ConsoleApp.cpp
)

set(PRINTING_FILES
    ${PRINTING_DIR}/BoardPrinter.h      ${PRINTING_DIR}/BoardPrinter.cpp
    ${PRINTING_DIR}/MovePrinter.h       ${PRINTING_DIR}/MovePrinter.cpp
)

set(SCENARIO_FILES
    ${SCENARIOS_DIR}/IScenario.h
    ${SCENARIOS_DIR}/AttackMapScenario.h            ${SCENARIOS_DIR}/AttackMapScenario.cpp
    ${SCENARIOS_DIR}/EvaluationScenario.h           ${SCENARIOS_DIR}/EvaluationScenario.cpp
    ${SCENARIOS_DIR}/MoveGenerationScenario.h       ${SCENARIOS_DIR}/MoveGenerationScenario.cpp
    ${SCENARIOS_DIR}/MoveOrderingScenario.h         ${SCENARIOS_DIR}/MoveOrderingScenario.cpp
    ${SCENARIOS_DIR}/PerftScenario.h                ${SCENARIOS_DIR}/PerftScenario.cpp
)

set(ALL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${APP_FILES}
    ${PRINTING_FILES}
    ${SCENARIO_FILES}
)


add_executable(${TARGET_NAME} ${ALL_FILES})


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${APP_DIR}
    ${PRINTING_DIR}
    ${SCENARIOS_DIR}
)

target_link_libraries(${TARGET_NAME} PRIVATE Chess.Engine.Core)

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
