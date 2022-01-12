add_definitions(-DUNICODE -D_UNICODE)

set(APP_CHECKER_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/GithubGrabber.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/mainFrame.cpp"
)

add_executable (GithubGrabber WIN32 ${APP_CHECKER_SOURCES})

target_include_directories(GithubGrabber PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(GithubGrabber PRIVATE _CRT_SECURE_NO_WARNINGS)

target_link_libraries(GithubGrabber PRIVATE dui::duilib grabber)

install(TARGETS GithubGrabber RUNTIME DESTINATION bin)

file(GLOB APP_CHECKER_RESOURCES "${CMAKE_CURRENT_LIST_DIR}/*.png")
list(APPEND APP_CHECKER_RESOURCES "${CMAKE_CURRENT_LIST_DIR}/grabberMainFrame.xml")

install(FILES ${APP_CHECKER_RESOURCES} DESTINATION bin)