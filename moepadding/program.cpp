#include "program.hpp"

#include "paddingProcess.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

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
                    std::string const& toFind,
                    std::string const& toReplace)
{
        if (toFind == toReplace) {
                return;
        }
        std::size_t foundAt = s.find(toFind);
        while (foundAt != std::string::npos) {
                s.erase(foundAt, toFind.length());
                s.insert(foundAt, toReplace);
                foundAt = s.find(toFind, foundAt + toReplace.length());
        }
}




void restoreNewlines(std::string& s)
{
        findReplaceAll(s, "\r", "");
}




int program(char const* filename, int desiredPadding, bool overwriteFile)
{
        std::FILE* inputFile = openFile(filename, "rb");
        if (inputFile == nullptr) {
                std::cout << "File couldn't be opened\n";
                return EXIT_FAILURE;
        }
        std::string inputText(extractAll(inputFile));
        fclose(inputFile);
        inputFile = nullptr;

        doPadding(inputText, desiredPadding);

        restoreNewlines(inputText);

        // @Todo Replace with file stream
        if (overwriteFile) {
                std::FILE* outputFile = openFile(filename, "w");
                if (outputFile == nullptr) {
                        std::cout << "Failed to create output file\n";
                        return EXIT_FAILURE;
                }
                int writeSuccess = fputs(inputText.c_str(), outputFile);
                fclose(outputFile);
                if (writeSuccess < 0) {
                        std::cout
                            << "Failed to write contents to output file\n";
                        return EXIT_FAILURE;
                }
        }
        else {
                std::cout << inputText << "\n";
        }

        return EXIT_SUCCESS;
}
