module;

#define _LIBCPP_HAS_LOCALIZATION 1
#define _LIBCPP_HAS_ATOMIC_HEADER 1
#define _LIBCPP_HAS_RANDOM_DEVICE 1
#define _LIBCPP_HAS_THREADS 1

// if c++ 23 standard
#if __cplusplus >= 202302L
#define _LIBCPP_STD_VER 23
#else
#define _LIBCPP_STD_VER 20
#endif

// The headers of Table 24: C++ library headers [tab:headers.cpp]
// and the headers of Table 25: C++ headers for C library facilities [tab:headers.cpp.c]
#include <algorithm>
#include <any>
#include <array>
#if _LIBCPP_HAS_ATOMIC_HEADER
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
#if _LIBCPP_HAS_LOCALIZATION
#  include <clocale>
#endif
#include <cmath>
#if _LIBCPP_HAS_LOCALIZATION
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
#include <format>
#include <forward_list>
#if _LIBCPP_HAS_LOCALIZATION
#  include <fstream>
#endif
#include <functional>
#include <future>
#include <initializer_list>
#if _LIBCPP_HAS_LOCALIZATION
#  include <iomanip>
#endif
#if _LIBCPP_HAS_LOCALIZATION
#  include <ios>
#endif
#include <iosfwd>
#if _LIBCPP_HAS_LOCALIZATION
#  include <iostream>
#endif
#if _LIBCPP_HAS_LOCALIZATION
#  include <istream>
#endif
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#if _LIBCPP_HAS_LOCALIZATION
#  include <locale>
#endif
#include <map>
#include <mdspan>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#if _LIBCPP_HAS_LOCALIZATION
#  include <ostream>
#endif
#include <print>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#if _LIBCPP_HAS_LOCALIZATION
#  include <regex>
#endif
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#if _LIBCPP_HAS_LOCALIZATION
#  include <sstream>
#endif
#include <stack>
#include <stdexcept>
#include <stop_token>
#if _LIBCPP_HAS_LOCALIZATION
#  include <streambuf>
#endif
#include <string>
#include <string_view>
#if _LIBCPP_HAS_LOCALIZATION
#  include <strstream>
#endif
#if _LIBCPP_HAS_LOCALIZATION
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
#include <version>

// *** Headers not yet available ***
//
// This validation is mainly to catch when a new header is added but adding the
// corresponding .inc file is forgotten. However, the check based on __has_include
// alone doesn't work on Windows because the Windows SDK is on the include path,
// and that means the MSVC STL headers can be found as well, tricking __has_include
// into thinking that libc++ provides the header.
//
#ifndef _WIN32
#  if __has_include(<debugging>)
#    error "please update the header information for <debugging> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<debugging>)
#  if __has_include(<flat_set>)
#    error "please update the header information for <flat_set> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<flat_set>)
#  if __has_include(<generator>)
#    error "please update the header information for <generator> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<generator>)
#  if __has_include(<hazard_pointer>)
#    error "please update the header information for <hazard_pointer> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<hazard_pointer>)
#  if __has_include(<inplace_vector>)
#    error "please update the header information for <inplace_vector> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<inplace_vector>)
#  if __has_include(<linalg>)
#    error "please update the header information for <linalg> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<linalg>)
#  if __has_include(<rcu>)
#    error "please update the header information for <rcu> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<rcu>)
#  if __has_include(<spanstream>)
#    error "please update the header information for <spanstream> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<spanstream>)
#  if __has_include(<stacktrace>)
#    error "please update the header information for <stacktrace> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<stacktrace>)
#  if __has_include(<stdfloat>)
#    error "please update the header information for <stdfloat> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<stdfloat>)
#  if __has_include(<text_encoding>)
#    error "please update the header information for <text_encoding> in headers_not_available in utils/libcxx/header_information.py"
#  endif // __has_include(<text_encoding>)
#endif // _WIN32

// addtional
#include <stacktrace>

export module ywl.std.prelude;

#define _LIBCPP_USING_IF_EXISTS

