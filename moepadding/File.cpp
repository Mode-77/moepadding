#include "File.hpp"
#include "StringParser.hpp"

#include <cassert>
#include <cstdio>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string>

File::File(std::string const& filename)
    : mParser()
{
        std::optional<std::string> fileContents;
        fileContents = readFile(filename.c_str(), "r");
        if (fileContents.has_value()) {
                mParser.load(fileContents.value());
        }
        else {
                std::string errorMessage("Cannot open \"");
                errorMessage.append(filename);
                errorMessage.append("\"");
                throw std::logic_error(errorMessage);
        }
}




bool File::exhausted() const noexcept
{
        return mParser.exhausted();
}




std::optional<int> File::readAsInt() const noexcept
{
        return mParser.toInt();
}




std::FILE* File::openFile(char const* filename, char const* mode) noexcept
{
        std::FILE* file = nullptr;
        // @Todo How long does this take?
        fopen_s(&file, filename, mode);
        return file;
}




std::optional<std::string> File::readFile(char const* filename,
                                          char const* mode) noexcept
{
        std::FILE* file = openFile(filename, mode);
        if (file == nullptr) {
                return std::nullopt;
        }
        std::string fileText;
        int character = std::fgetc(file);
        while (character != EOF) {
                fileText += character;
                character = std::fgetc(file);
        }
        fclose(file);
        return fileText;
}
