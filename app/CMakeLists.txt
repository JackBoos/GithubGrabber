
if (BUILD_APP_Grabber AND WIN32)
	add_subdirectory(GithubGrabber)
endif()

if (BUILD_APP_Checker AND WIN32)
	add_subdirectory(GithubChecker)
endif()

if (BUILD_APP_GitMerger AND WIN32)
	add_subdirectory(gitMerger)
endif()

