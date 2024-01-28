#include "StringMarkers.hpp"
#include "SuperString.hpp"
#include <cassert>
#include <stack>
#include <string>

bool isWhitespace(int character)
{
        return character == ' ' || character == '\t' || character == '\r' ||
               character == '\n';
}




void doubleQuoteContext(SuperString& from, std::string& to)
{
        from.append_strike(to, 1);
        while (!from.is_exhausted()) {
                if (from.test('"')) // Matching double quote. We're done.
                {
                        from.append_strike(to, 1);
                        return;
                }
                else if (from.test('\\')) {
                        from.append_strike(
                            to, 1); // Consume the escape sequence character
                        from.append_strike(to,
                                           1); // Can only be one character here
                }
                else {
                        from.append_strike(to, 1);
                }
        }
}




void singleQuoteContext(SuperString& from, std::string& to)
{
        from.append_strike(to, 1);
        while (!from.is_exhausted()) {
                if (from.test('\'')) // Matching single quote. We're done.
                {
                        from.append_strike(to, 1);
                        return;
                }
                else if (from.test('\\')) {
                        from.append_strike(
                            to, 1); // Consume the escape sequence character
                        from.append_strike(to,
                                           1); // Can only be one character here
                }
                else {
                        from.append_strike(to, 1);
                }
        }
}




void singleLineComment(SuperString& from, std::string& to)
{
        // After the slash here
        int state = 0;
        while (!from.is_exhausted()) {
                switch (state) {
                case 0: {
                        if (from.test('\r')) {
                                from.append_original(to);
                                state = 1;
                        }
                        else
                                state = 2;
                } break;

                case 1: {
                        if (from.test('\n')) {
                                from.append_original(to);
                                return;
                        }
                        else
                                state = 2;
                } break;

                case 2: {
                        if (isWhitespace(*(from.peek()))) {
                                from.append_original(to);
                        }
                        else {
                                from.append_strike(to, 1);
                        }
                        state = 0;
                }
                }
        }
}




void multilineCommentContext(SuperString& from, std::string& to)
{
        // After the star here
        int state = 0;
        while (!from.is_exhausted()) {
                switch (state) {
                case 0: {
                        if (from.test('*')) {
                                from.append_strike(to, 1);
                                state = 1;
                        }
                        else
                                state = 2;
                } break;

                case 1: {
                        if (from.test('/')) {
                                from.append_strike(to, 1);
                                return;
                        }
                        else
                                state = 2;
                } break;

                case 2: {
                        if (isWhitespace(*(from.peek()))) {
                                from.append_original(to);
                        }
                        else {
                                from.append_strike(to, 1);
                        }
                        state = 0;
                }
                }
        }
}




void forwardSlashTree(SuperString& from, std::string& to)
{
        /* We're at a forward slash, so we could be at a multiline comment,
        single-line comment, or something else */

        // Go ahead and strike out the first slash
        from.append_strike(to, 1);
        if (from.test('*')) // Multiline comment
        {
                from.append_strike(to, 1); // Strike out the star
                multilineCommentContext(from, to);
        }
        else if (from.test('/')) // Single-line comment
        {
                from.append_strike(to, 1);
                singleLineComment(from, to);
        }
        // If not any of the above, what else could it be?
}




std::string siftCriticalCharacters(std::string const& s)
{
        SuperString superString(s);
        std::string result;
        int i = 0;
        while (!superString.is_exhausted()) {
                char const& c = *(superString.peek());
                if (isWhitespace(c)) {
                        superString.append_original(result);
                }

                else if (c == '/') {
                        // Could be a multiline comment or single-line comment
                        forwardSlashTree(superString, result);
                }

                else if (c == '\'') {
                        // Enter single quote context to cross out characters
                        // that would interfere with parsing
                        singleQuoteContext(superString, result);
                }

                else if (c == '"') {
                        doubleQuoteContext(superString, result);
                }

                else if (c == '{' || c == '}' || c == ';') {
                        // Leave these unfiltered in the result for the next
                        // filter stage
                        superString.append_original(result);
                }

                else {
                        superString.append_strike(result, 1);
                }
        }
        assert(result.length() == superString.length());
        return result;
}




