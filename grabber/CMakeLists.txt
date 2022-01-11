

set(GRABBER_SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/dataAnalysis.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/urlParser.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/excelExecuter.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/grabber.cpp"
)

add_library (grabber STATIC ${GRABBER_SOURCES})

target_include_directories(grabber PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> $<INSTALL_INTERFACE:include/githubgrabber>)

if (MSVC)
	target_compile_definitions(grabber PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(grabber PUBLIC CURL::libcurl jsoncpp_lib jsoncpp_object ${XLSX_TARGETS})

file(GLOB GRABBER_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/*.h")
install(FILES ${GRABBER_INCLUDES} DESTINATION include/githubgrabber)

install(
    TARGETS grabber
    EXPORT githubgrabberTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(EXPORT githubgrabberTargets
    NAMESPACE githubgrabber::
    DESTINATION share/githubgrabber
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/githubgrabber-config.cmake.in"
[[include(CMakeFindDependencyMacro)
find_dependency(CURL CONFIG)
find_dependency(jsoncpp CONFIG)
find_dependency(xlsxio CONFIG)

include("${CMAKE_CURRENT_LIST_DIR}/githubgrabberTargets.cmake")
]]
)

configure_file("${CMAKE_CURRENT_BINARY_DIR}/githubgrabber-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/githubgrabber-config.cmake" @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/githubgrabber-config.cmake DESTINATION share/githubgrabber)