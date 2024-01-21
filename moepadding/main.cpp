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

std::FILE* copyFile(std::FILE* source, std::string const& filename)
{
    assert(source);
    std::FILE* newFile = openFile(filename.c_str(), "w");
    if (newFile == nullptr)
    {
        return nullptr;
    }
    std::string sourceText(extractAll(source));
    int writeSuccess = fputs(sourceText.c_str(), newFile);
    if (writeSuccess < 0)
    {
        fclose(newFile);
        return nullptr;
    }
    return newFile;
}

std::string mask(std::string const& s)
{
    std::string result;
    for (char const& c : s)
    {
        if (!(c == ' ' || c == '\t' || c == '\n'))
        {
            result.append("x");
        }
        else
        {
            result.append(1, c);
        }
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

    std::FILE* inputFile = openFile(argv[1], "r");
    if (inputFile == nullptr)
    {
        std::cout << "File couldn't be opened\n";
        return EXIT_FAILURE;
    }

    std::string baseFileContents(extractAll(inputFile));
    fclose(inputFile); // Done with it now.

    std::string masked(mask(baseFileContents));

    std::FILE* outputFile = openFile("moeout", "w");
    if (outputFile == nullptr)
    {
        std::cout << "Failed to create output file\n";
        return EXIT_FAILURE;
    }

    int writeSuccess = fputs(masked.c_str(), outputFile);
    if (writeSuccess < 0)
    {
        std::cout << "Failed to write contents to output file\n";
        fclose(outputFile);
        return EXIT_FAILURE;
    }

    fclose(outputFile);

    return 0;
}
