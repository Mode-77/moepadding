#pragma once

#include <cstdio>
#include <optional>
#include <string>

#include "StringParser.hpp"

class File final {
    public:
        /// @brief Create a new file instance given a filename
        /// @details On creation, the index shall point to the first word. If
        /// there are no words in the file, it will be exhausted.
        /// @throw std::logic_error if the file could not be opened for reading
        /// @param filename
        File(std::string const& filename);

        /// @return True if there are no words in the file. False otherwise.
        [[nodiscard]] bool exhausted() const noexcept;

        /// @brief Attempts to parse the word pointed to by the index as an
        /// integer
        /// @throw std::invalid_argument if the conversion failed
        /// @return If the conversion was successful, the optional value will
        /// contain the parsed integer. Otherwise, the optional will have no
        /// value.
        [[nodiscard]] std::optional<int> readAsInt() const noexcept;

    private:
        [[nodiscard]] static std::FILE* openFile(char const* filename,
                                                 char const* mode) noexcept;

        [[nodiscard]] static std::optional<std::string> readFile(
            char const* filename,
            char const* mode) noexcept;

        StringParser mParser;
};
