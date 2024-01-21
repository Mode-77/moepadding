#include <cassert>
#include <cstdio>
#include <iostream>

/*
    Count non-whitespace characters
*/
int characterCount(std::FILE *file)
{
    assert(file);
    int result = 0;
    int character = std::fgetc(file);
    while (character != EOF) {
        if (!(character == ' ' || character == '\t' || character == '\n')) {
            ++result;
        }
        character = std::fgetc(file);
    }
    return result;
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cout << "Must tell me the filename\n";
        return EXIT_FAILURE;
    }
    std::string filename(argv[1]);
    std::FILE *file = nullptr;
    // Open file for reading.
    fopen_s(&file, filename.c_str(), "r");
    if (file == nullptr) {
        std::cout << "File couldn't be opened\n";
        return EXIT_FAILURE;
    }
    std::cout << "Character count: " << characterCount(file) << "\n";
    return 0;
}