#include "std/algorithm.inc"
#include "std/any.inc"
#include "std/array.inc"
#include "std/atomic.inc"
#include "std/barrier.inc"
#include "std/bit.inc"
#include "std/bitset.inc"
#include "std/cassert.inc"
#include "std/cctype.inc"
#include "std/cerrno.inc"
#include "std/cfenv.inc"
#include "std/cfloat.inc"
#include "std/charconv.inc"
#include "std/chrono.inc"
#include "std/cinttypes.inc"
#include "std/climits.inc"
#include "std/clocale.inc"
#include "std/cmath.inc"
#include "std/codecvt.inc"
#include "std/complex.inc"
#include "std/condition_variable.inc"
#include "std/coroutine.inc"
#include "std/csetjmp.inc"
#include "std/csignal.inc"
#include "std/cstdarg.inc"
#include "std/cstddef.inc"
#include "std/cstdint.inc"
#include "std/cstdio.inc"
#include "std/cstring.inc"
#include "std/cuchar.inc"
#include "std/cwchar.inc"
#include "std/cwctype.inc"
#include "std/deque.inc"
#include "std/exception.inc"
#include "std/execution.inc"
#include "std/expected.inc"
#include "std/filesystem.inc"
#include "std/flat_set.inc"
#include "std/forward_list.inc"
#include "std/fstream.inc"
#include "std/functional.inc"
#include "std/future.inc"
#include "std/generator.inc"
#include "std/hazard_pointer.inc"
#include "std/initializer_list.inc"
#include "std/iomanip.inc"
#include "std/ios.inc"
#include "std/iosfwd.inc"
#include "std/iostream.inc"
#include "std/istream.inc"
#include "std/latch.inc"
#include "std/limits.inc"
#include "std/list.inc"
#include "std/locale.inc"
#include "std/map.inc"
#include "std/mdspan.inc"
#include "std/memory.inc"
#include "std/memory_resource.inc"
#include "std/mutex.inc"
#include "std/new.inc"
#include "std/numbers.inc"
#include "std/numeric.inc"
#include "std/optional.inc"
#include "std/ostream.inc"
#include "std/print.inc"
#include "std/queue.inc"
#include "std/random.inc"

#include "std/ratio.inc"
#include "std/rcu.inc"
#include "std/regex.inc"
#include "std/scoped_allocator.inc"
#include "std/semaphore.inc"
#include "std/set.inc"
#include "std/shared_mutex.inc"
#include "std/source_location.inc"
#include "std/span.inc"
#include "std/spanstream.inc"
#include "std/sstream.inc"
#include "std/stack.inc"

#include "std/stdexcept.inc"
#include "std/stdfloat.inc"
#include "std/stop_token.inc"
#include "std/streambuf.inc"
#include "std/string.inc"
#include "std/string_view.inc"
#include "std/strstream.inc"
#include "std/syncstream.inc"
#include "std/system_error.inc"
#include "std/text_encoding.inc"
#include "std/thread.inc"
#include "std/tuple.inc"
#include "std/typeindex.inc"
#include "std/typeinfo.inc"
#include "std/unordered_map.inc"
#include "std/unordered_set.inc"
#include "std/utility.inc"
#include "std/valarray.inc"
#include "std/variant.inc"
#include "std/vector.inc"
#include "std/version.inc"

// std compact
#include "std.compat/cassert.inc"
#include "std.compat/cctype.inc"
#include "std.compat/cerrno.inc"
#include "std.compat/cfenv.inc"
#include "std.compat/cfloat.inc"
#include "std.compat/cinttypes.inc"
#include "std.compat/climits.inc"
#include "std.compat/clocale.inc"
#include "std.compat/cmath.inc"
#include "std.compat/csetjmp.inc"
#include "std.compat/csignal.inc"
#include "std.compat/cstdarg.inc"
#include "std.compat/cstddef.inc"
#include "std.compat/cstdint.inc"
#include "std.compat/cstdio.inc"

#include "std.compat/cstring.inc"

#include "std.compat/cuchar.inc"
#include "std.compat/cwchar.inc"
#include "std.compat/cwctype.inc"

// #include "std/stacktrace.inc"
#include "std/stacktrace_modified.inc"

// #include "std/concepts.inc"
#include "std/concepts_modified.inc"

//#include "std/iterator.inc"
#include "std/iterator_modified.inc"

//#include "std/ranges.inc"
#include "std/ranges_modified.inc"

//#include "std.compat/cstdlib.inc"
#include "std.compat/cstdlib_modified.inc"

//#include "std/cstdlib.inc"
#include "std/cstdlib_modified.inc"

//#include "std.compat/ctime.inc"
#include "std.compat/ctime_modified.inc"

//#include "std/compare.inc"
#include "std/compare_modified.inc"

//#include "std/ctime.inc"
#include "std/ctime_modified.inc"

// #include "std/format.inc"
#include "std/format_modified.inc"

// #include "std/type_traits.inc"
#include "std/type_traits_modified.inc"

//#include "std/flat_map.inc"