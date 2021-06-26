#include "base/inc/str.h"

#include <string>
#include <vector>

using namespace std;

bool
split_first(const string &s, string &lhs, string &rhs, char c)
{
    string::size_type offset = s.find(c);
    if (offset == string::npos) {
        lhs = s;
        rhs = "";
        return false;
    }

    lhs = s.substr(0, offset);
    rhs = s.substr(offset + 1);
    return true;
}

bool
split_last(const string &s, string &lhs, string &rhs, char c)
{
    string::size_type offset = s.rfind(c);
    if (offset == string::npos) {
        lhs = s;
        rhs = "";
        return false;
    }

    lhs = s.substr(0, offset);
    rhs = s.substr(offset + 1);
    return true;
}

void
tokenize(vector<string>& v, const string &s, char token, bool ignore)
{
    string::size_type first = 0;
    string::size_type last = s.find_first_of(token);

    if (s.empty())
        return;

    if (ignore && last == first) {
        while (last == first)
            last = s.find_first_of(token, ++first);

        if (last == string::npos) {
            if (first != s.size())
                v.push_back(s.substr(first));
            return;
        }
    }

    while (last != string::npos) {
        v.push_back(s.substr(first, last - first));

        if (ignore) {
            first = s.find_first_not_of(token, last + 1);

            if (first == string::npos)
                return;
        } else
            first = last + 1;

        last = s.find_first_of(token, first);
    }

    v.push_back(s.substr(first));
}
