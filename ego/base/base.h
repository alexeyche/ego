#pragma once


#include <memory>
#include <string>


template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using SPtr = std::shared_ptr<T>;


using TString = std::string;

#define JOIN(X, Y) X##Y

#define GENERATE_UNIQUE_ID(N) JOIN(N, __COUNTER__)

