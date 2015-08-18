/*
 * exceptions.hpp:
 * 	   Defines exceptions that are thrown by the Google Maps client.
 *
 *     Author: Faissal Elamraoui
 */
#pragma once

#ifndef EXCEPT_EXCEPTIONS_HPP_
#define EXCEPT_EXCEPTIONS_HPP_

#include <string>
#include <boost/format.hpp>

namespace googlemaps {
namespace exceptions {

// Something went wrong while trying to execute a request
class base_error {
public:
	virtual ~base_error() = default;
	virtual std::string __str__() = 0;
};

// Represents an error returned by the remote API
class api_error: public base_error {
	std::string status_;
	std::string message_;
public:
	api_error(std::string status, std::string message = "") :
			status_(status), message_(message) {
	}
	virtual std::string __str__() override {
		return boost::str(boost::format("%s (%s)") % status_ % message_);
	}
	~api_error() = default;
};

// An unexpected HTTP error occurred
class http_error: public base_error {
	int status_code_;
public:
	http_error(int status_code) :
			status_code_(status_code) {
	}

	virtual std::string __str__() override {
		return boost::str(boost::format("HTTP Error: %d") % status_code_);
	}
	~http_error() = default;
};

// The request timed out
class timeout_error {
	// pass
	;
};

// Signifies that the request can be retried
class retriable_request_error {
	// pass
	;
};
}

}

#endif /* EXCEPT_EXCEPTIONS_HPP_ */
