add_definitions(-DUNICODE -D_UNICODE)

set(APP_GETREPOS_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/main.cpp"
)

add_executable (GetRepos ${APP_GETREPOS_SOURCES})

target_include_directories(GetRepos PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(GetRepos PRIVATE _CRT_SECURE_NO_WARNINGS)
set(CMAKE_CXX_STANDARD 17)

target_link_libraries(GetRepos PRIVATE dui::duilib grabber)