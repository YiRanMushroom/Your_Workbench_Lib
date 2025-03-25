module;
#include <version>

// libc++
#if defined(_LIBCPP_VERSION)
#include "libcxx/std.compat.inc"
#elif defined(_MSC_VER)
#include "libstdcxx-msvc/std.compat.inc"
#elif
#error "Unsupported std library"
#endif
