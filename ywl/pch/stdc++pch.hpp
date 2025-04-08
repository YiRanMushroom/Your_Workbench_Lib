#pragma once

#include <algorithm>
#include <any>
#include <array>
#if __has_include(<atomic>)
#  include <atomic>
#endif
#include <barrier>
#include <bit>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <charconv>
#include <chrono>
#include <cinttypes>
#include <climits>
#if __has_include(<clocale>)
#  include <clocale>
#endif
#include <cmath>
#if __has_include(<codecvt>)
#  include <codecvt>
#endif
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <exception>
#include <execution>
#include <expected>
#include <filesystem>
#if __has_include(<flat_map>)
#include <flat_map>
#endif
#include <format>
#include <forward_list>
#if __has_include(<fstream>)
#  include <fstream>
#endif
#include <functional>
#include <future>
#include <initializer_list>
#if __has_include(<iomanip>)
#  include <iomanip>
#endif
#if __has_include(<ios>)
#  include <ios>
#endif
#include <iosfwd>
#if __has_include(<iostream>)
#  include <iostream>
#endif
#if __has_include(<istream>)
#  include <istream>
#endif
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#if __has_include(<locale>)
#  include <locale>
#endif
#include <map>
#if __has_include(<mdspan>)
#include <mdspan>
#endif
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#if __has_include(<ostream>)
#  include <ostream>
#endif
#if __has_include(<print>)
#include <print>
#endif
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#if __has_include(<regex>)
#  include <regex>
#endif
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#if __has_include(<sstream>)
#  include <sstream>
#endif
#include <stack>
#if __has_include(<stacktrace>)
#include <stacktrace>
#endif
#include <stdexcept>
#include <stop_token>
#if __has_include(<streambuf>)
#  include <streambuf>
#endif
#include <string>
#include <string_view>
#if __has_include(<strstream>)
#  include <strstream>
#endif
#if __has_include(<syncstream>)
#  include <syncstream>
#endif
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

#if __has_include(<spanstream>)
#include <spanstream>
#endif

#if __has_include(<stdfloat>)
#include <stdfloat>
#endif

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#if __has_include(<clocale>)
#  include <clocale>
#endif
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>