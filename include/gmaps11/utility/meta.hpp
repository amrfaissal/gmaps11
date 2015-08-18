/*
 * meta.hpp:
 * 		Some meta-stuff!
 *
 *      Author: Faissal Elamraoui
 */
#pragma once

#ifndef UTILITY_META_HPP_
#define UTILITY_META_HPP_

#include <vector>
#include <algorithm> // std::find


namespace meta {

// template metaclass that allows to check
// if value of type T is inside an std::vector<T>
// e.g: meta::in<long>(200l, Container);
template<typename T>
class in {
	T value_;
	const std::vector<T> & C_;
public:
	in(T value, const std::vector<T> & C) :
			value_(value), C_(C) {
	}
	operator bool() {
		return std::find(C_.begin(), C_.end(), value_) != C_.end();
	}
};

}

#endif /* UTILITY_META_HPP_ */
