/*
 * http_client.h :
 * 		Core client functionality, common across all API requests
 * 		(including performing HTTP requests).
 *
 *      Author: Faissal Elamraoui
 */
#pragma once

#ifndef _CLIENT_HTTP_CLIENT_HPP_
#define _CLIENT_HTTP_CLIENT_HPP_

#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <cmath> // std::pow
#include <cstdlib>
#include <thread>
#include <sstream>
#include <algorithm> // std::sort
#include <iostream> // std::cout
// boost
#include <boost/algorithm/string/join.hpp> // join
#include <boost/algorithm/string/predicate.hpp> // starts_with
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/python.hpp>
// boost - using Python 2.x
#include <python2.7/Python.h>
#include <boost/filesystem.hpp>

// curlcpp
#include <curlcpp/curl_easy.h>
#include <functional> // std::function
#include "../json/json11.hpp" // Json::Parse(str, err)
// meta stuff
#include "../utility/meta.hpp"
// exceptions
#include "../except/exceptions.hpp"
#include <stdexcept>

/*
 * Core client functionality, common across all API requests (including performing HTTP requests).
 */
namespace http {
using namespace std::chrono;

typedef time_point<system_clock> Timestamp;
typedef std::map<std::string, std::string> UrlParams;

static const std::string googleapis_BASE_URL("https://maps.googleapis.com");
static const std::vector<long> RETRIABLE_STATUSES = { 500, 503, 504 };


class Client {
public:
	Client(std::string key = "", std::string client_id = "",
			std::string client_secret = "", int timeout = -1,
			int connect_timeout = -1, int retry_timeout = 60);

	std::string Get(std::string url, UrlParams params, bool accepts_clientid = false,
			boost::optional<Timestamp> first_request_time = boost::none,
			int retry_counter = 0, std::string base_url = googleapis_BASE_URL,
			std::function<std::string(std::string)> extract_body = nullptr);

private:
	double unirand();
	std::string get_body(std::string response);
	std::string urlencode_params(const UrlParams & params);
	std::string sign_url(std::string secret, std::string url_payload);
	std::string generate_auth_url(std::string path, UrlParams params,
			bool accepts_clientid);
	std::string parse_python_exception();

private:
	std::string key_;
	int timeout_;
	int connect_timeout_;

	int retry_timeout_;
	std::string client_id_;
	std::string client_secret_; // base64 encoded
	boost::python::object crypto_mod_;
};

}

#endif /* _CLIENT_HTTP_CLIENT_HPP_ */
