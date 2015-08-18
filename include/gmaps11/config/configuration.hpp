/**
 * Configuration.hpp:
 * 		Wrapper to load app-specific global configuration
 *
 * 		Author: Faissal Elamraoui
 */
#pragma once

#ifndef _CONFIG_CONFIGURATION_HPP_
#define _CONFIG_CONFIGURATION_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include <map>
#include <stdexcept>
#include "../json/json11.hpp"
// Boost
#include <boost/variant.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>

typedef enum {
	// GMaps API
	API_KEY,
	USE_CLIENT_ID,
	CLIENT_ID,
	CLIENT_SECRET,
	MODE,
	LANG,
	// MySQL
	MYSQL_DB_NAME,
	MYSQL_HOST,
	MYSQL_USER,
	MYSQL_PWD,
	// Query limits
	START,
	COUNT,
	QPS,
	CADENCE,
	DISTANCE_LIMIT,
	OUTPUT_FILE
} Setting;


class ConfigLoader {
public:
	// Gets a setting value (string) from configuration map
	// e.g: ConfigLoader::Instance()->Get<API_KEY>();
	template<Setting S> std::string Get() {
		return __settings__[S];
	}

	static ConfigLoader * const Instance() {
		if (instance == nullptr) {
			instance = new ConfigLoader();
		}
		return instance;
	}

	// string representation of the current configuration
	std::string __str__() {
		return __cfgstr__;
	}

	// print the current load configuration to ostream
	// *NOTE* For DEBUG usage only
	void print_settings(std::ostream & os) {
		for (auto & kv: __settings__) {
			os << kv.first << " - " << kv.second << '\n';
		}
	}

private:
	 /* Load all settings in the Ctor */
	ConfigLoader() {
		boost::filesystem::path configFile("gmaps11.config");
		std::string configPath;
		try {
			// Check if the configuration file exists
			if (!boost::filesystem::exists(configFile)) {
				std::cerr << "Unable to find configuration file: "
									<< boost::filesystem::absolute(configFile).native()
									<< std::endl;
			}

			// Get configuration file absolute path
			configPath = boost::filesystem::absolute(configFile).native();
		} catch (const boost::filesystem::filesystem_error& e) {
			std::cerr << e.what() << std::endl;
		}

		// load and read the configuration file
		namespace io = boost::iostreams;
		io::file_source configSource { configPath.c_str() };
		if ( configSource.is_open() )
		{
			io::stream<io::file_source> is{configSource};
			std::stringstream buffer;
			buffer << is.rdbuf();

			// parse the JSON formatted configuration file
			std::string err;
			json11::Json configJson = json11::Json::parse(buffer.str(), err);
			if (!err.empty()) {
				throw std::domain_error("Invalid format configuration file: " + err);
			} else {
				__cfgstr__ = configJson.dump();
				// GoogleMaps APIs settings
				const json11::Json gmApiConfig = configJson["gm_api"];
				if (!gmApiConfig.is_null()) {
					__settings__[API_KEY] 		= gmApiConfig["api_key"].string_value();
					__settings__[USE_CLIENT_ID] = gmApiConfig["use_clientid"].string_value();
					__settings__[CLIENT_ID] 	= gmApiConfig["client_id"].string_value();
					__settings__[CLIENT_SECRET] = gmApiConfig["client_secret"].string_value();
					__settings__[MODE] 			= gmApiConfig["mode"].string_value();
					__settings__[LANG] 			= gmApiConfig["language"].string_value();
				}
				// Database-related settings
				const json11::Json mysqlDBConfig = configJson["databases"]["mysql"];
				if (!mysqlDBConfig.is_null()) {
					__settings__[MYSQL_DB_NAME] = mysqlDBConfig["db_name"].string_value();
					__settings__[MYSQL_HOST] 	= mysqlDBConfig["host"].string_value();
					__settings__[MYSQL_USER] 	= mysqlDBConfig["user"].string_value();
					__settings__[MYSQL_PWD] 	= mysqlDBConfig["passwd"].string_value();
				}
				// Query limits
				const json11::Json queryLimitsConfig = configJson["query_limits"];
				if (!queryLimitsConfig.is_null()) {
					__settings__[START] 			= queryLimitsConfig["start"].string_value();
					__settings__[COUNT] 			= queryLimitsConfig["count"].string_value();
					__settings__[QPS]				= queryLimitsConfig["queries_per_second"].string_value();
					__settings__[CADENCE] 			= queryLimitsConfig["cadence"].string_value();
					__settings__[DISTANCE_LIMIT] 	= queryLimitsConfig["distance_limit_km"].string_value();
					__settings__[OUTPUT_FILE]		= queryLimitsConfig["output_file"].string_value();
				}
			}
			configSource.close();
		} else {
			std::cerr << "Could not open configuration file." << std::endl;
		}
	}

private:
	std::map<Setting, std::string> __settings__;
	std::string __cfgstr__;
	static ConfigLoader * instance;
};

#endif
