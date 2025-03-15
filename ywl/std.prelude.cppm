module;
#include <utility>
#include <tuple>

#include <vector>
#include <string>
#include <array>
#include <memory>
#include <functional>
#include <list>

#include <queue>
#include <stack>
#include <deque>

#include <algorithm>

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

export module ywl.std.prelude;

export namespace std {
    using std::move;
    using std::forward;
    using std::pair;
    using std::make_pair;
    using std::tuple;
    using std::make_tuple;
    using std::get;
    using std::forward_as_tuple;
    using std::tie;

    using std::swap;
    using std::exchange;
    using std::vector;
    using std::string;
    using std::to_string;
    using std::array;

    using std::unique_ptr;
    using std::make_unique;
    using std::shared_ptr;
    using std::make_shared;
    using std::weak_ptr;

    using std::function;
    using std::move_only_function;
    using std::bind;

    using std::list;

    using std::deque;
    using std::queue;
    using std::priority_queue;
    using std::stack;

    using std::sort;
    using std::min;
    using std::max;
    using std::find;

    using std::unordered_map;
    using std::unordered_multimap;
    using std::unordered_set;
    using std::unordered_multiset;
    using std::map;
    using std::multimap;
    using std::set;
    using std::multiset;
}