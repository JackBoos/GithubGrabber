add_definitions(-DUNICODE -D_UNICODE)

set(APP_GRABBER_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/GithubChecker.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/mainFrame.cpp"
)

add_executable (GithubChecker WIN32 ${APP_GRABBER_SOURCES})

target_include_directories(GithubChecker PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(GithubChecker PRIVATE _CRT_SECURE_NO_WARNINGS)

target_link_libraries(GithubChecker PRIVATE dui::duilib grabber)

install(TARGETS GithubChecker RUNTIME DESTINATION bin)