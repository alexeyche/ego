#pragma once

#include "errors.h"

#include <memory>
#include <string>
#include <vector>
#include <tuple>

namespace NEgo {

	template <typename T>
	using UPtr = std::unique_ptr<T>;

	template <typename T>
	using SPtr = std::shared_ptr<T>;


	template <typename X, typename Y>
	using TPair = std::pair<X, Y>;

	using TString = std::string;

	#define JOIN(X, Y) X##Y

	#define GENERATE_UNIQUE_ID(N) JOIN(N, __LINE__)

	template <typename X, typename Y>
	TPair<X, Y> MakePair(X x, Y y) {
		return TPair<X, Y>(x, y);
	}

    template <typename T>
    using TVector = std::vector<T>;

    using ui32 = size_t;

    //template< class... Types>
    //using Tie = std::tie<Types& ...>;


	template< typename T1, typename T2>
	class TTie {
	public:
	 	TTie(T1 &first,T2 &second)
	  		: First(first)
	  		, Second(second)
	  	{
	  	}

	 	TPair<T1, T2> const & operator = (TPair<T1, T2> const &rhs) {
		    First = rhs.first;
		    Second = rhs.second;
		    return rhs;
		}

	private:
	  	void operator=(TTie const &);
	  	T1 &First;
	  	T2 &Second;
	};

	template <typename T1, typename T2>
	inline TTie<T1,T2> Tie(T1 &first, T2 &second)
	{
	  return TTie<T1, T2>(first, second);
	}

	template <typename T>
	SPtr<T> MakeShared(T *ptr) {
		return SPtr<T>(ptr);
	}

	using TOutputStream = std::ostream;

} // namespace NEgo