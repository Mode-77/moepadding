#pragma once

#include <list>
#include <string>
#include <vector>

class SuperString final {
    public:
        SuperString(std::string const& string);

        void find_replace(std::string const& to_find,
                          std::string const& to_replace);

        [[nodiscard]] char const* peek() const;

        [[nodiscard]] bool test(char c) const;

        void append_original(std::string& to);

        void append_strike(std::string& to, int num_strikes);

        [[nodiscard]] bool is_exhausted() const;

        [[nodiscard]] std::size_t length() const;

    private:
        std::string m_string;
        int m_index;
        bool m_exhausted;

        void increment_index();
};
