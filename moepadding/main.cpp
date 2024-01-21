#include <cassert>
#include <cstdio>
#include <iostream>

std::FILE* openFile(char const* filename, char const* mode)
{
    std::FILE* file = nullptr;
    fopen_s(&file, filename, mode);
    return file;
}

/*
    Count non-whitespace characters
*/
int characterCount(std::FILE* file)
{
    assert(file);
    int result = 0;
    int character = std::fgetc(file);
    while (character != EOF)
    {
        if (!(character == ' ' || character == '\t' || character == '\n'))
        {
            ++result;
        }
        character = std::fgetc(file);
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << "Must tell me the filename\n";
        return EXIT_FAILURE;
    }
    std::FILE* file = openFile(argv[1], "r");
    if (file == nullptr)
    {
        std::cout << "File couldn't be opened\n";
        return EXIT_FAILURE;
    }
    std::cout << "Character count: " << characterCount(file) << "\n";
    return 0;
}
