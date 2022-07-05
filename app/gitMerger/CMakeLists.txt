add_definitions(-DUNICODE -D_UNICODE)

set(APP_GITMERGER_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/gitMerger.cpp"
)

add_executable (gitMerger ${APP_GITMERGER_SOURCES})

target_include_directories(gitMerger PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(gitMerger PRIVATE _CRT_SECURE_NO_WARNINGS)

target_link_libraries(gitMerger PRIVATE grabber)

install(TARGETS gitMerger RUNTIME DESTINATION bin)
