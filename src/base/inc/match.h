#pragma once

#include <string>
#include <vector>

/**
 * ObjectMatch contains a vector of expressions. ObjectMatch can then be
 * queried, via ObjectMatch.match(std::string), to check if a string matches
 * any expressions in the vector.
 *
 * Expressions in ObjectMatch take the form "<token1>.<token2>.<token3>"; a
 * series of expected tokens separated by a period. The input string takes the
 * form "<value1>.<value2>.<value3>". In this case, the input string matches
 * the expression if <value1> == <token1> && <token2> == <value2>
 * && <value3> == <token3>.  A token may be a wildcard character, "*", which
 * will match to any value in that position (inclusive of no value at that
 * location).
 */
class ObjectMatch
{
  protected:
    std::vector<std::vector<std::string> > tokens;
    bool domatch(const std::string &name) const;

  public:
    ObjectMatch();
    ObjectMatch(const std::string &expression);
    void add(const ObjectMatch &other);
    void setExpression(const std::string &expression);
    void setExpression(const std::vector<std::string> &expression);
    std::vector<std::vector<std::string> > getExpressions();
    bool match(const std::string &name) const
    {
        return tokens.empty() ? false : domatch(name);
    }
};

