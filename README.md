# C++11 Client for Google Maps Web Services

[![Build Status](https://travis-ci.org/amrfaissal/gmaps11.svg?branch=master)](https://travis-ci.org/amrfaissal/gmaps11)

## Description

This library brings the Google Maps API Web Services to your C++11 application.

The C++11 Client for Google Maps Services is a C++11 Client library for the following Google Maps APIs:

 - [Directions API]
 - [Distance Matrix API]
 - ~~Elevation API~~
 - ~~Geocoding API~~
 - ~~Time Zone API~~
 - ~~Roads API~~
 - ~~Places API~~

## Latest Stable Version

The latest stable version of the library can be found at:
- <https://github.com/amrfaissal/gmaps11/releases/latest>

## Getting started

### Building and Installing

#### Dependencies

- [CMake v2.8 or better](http://www.cmake.org/)
- A C++11 compiler (GCC 4.8+, clang)
- [CURL](http://curl.haxx.se/) library
- [CURLcpp](https://github.com/AmrFaissal/curlcpp) C++ wrapper for CURL
- [Boost library](http://www.boost.org) version 1.54.0+

After downloading and extracting the source from a tarball to a directory, the commands to build `gmaps11` on most systems are:

    mkdir build && cd build
    cmake ..
    cmake --build .

Installing the library and optionally specifying a prefix can be done with:

    cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
    cmake --build . --target install

More information on CMake can be found on its [FAQ](http://www.cmake.org/Wiki/CMake_FAQ).

## Examples

Before we dive into examples, please make sure you have a `gmaps11.config` configuration file in the root of your project.

> `gmaps11.config` is JSON configuration file used by the C++11 client.

### Google Directions API

The [Google Directions API](https://developers.google.com/maps/documentation/directions/intro) is a service that calculates directions between locations using an HTTP request.

`gmaps11` comes with a `DirectionsService` class to help you query The [Google Directions API]:

```cpp
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

```cpp
    std::string err;
    json11::Json json_body = json11::Json::parse(response, err);
    if (err.empty()) { // JSON response is valid
        std::string __status__ = json_body["status"].string_value();
    }
```

## Contributing

Bug reports, Pull requests and Stars are always welcome. For bugs and feature requests, [please create an issue](https://github.com/amrfaissal/gmaps11/issues/new).

## License

The library is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).

[Directions API]: https://developers.google.com/maps/documentation/directions/
[Distance Matrix API]: https://developers.google.com/maps/documentation/distancematrix/
