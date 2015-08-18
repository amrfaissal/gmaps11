find_library (Curlcpp_LIBRARIES NAMES curlcpp HINTS /usr/local/lib NO_DEFAULT_PATH)
find_path (Curlcpp_INCLUDE_DIRS curl_config.h curl_header.h curl_option.h curl_share.h
							curl_easy.h curl_info.h curl_pair.h curl_utility.h
							curl_exception.h curl_interface.h curl_receiver.h curl_writer.h
							curl_form.h curl_multi.h curl_sender.h
							/usr/local/include/curlcpp)

if (${Curlcpp_LIBRARIES} MATCHES "NOTFOUND")
  set (Curlcpp_FOUND FALSE CACHE INTERNAL "")
  message (STATUS "Curlcpp library not found")
  unset (Curlcpp_LIBRARIES)
else()
  set (Curlcpp_FOUND TRUE CACHE INTERNAL "")
  message (STATUS "Found Curlcpp library: ${Curlcpp_LIBRARIES}")
endif()

set(CMAKE_REQUIRED_INCLUDES ${Curlcpp_INCLUDE_DIRS})
