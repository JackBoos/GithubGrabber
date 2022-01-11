add_definitions(-DUNICODE -D_UNICODE)

add_executable (GithubGrabber WIN32 "GithubGrabber.cpp" "mainFrame.cpp")

target_include_directories(GithubGrabber PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# TODO: Add tests and install targets if needed.

target_link_libraries(GithubGrabber PRIVATE dui::duilib grabber)

install(TARGETS GithubGrabber RUNTIME DESTINATION bin)