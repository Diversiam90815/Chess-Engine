set (API_FILES_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/API)

set (API_FILES			${API_FILES_DIR}/EngineTypes.h
						${API_FILES_DIR}/EngineAPI.h			${API_FILES_DIR}/EngineAPI.cpp
						${API_FILES_DIR}/EngineInstance.h		${API_FILES_DIR}/EngineInstance.cpp
						${API_FILES_DIR}/CallbackBridge.h		${API_FILES_DIR}/CallbackBridge.cpp
						${API_FILES_DIR}/TypeConversion.h		${API_FILES_DIR}/TypeConversion.cpp)

set (Include_Dirs 		${PROJECT_BINARY_DIR}			
						${API_FILES_DIR})

set (PROJECT_LIBS		Chess.Engine.Core)

include_directories(${API_FILES_DIR})

source_group("API" FILES ${API_FILES})

add_library(Chess.Engine.API SHARED ${API_FILES})

if(ENABLE_FORMAT)
	Format(Chess.Engine.API .)
endif()

target_include_directories(Chess.Engine.API PUBLIC ${Include_Dirs})

target_link_libraries(Chess.Engine.API PUBLIC ${PROJECT_LIBS})
