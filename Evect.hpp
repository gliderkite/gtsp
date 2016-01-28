#ifndef EVECT_HPP
#define EVECT_HPP


namespace pso
{
	template<class T>
	struct evect
	{
		explicit evect(T x = 0, T y = 0)
		: x(x), y(y)
		{
		}

		evect<T>& operator+=(const evect<T>& rhs)
		{
			x += rhs.x;
			y += rhs.y;

			return *this;
		}

  		friend evect<T> operator+(evect<T> lhs, const evect<T>& rhs)
		{
			lhs += rhs;
			return lhs;
		}

		evect<T>& operator-=(const evect<T>& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;

			return *this;
		}

  		friend evect<T> operator-(evect<T> lhs, const evect<T>& rhs)
		{
			lhs -= rhs;
			return lhs;
		}


		evect<T>& operator*=(T val)
		{
			x *= val;
			y *= val;

			return *this;
		}

  		friend evect<T> operator*(evect<T> lhs, T val)
		{
			lhs *= val;
			return val;
		}

		friend evect<T> operator*(T val, const evect<T>& rhs)
		{
			return evect<T>(val * rhs.x, val * rhs.y);
		}


		T x;
		T y;
	};
}


#endif