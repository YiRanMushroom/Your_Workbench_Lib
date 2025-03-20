export module ywl.basic.algorithm;

import ywl.std.prelude;
import ywl.basic.helpers;
import ywl.basic.exceptions;

// binary search
namespace ywl::basic {
    export template<std::random_access_iterator Iter,
            ywl::basic::function_like<bool(
                    const typename std::iterator_traits<Iter>::value_type&)> Predicate>
    constexpr std::optional <Iter> first_to_satisfy(Iter
    begin, Iter end, Predicate&& pred) {
    if (begin == end) {
    return std::optional <Iter>{};
}

bool first_satisfy = pred(*begin);
bool last_satisfy = pred(*(end - 1));

if (!last_satisfy) {
if (first_satisfy) {
throw ywl::basic::logic_error{"range is not in correct order"};
} else {
return std::optional <Iter>{};
}
}

--end;

while (begin < end) {
Iter mid = begin + (end - begin) / 2;
if (pred(*mid)) {
end = mid;
} else {
begin = mid + 1;
}
}

return std::optional<Iter>(end);
}

export template<std::random_access_iterator Iter,
        ywl::basic::function_like<bool(
                const typename std::iterator_traits<Iter>::value_type&)> Predicate>
constexpr std::optional <Iter> last_to_satisfy(Iter begin, Iter end, Predicate&& pred) {
    if (begin == end) {
        return std::optional <Iter>{};
    }

    bool first_satisfy = pred(*begin);
    bool last_satisfy = pred(*(end - 1));

    if (!first_satisfy) {
        if (last_satisfy) {
            throw ywl::basic::logic_error{"range is not in correct order"};
        } else {
            return std::optional <Iter>{};
        }
    }

    --end;

    while (begin < end) {
        Iter mid = begin + (end + 1 - begin) / 2;
        if (pred(*mid)) {
            begin = mid;
        } else {
            end = mid - 1;
        }
    }

    return std::optional<Iter>(begin);
}
}
