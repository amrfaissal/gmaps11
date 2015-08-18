#include "distancematrix.hpp"

using namespace googlemaps;

DistanceMatrixService::DistanceMatrixService() {
	gm_Client.reset(new http::Client(
			ConfigLoader::Instance()->Get<API_KEY>(),
			ConfigLoader::Instance()->Get<CLIENT_ID>(),
			ConfigLoader::Instance()->Get<CLIENT_SECRET>()
			));
}

std::string DistanceMatrixService::convert_path(std::vector<waypoint_t> waypoints) {
	return convert::join_list("|", waypoints);
}

std::string DistanceMatrixService::query(std::vector<waypoint_t> origins,
		std::vector<waypoint_t> destinations, std::string mode,
		std::string language, std::string avoid, std::string units,
		int departure_time, int arrival_time, std::string transit_mode,
		std::string transit_routing_preference)
{
	http::UrlParams params = {
			{ "origins", convert_path(origins) },
			{ "destinations", convert_path(destinations) }
	};

	if (!mode.empty()) {
		// *NOTE* The mode parameter is not validated by the Maps API
		// server. Check here to prevent silent failures.
		if ( meta::in<std::string>(mode, dm_Modes) ) {
			params["mode"] = mode;
		} else {
			throw std::domain_error("Invalid travel mode.");
		}
	}

	if (!language.empty()) params["language"] = language;

	if (!avoid.empty()) {
		if ( meta::in<std::string>(avoid, avoid_Types) ) {
			params["avoid"] = avoid;
		} else {
			throw std::domain_error("Invalid route restriction.");
		}
	}

	if (!units.empty()) params["units"] = units;

	if (departure_time > 0) {
		params["departure_time"] = departure_time;
	}

	if (arrival_time > 0) {
		params["arrival_time"] = arrival_time;
	}

	if (departure_time > 0 && arrival_time > 0) {
		throw std::domain_error("Should not specify both departure_time and arrival_time.");
	}

	// If multiple use '|' as a separator
	if (!transit_mode.empty()) {
		params["transit_mode"] = transit_mode;
	}

	if (!transit_routing_preference.empty()) {
		params["transit_routing_preference"] = transit_routing_preference;
	}

	// accepts_clientid parameter defaults to false
	bool _use_client_id = ConfigLoader::Instance()->Get<USE_CLIENT_ID>() == "true";

	return gm_Client->Get("/maps/api/distancematrix/json", params,
			_use_client_id);
}
