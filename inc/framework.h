#ifndef ARPCTL_FRAMEWORK_H
#define ARPCTL_FRAMEWORK_H

#ifndef __GNUG__
#warning Application must be compiled with GNU/C++-compatible-compilers
#endif

/* POSIX */
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

/* CONCEPTS */
#include <stdexcept>
#include <stdfloat>

/* STL */
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <deque>
#include <exception>
#include <execution>
#include <expected>
#include <filesystem>
#include <format>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <semaphore>
#include <set>
#include <thread>
#include <tuple>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

/* PCAP */
#include <pcap.h>

/* INTRINSICS */
#include "intrin/intrin.h"

/* LOGGING */
#include "log.h"

template<typename T, typename... P>
T* typed_alloc(P... args)
{
	TRACE_S;
	return new T(args...);
}

template<typename T>
T* typed_alloc_v(size_t n)
{
	TRACE_S;
	return new T[n];
}

#endif //ARPCTL_FRAMEWORK_H
