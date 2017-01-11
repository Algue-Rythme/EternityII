#ifndef UTILS_HPP
#define UTILS_HPP

#include <complex>

#include "SFML/System/Vector2.hpp"

template<typename T>
inline sf::Vector2<T> makeVector2(std::complex<T> value)
{
	return sf::Vector2<T>(value.real(), value.imag());
}

#endif