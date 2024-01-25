#include "SuperString.hpp"
#include <cassert>

SuperString::SuperString(std::string const& string)
    : m_string(string),
      m_index(0),
      m_exhausted(false)
{
    if (m_string.empty())
    {
        m_exhausted = true;
    }
}

void SuperString::find_replace(std::string const& to_find,
                               std::string const& to_replace)
{
    if (to_find == to_replace)
    {
        // Nothing to do
        return;
    }
    std::size_t found_position = m_string.find(to_find);
    while (found_position != std::string::npos)
    {
        m_string.erase(found_position, to_find.length());
        m_string.insert(found_position, to_replace);
        found_position =
            m_string.find(to_find, found_position + to_replace.length());
    }
}

char const* SuperString::peek() const
{
    if (is_exhausted())
    {
        return nullptr;
    }
    return &m_string.at(m_index);
}

bool SuperString::test(char c) const
{
    if (is_exhausted())
    {
        return false;
    }
    return m_string.at(m_index) == c;
}

void SuperString::append_original(std::string& to)
{
    if (!is_exhausted())
    {
        to.append(1, m_string.at(m_index));
        increment_index();
    }
}

void SuperString::append_strike(std::string& to, int num_strikes)
{
    assert(num_strikes > 0);
    while ((num_strikes > 0) && (!is_exhausted()))
    {
        to.append("x");
        increment_index();
        --num_strikes;
    }
}

bool SuperString::is_exhausted() const
{
    return m_exhausted;
}

void SuperString::increment_index()
{
    ++m_index;
    if (m_index == m_string.length())
    {
        m_exhausted = true;
    }
}

std::size_t SuperString::length() const
{
    return m_string.length();
}
