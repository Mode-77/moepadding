#include "SuperString.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <vector>

/**/
/* spam */
/* spam*/
/*spam */
/*spam*/
/*              */
/* This
   is a
   multiline
   comment.
*/

/* { } ; */

// { } ;

char testChar1 = '{';
char testChar2 = '}';
char testChar3 = ';';

std::string testStr1 = "{ } ; ' \" \\ / \n \t";

std::FILE* openFile(char const* filename, char const* mode)
{
        std::FILE* file = nullptr;
        fopen_s(&file, filename, mode);
        return file;
}

std::string extractAll(std::FILE* file)
{
        assert(file);
        std::string result;
        int character = std::fgetc(file);
        while (character != EOF) {
                result += character;
                character = std::fgetc(file);
        }
        return result;
}

void findReplaceAll(std::string& s,
                    std::string const& to_find,
                    std::string const& to_replace)
{
        if (to_find == to_replace) {
                return;
        }
        std::size_t found_at = s.find(to_find);
        while (found_at != std::string::npos) {
                s.erase(found_at, to_find.length());
                s.insert(found_at, to_replace);
                found_at = s.find(to_find, found_at + to_replace.length());
        }
}

void find_replace(std::string& s,
                  std::string const& to_find,
                  std::string const& to_replace,
                  int n)
{
        assert(n >= 0);
        if ((n == 0) || (to_find == to_replace)) {
                return;
        }
        std::size_t found_at = s.find(to_find);
        while ((found_at != std::string::npos)) {
                s.erase(found_at, to_find.length());
                s.insert(found_at, to_replace);
                if ((--n) == 0) {
                        return;
                }
                found_at = s.find(to_find, found_at + to_replace.length());
        }
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

bool isWhitespace(int character)
{
        return character == ' ' || character == '\t' || character == '\r' ||
               character == '\n';
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

int getPadding(std::string const& s, int index)
{
        int numNewlines = 0;
        ++index;
        if (index >= s.length()) {
                return -1;
        }
        int character = s.at(index);
        while (character != 'x') {
                if (character == '\n') {
                        ++numNewlines;
                }
                ++index;
                if (index >= s.length()) {
                        break;
                }
                character = s.at(index);
        }
        assert(numNewlines > 0);
        return numNewlines - 1;
}

/*
    Count non-whitespace characters
*/
int characterCount(std::FILE* file)
{
        assert(file);
        int result = 0;
        int character = std::fgetc(file);
        while (character != EOF) {
                if (!(character == ' ' || character == '\t' ||
                      character == '\n')) {
                        ++result;
                }
                character = std::fgetc(file);
        }
        return result;
}

struct ScopeMarker final {
        int begin;
        int end;
};

class StringMarkers final {
    public:
        StringMarkers(std::vector<std::size_t> const& v)
            : m_markers()
        {
                for (std::size_t i : v) {
                        m_markers.push_back(i);
                }
        }

        [[nodiscard]] std::size_t peek() const
        {
                return m_markers.front();
        }

        void pop()
        {
                m_markers.pop_front();
        }

        void shift(int delta)
        {
                for (std::size_t& m : m_markers) {
                        m += delta;
                }
        }

        [[nodiscard]] bool empty() const
        {
                return m_markers.empty();
        }

    private:
        std::list<std::size_t> m_markers;
};

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

void markScopes(std::string& s, StringMarkers markers)
{
        while (!markers.empty()) {
                s.insert(markers.peek(), "Y");
                markers.pop();
                markers.shift(1);
        }
}

std::string scopePassFilter(std::string const& s,
                            std::vector<ScopeMarker> const& scopes)
{
        std::string result;
        for (int i = 0; i < s.length(); ++i) {
                char const& character = s.at(i);
                if (character == ' ' || character == '\t' ||
                    character == '\n') {
                        result.append(1, character);
                        continue;
                }

                for (ScopeMarker const& marker : scopes) {
                        if (i == marker.begin || i == marker.end) {
                                result.append(1, character);
                                continue;
                        }
                }

                result.append(" ");
        }
        return result;
}

void restore(std::string const& original, std::string& padded)
{
        int masterIndex = 0;
        for (int i = 0; i < padded.length(); ++i) {
                char& c = padded.at(i);
                if (isWhitespace(c)) {
                        ++masterIndex;
                        continue;
                }
        }
}

struct PaddingAction final {
        int m_index;
        int m_deltaPadding;
};

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

int main(int argc, char* argv[])
{
        if (argc == 1) {
                // @Todo Need sanitization of filename?
                std::cout << "Must tell me the filename\n";
                return EXIT_FAILURE;
        }

        if (argc == 2) {
                // @Todo Need sanitization of padding argument
                std::cout << "Must tell me how much padding\n";
                return EXIT_FAILURE;
        }

        std::FILE* inputFile = openFile(argv[1], "rb");
        if (inputFile == nullptr) {
                std::cout << "File couldn't be opened\n";
                return EXIT_FAILURE;
        }

        std::string baseFileContents(extractAll(inputFile)); // Verified working
        fclose(inputFile);                                   // Done with it now

        std::string sifted;
        sifted = siftCriticalCharacters(baseFileContents);

        std::string outerScopes;
        outerScopes = siftOuterScopes(sifted);
        StringMarkers scopeMarkers = findScopes(outerScopes);

        // markScopes(outerScopes, scopeMarkers);
        // findReplaceAll(outerScopes, "x", " ");

        std::string const paddingAsString(argv[2]);
        // @Todo Check return value for error handling
        int const padding = std::stoi(paddingAsString);
        calculatePadding(baseFileContents, outerScopes, scopeMarkers, padding);

        std::FILE* outputFile = openFile(argv[1], "w");
        if (outputFile == nullptr) {
                std::cout << "Failed to create output file\n";
                return EXIT_FAILURE;
        }

        std::string outputString = baseFileContents;
        findReplaceAll(outputString, "\r", "");
        int writeSuccess = fputs(outputString.c_str(), outputFile);
        if (writeSuccess < 0) {
                std::cout << "Failed to write contents to output file\n";
                fclose(outputFile);
                return EXIT_FAILURE;
        }

        fclose(outputFile);

        return 0;
}
