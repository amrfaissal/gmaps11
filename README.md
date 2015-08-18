Google Maps API Web Services Client for C++11
------

## Introduction
gmaps11 is a simple C++11 client library for [Google Maps API Web Services](https://developers.google.com/maps/documentation/webservices/)

## Latest Stable Version
The latest stable version of the gmaps11 library can be found at:
- <https://github.com/amrfaissal/gmaps11/releases/latest>

## Getting started
### Building and Installing
#### Dependencies :
- [CMake v2.8 or better](http://www.cmake.org/)
- A C++11 compiler (GCC 4.8+, clang)
- [CURL](http://curl.haxx.se/) library
- [curlcpp](https://github.com/AmrFaissal/curlcpp) C++ wrapper for CURL
- [Boost library](http://www.boost.org) version 1.54.0+

After downloading and extracting the source from a tarball to a directory.
([see above][Latest Stable Version]), the commands to build gmaps11 on most systems are:

    mkdir build && cd build
    cmake ..
    cmake --build .

Installing the library and optionally specifying a prefix can be done with:

    cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
    cmake --build . --target install

More information on CMake can be found on its [FAQ](http://www.cmake.org/Wiki/CMake_FAQ).

## Examples
Before we dive into examples, please make sure you have a `gmaps11.config` configuration file in the root of your project. `gmaps11.config` is JSON configuration file used by all the Google Maps services in the library.
### Google Directions API
The [Google Directions API](https://developers.google.com/maps/documentation/directions/intro) is a service that calculates directions between locations using an HTTP request.

`gmaps11` comes with a `DirectionsService` class to help you query the The [Google Directions API](https://developers.google.com/maps/documentation/directions/intro) :

```
   #include <gmaps11/directions.hpp>
   #include <boost/scoped_ptr.hpp>
   #include <iostream>

   using namespace googlemaps;

   auto main() -> int {
   boost::scoped_ptr<DirectionsService> directions_service(new DirectionsService());
   // Query the Google Maps Directions API
   // *NOTE* 'waypoint_t' is a typdef for boost::variant which can be a
   // string holding a name (city, place...) or a boost tuple holding lat and long
   // of a position
   std::string response = directions_service->query("Paris", boost::make_tuple(44.051682, 4.643433), "driving");
   // Print the JSON response
   std::cout << response << std::endl;
  }

```

## Notes
You can parse the JSON response using the JSON parser in `gmaps11/json/json11.hpp` header:

```
    std::string err;
    json11::Json json_body = json11::Json::parse(response, err);
    if (err.empty()) { // JSON response is valid
        std::string __status__ = json_body["status"].string_value();
    }

```





