#include "StringMarkers.hpp"
#include <cstdlib>
#include <list>
#include <vector>

StringMarkers::StringMarkers(std::vector<std::size_t> const& v)
    : m_markers()
{
        for (std::size_t i : v) {
                m_markers.push_back(i);
        }
}




std::size_t StringMarkers::peek() const
{
        return m_markers.front();
}




void StringMarkers::pop()
{
        m_markers.pop_front();
}




void StringMarkers::shift(int delta)
{
        for (std::size_t& m : m_markers) {
                m += delta;
        }
}




bool StringMarkers::empty() const
{
        return m_markers.empty();
}
