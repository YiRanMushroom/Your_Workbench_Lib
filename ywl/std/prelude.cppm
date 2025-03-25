export module ywl.std.prelude;
export import build_std.compat;
export import ywl.std.ctime_help;

export {
    using std::ctime;
    using std::difftime;
    using std::gmtime;
    using std::localtime;
    using std::mktime;
    using std::time;
    using std::timespec_get;
}