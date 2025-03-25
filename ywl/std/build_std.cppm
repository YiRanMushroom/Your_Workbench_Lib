module;
#include <version>

#if defined(_LIBCPP_VERSION)
#include "libcxx/std.cppm.inc"
#elif defined(_MSC_VER)
#include "libstdcxx-msvc/std.cppm.inc"
#elif
#error "Unsupported std library"
#endif
