cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(PROJECT_NAME "Google Maps API client for C++11")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

##############################################
# BOOST LIB                                  #
##############################################
set (Boost_USE_MULTITHREADED ON)
set (BOOST_ALL_DYN_LINK ON)

find_package(Boost 1.54.0 REQUIRED COMPONENTS system python filesystem)
if (Boost_FOUND)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
	link_directories(SYSTEM ${Boost_LIBRARY_DIRS})
else()
	message (FATAL_ERROR "Need Boost library 1.54.0+ to continue build.")
	return()
endif()

##############################################
# PYTHON LIBS                                #
##############################################
find_package (PythonLibs REQUIRED)
if (PYTHONLIBS_FOUND)
	include_directories(SYSTEM ${PYTHON_INCLUDE_DIRS})
else()
	message (FATAL_ERROR "Need Python libs to continue build")
	return()
endif()

#############################################
# CURL LIB                                  #
#############################################
find_package (CURL REQUIRED)
if (CURL_FOUND)
	include_directories (${CURL_INCLUDE_DIRS})
	link_directories(${CURL_LIBRARIES})
else()
	message (FATAL_ERROR "Could not found curl library")
	return ()
endif()

#############################################
# CURLCPP LIB                               #
#############################################
find_package (Curlcpp REQUIRED)
if (Curlcpp_FOUND)
	include_directories (${Curlcpp_INCLUDE_DIRS})
	link_directories (${Curlcpp_LIBRARIES})
else()
	message (FATAL_ERROR "Could not find curlcpp library")
	return ()
endif()


set (INCLUDE_DIRS include/gmaps11)
set (SOURCES src/configuration.cc src/directions.cc src/distancematrix.cc
						 src/http_client.cc src/json11.cc)

include_directories(${INCLUDE_DIRS})
add_library (gmaps11_dyn SHARED ${SOURCES})

set_target_properties (gmaps11_dyn PROPERTIES OUTPUT_NAME "gmaps11")
set_target_properties (gmaps11_dyn PROPERTIES VERSION 1.0 SOVERSION 1)

target_link_libraries(gmaps11_dyn ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${Curlcpp_LIBRARIES} ${CURL_LIBRARIES})


############################################
# INSTALL 																 #
############################################
# Install the static library
install (TARGETS gmaps11_dyn
				 LIBRARY DESTINATION lib
				)
# Install the headers
install (	DIRECTORY include/gmaps11
					DESTINATION include
				)

# Install PyCrypto module
find_program(EASY_INSTALL "easy_install")
if (EASY_INSTALL)
	set(EASY_INSTALL_OPTS "-d")
	set(PY_INSTALL_DEST "/usr/lib/python2.7/dist-packages")
	set(PY_CRYPTO "${CMAKE_CURRENT_SOURCE_DIR}/PyCrypto")
	set(INSTALL_COMMAND "${EASY_INSTALL}"
											"${EASY_INSTALL_OPTS}"
											"${PY_INSTALL_DEST}"
											"${PY_CRYPTO}")
	string(REPLACE ";" " " INSTALL_COMMAND "${INSTALL_COMMAND}")
  install(CODE "execute_process(COMMAND ${INSTALL_COMMAND})")
else()
	message (FATAL_ERROR "Please make sure 'easy_install' is installed")
	return()
endif()
