/*
 * convert.h:
 * 		Convert complex types to string representations suitable for Google Maps server.
 *
 *      Author: Faissal Elamraoui
 */
#pragma once

#ifndef _UTILITY_CONVERT_HPP_
#define _UTILITY_CONVERT_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/tuple/tuple.hpp>
#include <algorithm>

// A tuple of lat/lng values
typedef boost::tuple<double, double> latlng_t;
// A waypoint can be identified by an address (string)
// or a LatLng tuple (latlng_t)
typedef boost::variant<std::string, latlng_t> waypoint_t;

class wp_stringify: public boost::static_visitor<std::string> {
public:
	std::string operator()(std::string const & str) const {
		return str;
	}
	std::string operator()(latlng_t const & t) const {
		std::stringstream ss;
		ss << t.get<0>() << "," << t.get<1>();
		return ss.str();
	}
};

class convert {
	typedef std::vector<waypoint_t> Container;
public:
	inline static std::vector<std::string> transform_list(Container target) {
		// Stringify everything in the container
		wp_stringify str_visitor;

		std::vector<std::string> trans_list;
		trans_list.reserve(target.size());
		for (auto & elem : target)
			trans_list.push_back(boost::apply_visitor(str_visitor, elem));

		return trans_list;
	}

	inline static std::string join_list(const char * sep, Container list) {
		std::vector<std::string> _list = transform_list(list);
		return boost::algorithm::join(_list, sep);
	}

	inline static std::string as_string(waypoint_t wp) {
		wp_stringify str_visitor;
		return boost::apply_visitor(str_visitor, wp);
	}
};

#endif /* _UTILITY_CONVERT_HPP_ */
