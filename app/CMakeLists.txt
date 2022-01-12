add_definitions(-DUNICODE -D_UNICODE)

add_executable (GithubGrabber WIN32 "GithubGrabber.cpp" "mainFrame.cpp")

target_include_directories(GithubGrabber PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(GithubGrabber PRIVATE _CRT_SECURE_NO_WARNINGS)

target_link_libraries(GithubGrabber PRIVATE dui::duilib grabber)

install(TARGETS GithubGrabber RUNTIME DESTINATION bin)