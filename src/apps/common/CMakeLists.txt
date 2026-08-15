set(TARGET_NAME Chess.Engine.AppCommon)

set(COMMON_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardPrinter.h      ${CMAKE_CURRENT_SOURCE_DIR}/BoardPrinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MovePrinter.h       ${CMAKE_CURRENT_SOURCE_DIR}/MovePrinter.cpp
)

add_library(${TARGET_NAME} STATIC ${COMMON_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${COMMON_FILES})

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${TARGET_NAME} PUBLIC Chess.Engine.Core)

if(MSVC)
    # The figurine glyphs in BitboardTypes.h are UTF-8 source literals.
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()

if(ENABLE_FORMAT)
    Format(${TARGET_NAME} .)
endif()
