/*
 * distancematrix_service.h:
 * 		Wrapper to perform requests to the Google Maps Distance Matrix API.
 *
 *      Author: Faissal Elamraoui
 */
#pragma once

#ifndef DISTANCEMATRIX_HPP_
#define DISTANCEMATRIX_HPP_

#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/variant.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

#include "http/http_client.hpp"
#include "utility/convert.hpp"
#include "utility/meta.hpp"
#include "config/configuration.hpp"


namespace googlemaps {

static const std::vector<std::string> & dm_Modes = boost::assign::list_of(
		"driving")("walking")("bicycling")("transit");
static const std::vector<std::string> & avoid_Types = boost::assign::list_of(
		"tolls")("highways")("ferries");

class DistanceMatrixService {
public:
	DistanceMatrixService();
	std::string query(std::vector<waypoint_t> origins,
			std::vector<waypoint_t> destinations, std::string mode = "",
			std::string language = "", std::string avoid = "",
			std::string units = "", int departure_time = -1,
			int arrival_time = -1, std::string transit_mode = "",
			std::string transit_routing_preference = "");

private:
	std::string convert_path(std::vector<waypoint_t> waypoints);

private:
	boost::scoped_ptr<http::Client> gm_Client;
};

}

#endif /* DISTANCEMATRIX_HPP_ */
