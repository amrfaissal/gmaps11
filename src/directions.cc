#include "directions.hpp"

using namespace googlemaps;

DirectionsService::DirectionsService() {
	 //*NOTE* reset() is the only way to initialize a scoped_ptr inside Ctor
	gm_Client.reset(new http::Client(
			ConfigLoader::Instance()->Get<API_KEY>(),
			ConfigLoader::Instance()->Get<CLIENT_ID>(),
			ConfigLoader::Instance()->Get<CLIENT_SECRET>()
		));
}

std::string DirectionsService::convert_waypoint(waypoint_t waypoint) {
	return convert::as_string(waypoint);
}

std::string DirectionsService::query(waypoint_t origin, waypoint_t destination,
		std::string mode, Waypoints_t waypoints, bool alternatives,
		std::string avoid, std::string language, std::string units,
		std::string region, int departure_time, int arrival_time,
		bool optimize_waypoints, std::string transit_mode,
		std::string transit_routing_preference)
{
	http::UrlParams params = {
			{ "origin", convert_waypoint(origin) },
			{ "destination", convert_waypoint(destination) }
	};

	if (!mode.empty()) {
		// *NOTE* The mode parameter is not validated by the Maps API
		// server. Check here to prevent silent failures.
		if ( meta::in<std::string>(mode, DIRECTIONS_MODES) ) {
			params["mode"] = mode;
		} else {
			throw std::domain_error("Invalid travel mode.");
		}
	}

	if (waypoints) {
		std::vector<std::string> _waypoints_ = convert::transform_list(
				*waypoints);
		if (optimize_waypoints) {
			_waypoints_.insert(_waypoints_.begin(), "optimize:true");
		}
		params["waypoints"] = boost::algorithm::join(_waypoints_, "|");
	}

	if (alternatives) {
		params["alternatives"] = "true";
	}

	// If multiple use '|' as a separtor
	if (!avoid.empty()) {
		params["avoid"] = avoid;
	}

	if (departure_time > 0) {
		params["departure_time"] = departure_time;
	}

	if (arrival_time > 0) {
		params["arrival_time"] = arrival_time;
	}

	if (departure_time > 0 && arrival_time > 0) {
		throw std::domain_error("Should not specify both departure_time and arrival_time.");
	}

	if (!language.empty()) 	params["language"] = language;
	if (!units.empty()) 	params["units"] = units;
	if (!region.empty()) 	params["region"] = region;

	// If multiple use '|' as a separtor
	if (!transit_mode.empty()) {
		params["transit_mode"] = transit_mode;
	}

	if (!transit_routing_preference.empty()) {
		params["transit_routing_preference"] = transit_routing_preference;
	}

	// accepts_clientid parameter defaults to false
	bool _use_client_id = ConfigLoader::Instance()->Get<USE_CLIENT_ID>() == "true";

	return gm_Client->Get("/maps/api/directions/json", params,
			_use_client_id);
}
