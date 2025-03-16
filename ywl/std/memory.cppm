module;

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

export module ywl.std.memory;

// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

export namespace std {
    // [pointer.traits], pointer traits
    using std::pointer_traits;

    // [pointer.conversion], pointer conversion
    using std::to_address;

    // [ptr.align], pointer alignment
    using std::align;
    using std::assume_aligned;

    // [obj.lifetime], explicit lifetime management
    //  using std::start_lifetime_as;
    //  using std::start_lifetime_as_array;

    // [allocator.tag], allocator argument tag
    using std::allocator_arg;
    using std::allocator_arg_t;

    // [allocator.uses], uses_allocator
    using std::uses_allocator;

    // [allocator.uses.trait], uses_allocator
    using std::uses_allocator_v;

    // [allocator.uses.construction], uses-allocator construction
    using std::uses_allocator_construction_args;

    using std::make_obj_using_allocator;
    using std::uninitialized_construct_using_allocator;

    // [allocator.traits], allocator traits
    using std::allocator_traits;

#if _LIBCPP_STD_VER >= 23
    using std::allocation_result;
#endif

    // [default.allocator], the default allocator
    using std::allocator;
    using std::operator==;

    // [specialized.addressof], addressof
    using std::addressof;

    // [specialized.algorithms], specialized algorithms
    // [special.mem.concepts], special memory concepts

    using std::uninitialized_default_construct;
    using std::uninitialized_default_construct_n;

    namespace ranges {
        using std::ranges::uninitialized_default_construct;
        using std::ranges::uninitialized_default_construct_n;
    } // namespace ranges

    using std::uninitialized_value_construct;
    using std::uninitialized_value_construct_n;

    namespace ranges {
        using std::ranges::uninitialized_value_construct;
        using std::ranges::uninitialized_value_construct_n;
    } // namespace ranges

    using std::uninitialized_copy;
    using std::uninitialized_copy_n;

    namespace ranges {
        using std::ranges::uninitialized_copy;
        using std::ranges::uninitialized_copy_result;

        using std::ranges::uninitialized_copy_n;
        using std::ranges::uninitialized_copy_n_result;
    } // namespace ranges

    using std::uninitialized_move;
    using std::uninitialized_move_n;

    namespace ranges {
        using std::ranges::uninitialized_move;
        using std::ranges::uninitialized_move_result;

        using std::ranges::uninitialized_move_n;
        using std::ranges::uninitialized_move_n_result;
    } // namespace ranges

    using std::uninitialized_fill;
    using std::uninitialized_fill_n;

    namespace ranges {
        using std::ranges::uninitialized_fill;
        using std::ranges::uninitialized_fill_n;
    } // namespace ranges

    // [specialized.construct], construct_at
    using std::construct_at;

    namespace ranges {
        using std::ranges::construct_at;
    }
    // [specialized.destroy], destroy
    using std::destroy;
    using std::destroy_at;
    using std::destroy_n;

    namespace ranges {
        using std::ranges::destroy;
        using std::ranges::destroy_at;
        using std::ranges::destroy_n;
    } // namespace ranges

    // [unique.ptr], class template unique_ptr
    using std::default_delete;
    using std::unique_ptr;

    using std::make_unique;
    using std::make_unique_for_overwrite;

    using std::operator<;
    using std::operator>;
    using std::operator<=;
    using std::operator>=;
    using std::operator<=>;

    using std::operator<<;

    // [util.smartptr.weak.bad], class bad_weak_ptr
    using std::bad_weak_ptr;

    // [util.smartptr.shared], class template shared_ptr
    using std::shared_ptr;

    // [util.smartptr.shared.create], shared_ptr creation
    using std::allocate_shared;
    using std::allocate_shared_for_overwrite;
    using std::make_shared;
    using std::make_shared_for_overwrite;

    // [util.smartptr.shared.spec], shared_ptr specialized algorithms
    using std::swap;

    // [util.smartptr.shared.cast], shared_ptr casts
    using std::const_pointer_cast;
    using std::dynamic_pointer_cast;
    using std::reinterpret_pointer_cast;
    using std::static_pointer_cast;

#if _LIBCPP_HAS_RTTI
    using std::get_deleter;
#endif // _LIBCPP_HAS_RTTI

    // [util.smartptr.shared.io], shared_ptr I/O

    // [util.smartptr.weak], class template weak_ptr
    using std::weak_ptr;

    // [util.smartptr.weak.spec], weak_ptr specialized algorithms

    // [util.smartptr.ownerless], class template owner_less
    using std::owner_less;

    // [util.smartptr.enab], class template enable_shared_from_this
    using std::enable_shared_from_this;

    // [util.smartptr.hash], hash support
    using std::hash;

    // [util.smartptr.atomic], atomic smart pointers
    // using std::atomic;

#if _LIBCPP_STD_VER >= 23
    // [out.ptr.t], class template out_ptr_t
  using std::out_ptr_t;

  // [out.ptr], function template out_ptr
  using std::out_ptr;

  // [inout.ptr.t], class template inout_ptr_t
  using std::inout_ptr_t;

  // [inout.ptr], function template inout_ptr
  using std::inout_ptr;
#endif // _LIBCPP_STD_VER >= 23

#if _LIBCPP_HAS_THREADS
    // [depr.util.smartptr.shared.atomic]
  using std::atomic_is_lock_free;

  using std::atomic_load;
  using std::atomic_load_explicit;

  using std::atomic_store;
  using std::atomic_store_explicit;

  using std::atomic_exchange;
  using std::atomic_exchange_explicit;

  using std::atomic_compare_exchange_strong;
  using std::atomic_compare_exchange_strong_explicit;
  using std::atomic_compare_exchange_weak;
  using std::atomic_compare_exchange_weak_explicit;
#endif // _LIBCPP_HAS_THREADS
} // namespace std
