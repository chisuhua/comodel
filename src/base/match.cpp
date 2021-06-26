#include "base/inc/match.h"

#include "base/inc/str.h"

using namespace std;

ObjectMatch::ObjectMatch()
{
}

ObjectMatch::ObjectMatch(const string &expr)
{
    setExpression(expr);
}

void
ObjectMatch::add(const ObjectMatch &other)
{
    tokens.insert(tokens.end(), other.tokens.begin(), other.tokens.end());
}

void
ObjectMatch::setExpression(const string &expr)
{
    tokens.resize(1);
    tokenize(tokens[0], expr, '.');
}

void
ObjectMatch::setExpression(const vector<string> &expr)
{
    if (expr.empty()) {
        tokens.resize(0);
    } else {
        tokens.resize(expr.size());
        for (vector<string>::size_type i = 0; i < expr.size(); ++i)
            tokenize(tokens[i], expr[i], '.');
    }
}

/**
 * @todo this should probably be changed to just use regular
 * expression code
 */
bool
ObjectMatch::domatch(const string &name) const
{
    vector<string> name_tokens;
    tokenize(name_tokens, name, '.');
    int ntsize = name_tokens.size();

    int num_expr = tokens.size();
    for (int i = 0; i < num_expr; ++i) {
        const vector<string> &token = tokens[i];
        int jstop = token.size();

        bool match = true;
        for (int j = 0; j < jstop; ++j) {
            if (j >= ntsize)
                break;

            const string &var = token[j];
            if (var != "*" && var != name_tokens[j]) {
                match = false;
                break;
            }
        }

        if (match)
            return true;
    }

    return false;
}

std::vector<std::vector<std::string> >
ObjectMatch::getExpressions()
{
    std::vector<std::vector<std::string> > to_return;
    for (const auto &expression: tokens) {
        std::vector<std::string> to_add;
        to_add.insert(to_add.end(), expression.begin(), expression.end());
        to_return.push_back(to_add);
    }

    return to_return;
}

