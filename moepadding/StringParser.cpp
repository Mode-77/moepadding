#include "StringParser.hpp"

#include <cassert>
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

StringParser::StringParser()
    : mString(),
      mIndex(0),
      mExhausted(true)
{
}




void StringParser::load(std::string const& string) noexcept
{
        mString = string;
        mIndex = 0;
        mExhausted = true;
        if (mString.empty()) {
                return;
        }
        mExhausted = false;
        while (!mExhausted) {
                if (isWhitespace(peek())) {
                        incrementIndex();
                }
                else {
                        break;
                }
        }
}




bool StringParser::exhausted() const noexcept
{
        return mExhausted;
}




std::optional<int> StringParser::toInt() const noexcept
{
        if (mExhausted) {
                return std::nullopt;
        }

        std::string const grabbedWord = grabWord();
        std::size_t firstUnconverted = 0;
        int result = 0;
        try {
                result = std::stoi(grabbedWord, &firstUnconverted, 10);
        }
        catch (std::invalid_argument) {
                // No characters could be converted at all
                return std::nullopt;
        }
        // If not all of the input string could be converted, then it's the same
        // as an invalid argument
        if (firstUnconverted != grabbedWord.length()) {
                return std::nullopt;
        }
        return result;
}




std::string StringParser::grabWord() const noexcept
{
        assert(!isWhitespace(mString.at(mIndex)));

        std::size_t const nextWhitespace =
            mString.find_first_of(" \t\n", mIndex);

        if (nextWhitespace == std::string::npos) {
                // There was no whitespace after this word. Do a substring from
                // here to the end of the string.
                return mString.substr(mIndex, nextWhitespace);
        }
        else {
                // Do a substring from here until the first whitespace
                // (excluding the whitespace)
                return mString.substr(mIndex, nextWhitespace - mIndex);
        }
}




void StringParser::incrementIndex() noexcept
{
        ++mIndex;
        if (mIndex == mString.length()) {
                mExhausted = true;
        }
}




bool StringParser::isWhitespace(char c) const noexcept
{
        return c == ' ' || c == '\t' || c == '\n';
}




char StringParser::peek() const noexcept
{
        return mString.at(mIndex);
}
