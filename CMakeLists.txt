cmake_minimum_required (VERSION 3.8)

project(GithubGrabber CXX)

option(BUILD_APP "Build App" ON)

# For core
find_package(CURL CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(xlsxio CONFIG REQUIRED)

if (TARGET xlsxio::xlsxio_write_SHARED)
	set(XLSX_TARGETS xlsxio::xlsxio_write_SHARED)
elseif(TARGET xlsxio::xlsxio_write_STATIC)
	set(XLSX_TARGETS xlsxio::xlsxio_write_STATIC)
else()
	message(FATAL_ERROR "Can't find dependency xlsxio.")
endif()

add_subdirectory(grabber)

# For Gui
if (BUILD_APP)
	find_package(duilib CONFIG REQUIRED)
	add_subdirectory(app)
endif()
