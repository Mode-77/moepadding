#pragma once

#include <cstdlib>
#include <list>
#include <vector>

class StringMarkers final {
    public:
        StringMarkers(std::vector<std::size_t> const& v);

        [[nodiscard]] std::size_t peek() const;

        void pop();

        void shift(int delta);

        [[nodiscard]] bool empty() const;

    private:
        std::list<std::size_t> m_markers;
};
