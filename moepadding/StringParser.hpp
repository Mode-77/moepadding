#pragma once

#include <optional>
#include <string>

class StringParser final {
    public:
        /// @brief Create an exhausted StringParser
        StringParser(); // @Todo Can it throw?

        /// @return True if there are no words to be parsed. False otherwise.
        [[nodiscard]] bool exhausted() const noexcept;

        /// @details Loads the parser with a copy of a string and jumps to the
        /// first word in the string. If there are no words in the string to be
        /// parsed, the parser is exhausted.
        void load(std::string const& string) noexcept;

        /// @brief Attempts to parse the word pointed to by the index as an
        /// integer
        /// @throw std::invalid_argument if the conversion failed
        /// @return Converted value if the conversion succeeded
        [[nodiscard]] std::optional<int> toInt() const noexcept;

    private:
        [[nodiscard]] bool isWhitespace(char c) const noexcept;

        /// @return The character at the index
        [[nodiscard]] char peek() const noexcept;

        /// @pre Must not be exhausted
        [[nodiscard]] std::string grabWord() const noexcept;

        void incrementIndex() noexcept;

        std::string mString;
        int mIndex;
        bool mExhausted;
};
