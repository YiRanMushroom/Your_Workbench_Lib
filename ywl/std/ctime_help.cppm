module;
#include <version>

#if defined(_LIBCPP_VERSION)
#elif defined(_MSC_VER)
#include <yvals_core.h>
#include <time.h>
#else
#error("Unsupported std library")
#endif

export module ywl.std.ctime_help;

#if defined(_MSC_VER)
export namespace std {
    _EXPORT_STD template <int = 0>
    _Check_return_ _CRT_INSECURE_DEPRECATE(ctime_s) inline char* __CRTDECL ctime(_In_ const time_t* const _Time) noexcept
    /* strengthened */ {
        return _CSTD _ctime64(_Time);
    }

    _EXPORT_STD template <int = 0>
    _Check_return_ inline double __CRTDECL difftime(_In_ const time_t _Time1, _In_ const time_t _Time2) noexcept
    /* strengthened */ {
        return _CSTD _difftime64(_Time1, _Time2);
    }

    _EXPORT_STD template <int = 0>
    _Check_return_ _CRT_INSECURE_DEPRECATE(gmtime_s) inline tm* __CRTDECL gmtime(_In_ const time_t* const _Time) noexcept
    /* strengthened */ {
        return _CSTD _gmtime64(_Time);
    }

    _EXPORT_STD template <int = 0>
    _CRT_INSECURE_DEPRECATE(localtime_s)
    inline tm* __CRTDECL localtime(_In_ const time_t* const _Time) noexcept /* strengthened */ {
        return _CSTD _localtime64(_Time);
    }

    _EXPORT_STD template <int = 0>
    _Check_return_opt_ inline time_t __CRTDECL mktime(_Inout_ tm* const _Tm) noexcept /* strengthened */ {
        return _CSTD _mktime64(_Tm);
    }

    _EXPORT_STD template <int = 0>
    inline time_t __CRTDECL time(_Out_opt_ time_t* const _Time) noexcept /* strengthened */ {
        return _CSTD _time64(_Time);
    }

    _EXPORT_STD template <int = 0>
    _Check_return_ inline int __CRTDECL timespec_get(_Out_ timespec* const _Ts, _In_ const int _Base) noexcept
     {
        return _CSTD _timespec64_get(reinterpret_cast<_timespec64*>(_Ts), _Base);
    }
}
#endif
