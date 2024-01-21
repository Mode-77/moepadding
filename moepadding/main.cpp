#include <cassert>
#include <cstdio>
#include <iostream>

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
    while (character != EOF)
    {
        result += character;
        character = std::fgetc(file);
    }
    return result;
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
    std::string baseFileContents(extractAll(file));
    std::cout << baseFileContents;

    // Create copy of the input file.
    std::FILE* outputFile = nullptr;
    fopen_s(&outputFile, "moeout", "w");
    if (outputFile == nullptr)
    {
        std::cout << "Failed to create output file\n";
        // Can't continue like this.
        return EXIT_FAILURE;
    }

    // Copy the contents over.
    int copySuccess = fputs(baseFileContents.c_str(), outputFile);
    if (copySuccess < 0)
    {
        std::cout << "Failed to copy contents of base file\n";
        return EXIT_FAILURE;
    }

    fclose(outputFile);
    fclose(file);

    return 0;
}
