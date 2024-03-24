#include "program.hpp"
#include "version.h"
#include <iostream>
#include <stdexcept>
#include <string>

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




/// @brief Prints command usage along with program version information
void printUsage()
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




int main(int argc, char* argv[])
{
        // Usage
        //
        // mpad filename padding [-w]

        if (!(argc == 3 || argc == 4)) {
                // Too few or too many arguments
                printUsage();
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

        bool const overwriteFile = argc == 4 && std::string(argv[3]) == "-w";

        return program(argv[1], desiredPadding, overwriteFile);
}
