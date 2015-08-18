/*
 * directions_service.hpp:
 * 		Wrapper to perform requests to Google Maps Directions API.
 *
 *      Author: Faissal Elamraoui
 */
#pragma once

#ifndef DIRECTIONS_HPP_
#define DIRECTIONS_HPP_

#include <vector>
#include <stdexcept>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>

#include "http/http_client.hpp"
#include "utility/convert.hpp"
#include "utility/meta.hpp"
#include "config/configuration.hpp"

namespace googlemaps {

static const std::vector<std::string> & DIRECTIONS_MODES =
		boost::assign::list_of("driving")("walking")("bicycling")("transit");

class DirectionsService {
	typedef boost::optional<std::vector<waypoint_t>> Waypoints_t;
public:
	DirectionsService();
	// You can't specify both departure_time and arrival_time
	std::string query(waypoint_t origin, waypoint_t destination,
			std::string mode = "", Waypoints_t waypoints = boost::none,
			bool alternatives = false, std::string avoid = "",
			std::string language = "", std::string units = "",
			std::string region = "", int departure_time = -1,
			int arrival_time = -1, bool optimize_waypoints = false,
			std::string transit_mode = "",
			std::string transit_routing_preference = "");

private:
	std::string convert_waypoint(waypoint_t waypoint);

private:
	boost::scoped_ptr<http::Client> gm_Client;
};

} /* NAMEPSPACE googlemaps */

#endif /* DIRECTIONS_HPP_ */
