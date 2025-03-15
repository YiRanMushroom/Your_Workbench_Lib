module;

#include <cstdint>

export module ywl.prelude;

export import ywl.all;
export import ywl.std.prelude;

export using ywl::util::print_ln;
export using ywl::util::printf_ln;

export using ywl::util::err_print_ln;
export using ywl::util::err_printf_ln;

export using size_t = size_t;
export using uint8_t = uint8_t;
export using uint16_t = uint16_t;
export using uint32_t = uint32_t;
export using uint64_t = uint64_t;
export using int8_t = int8_t;
export using int16_t = int16_t;
export using int32_t = int32_t;
export using int64_t = int64_t;

export using nullptr_t = nullptr_t;