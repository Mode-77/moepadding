#include "SuperString.hpp"
#include "paddingProcess.hpp"
#include "version.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <list>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

std::string versionString()
{
        std::string version;
        version.append(std::to_string(VERSION_MAJOR));
        version.append(".");
        version.append(std::to_string(VERSION_MINOR));
        version.append(".");
        version.append(std::to_string(VERSION_PATCH));
        return version;
}




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




void printInfo()
{
        std::cout << "moepadding " << versionString() << "\n\n";
        std::cout << "Usage:\n";
        std::cout << "\tmpad <filename> <padding> [-w]\n\n";
        std::cout << "Options:\n";
        std::cout << "\t-w: Overwrite the input file\n\n";
}




/// @brief Converts a zero-terminated string to a nonnegative integer
/// @param c Not null
/// @return Nonnegative result on success, -1 if a conversion could not be
/// performed, or -2 if the input string represented a negative value
int cStringToNonnegativeInt(char const* c)
{
        std::size_t firstUnconverted = 0;
        std::string const paddingAsString(c);
        int result = 0;
        try {
                result = std::stoi(paddingAsString, &firstUnconverted, 10);
        }
        catch (std::invalid_argument) {
                return -1;
        }
        // If not all of the input string could be converted, then it's the same
        // as an invalid argument
        if (firstUnconverted != paddingAsString.length()) {
                return -1;
        }
        return result >= 0 ? result : -2;
}




void restoreNewlines(std::string& s)
{
        findReplaceAll(s, "\r", "");
}




int main(int argc, char* argv[])
{
        // Usage
        //
        // mpad filename padding [-w]

        if (!(argc == 3 || argc == 4)) {
                // Too few or too many arguments
                printInfo();
                return EXIT_FAILURE;
        }

        int const desiredPadding = cStringToNonnegativeInt(argv[2]);
        switch (desiredPadding) {
        case -1: {
                std::cout << "Invalid padding\n";
                return EXIT_FAILURE;
        }
        case -2: {
                std::cout << "Padding cannot be negative\n";
                return EXIT_FAILURE;
        }
        }

        std::FILE* inputFile = openFile(argv[1], "rb");
        if (inputFile == nullptr) {
                std::cout << "File couldn't be opened\n";
                return EXIT_FAILURE;
        }
        std::string fileContents(extractAll(inputFile));
        fclose(inputFile);

        doPadding(fileContents, desiredPadding);

        restoreNewlines(fileContents);

        // @Todo Replace with file stream
        if (argc > 3 && std::string(argv[3]) == "-w") {
                // Overwrite input file
                std::FILE* outputFile = openFile(argv[1], "w");
                if (outputFile == nullptr) {
                        std::cout << "Failed to create output file\n";
                        return EXIT_FAILURE;
                }
                int writeSuccess = fputs(fileContents.c_str(), outputFile);
                fclose(outputFile);
                if (writeSuccess < 0) {
                        std::cout
                            << "Failed to write contents to output file\n";
                        return EXIT_FAILURE;
                }
        }
        else {
                std::cout << fileContents << "\n";
        }

        return EXIT_SUCCESS;
}
