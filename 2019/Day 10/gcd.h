
#include <algorithm>
#include <type_traits>

template <class T>
typename std::enable_if<(std::is_integral<T>::value && std::is_unsigned<T>::value), T>::type
gcd(T a, T b)
{
	T x = std::min(a, b);
	T y = std::max(a, b);
	return x == 0 ? y : gcd(x, y % x);
}

template <class T>
typename std::enable_if<(std::is_integral<T>::value && std::is_signed<T>::value), T>::type
gcd(T a, T b)
{
	typedef typename std::make_unsigned<T>::type UT;
	return gcd(static_cast<UT>(std::abs(a)), static_cast<UT>(std::abs(b)));
}