std::string siftOuterScopes(std::string const& s)
{
        std::string result;
        std::stack<char> braceStack;
        for (char const& c : s) {
                assert(isWhitespace(c) || c == '{' || c == '}' || c == ';' ||
                       c == 'x');

                if (c == '{') {
                        if (braceStack.empty()) {
                                result.append(1, c);
                        }
                        else {
                                result.append("x");
                        }
                        braceStack.push(c);
                }
                else if (c == '}') {
                        assert(!braceStack.empty());
                        braceStack.pop();
                        if (braceStack.empty()) {
                                result.append(1, c);
                        }
                        else {
                                result.append("x");
                        }
                }
                else if (c == ';') {
                        if (braceStack.empty()) {
                                result.append(1, c);
                        }
                        else {
                                result.append("x");
                        }
                }
                else {
                        result.append(1, c);
                }
        }
        assert(result.length() == s.length());
        return result;
}




StringMarkers findScopes(std::string const& s)
{
        std::vector<std::size_t> result;
        int state = 0;
        for (std::size_t i = 0; i < s.length(); ++i) {
                char const& c = s.at(i);

                switch (state) {
                case 0: {
                        if (c == '{') {
                                state = 1;
                        }
                } break;

                case 1: {
                        if (c == '}') {
                                result.push_back(i + 1);
                                state = 2;
                        }
                } break;

                case 2: {
                        if (c == '{') {
                                state = 1;
                        }
                        else if (c == 'x') {
                                state = 0;
                        }
                        else if (c == ';') {
                                result.back() = i + 1;
                                state = 0;
                        }
                }
                }
        }
        return StringMarkers(result);
}




std::size_t occurrences(std::string const& s, std::string const& to_find)
{
        assert(!to_find.empty());
        if (s.empty()) {
                return 0;
        }
        std::size_t result = 0;
        std::size_t pos = s.find(to_find);
        while ((pos != std::string::npos)) {
                ++result;
                pos = s.find(to_find, pos + to_find.length());
        }
        return result;
}




void erase_occurrences(std::string& s, std::string const& to_find, int n)
{
        assert(n >= 0);
        if (n == 0) {
                return;
        }
        std::size_t found_at = s.find(to_find);
        while ((found_at != std::string::npos)) {
                s.erase(found_at, to_find.length());
                if ((--n) == 0) {
                        return;
                }
                found_at = s.find(to_find, found_at);
        }
}




void insert_copies(std::string& s, std::string const& to_insert, int n)
{
        assert(n >= 0);
        if (n == 0) {
                return;
        }
        for (; n > 0; --n) {
                s.insert(0, to_insert);
        }
}




void calculatePadding(std::string& input,
                      std::string sifted,
                      StringMarkers stringMarkers,
                      int wantPadding)
{
        while (!stringMarkers.empty()) {
                std::size_t const marker = stringMarkers.peek();

                std::size_t const first_x_at = sifted.find('x', marker);

                if (first_x_at == std::string::npos) {
                        return;
                }

                std::string const initial_window =
                    input.substr(marker, first_x_at - marker);

                std::string final_window = initial_window;

                std::size_t const newlines =
                    occurrences(initial_window, "\r\n");
                assert(newlines > 0);

                std::size_t const actual_padding = newlines - 1;

                int deltaPadding = wantPadding - actual_padding;
                if (deltaPadding == 0) {
                        stringMarkers.pop();
                }
                else if (deltaPadding < 0) {
                        erase_occurrences(final_window, "\r\n", -deltaPadding);

                        std::size_t expected_new_length =
                            initial_window.length() - (-2 * deltaPadding);
                        assert(final_window.length() == expected_new_length);

                        input.erase(marker, initial_window.length());
                        input.insert(marker, final_window);
                        sifted.erase(marker, initial_window.length());
                        sifted.insert(marker, final_window);
                        stringMarkers.pop();
                        stringMarkers.shift(2 * deltaPadding);
                }
                else {
                        insert_copies(final_window, "\r\n", deltaPadding);
                        input.erase(marker, initial_window.length());
                        input.insert(marker, final_window);
                        sifted.erase(marker, initial_window.length());
                        sifted.insert(marker, final_window);
                        stringMarkers.pop();
                        stringMarkers.shift(2 * deltaPadding);
                }
        }
}




void doPadding(std::string& inputString, int desiredPadding)
{
        std::string sifted = siftCriticalCharacters(inputString);
        std::string outerScopes = siftOuterScopes(sifted);
        StringMarkers scopeMarkers = findScopes(outerScopes);
        calculatePadding(
            inputString, outerScopes, scopeMarkers, desiredPadding);
}
