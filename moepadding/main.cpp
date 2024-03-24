#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "File.hpp"
#include "program.hpp"
#include "version.h"

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
        std::cout << "moepadding " << versionString()
                  << "\n"
                     "\n"
                     "Usage:\n"
                     "\tmpad <filename> [<padding>] [-w]\n"
                     "\n"
                     "Options:\n"
                     "\t-w: Overwrite the input file\n";
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




/// @brief Class that represents the state of the program including its
/// variables and error state
class ProgramVariables final {
    public:
        ProgramVariables()
            : mPadding(-1),
              mOverwrite(false),
              mError(false),
              mErrorWhat(),
              mConfigPath(),
              mFullFilePath()
        {
        }

        /// @return True if the amount of desired padding was not set by any
        /// mechanism-- user or configuration file. False otherwise.
        [[nodiscard]] bool paddingNotSet() const noexcept
        {
                return mPadding == -1;
        }

        /// @details Determine whether a path to a configuration file has been
        ///          found. The contents of the config file need not be
        ///          correct.
        ///
        /// @return True if a path to a configuration file has been found.
        ///         False if no config file was found.
        [[nodiscard]] bool configFileFound() const noexcept
        {
                return !mConfigPath.empty();
        }

        int mPadding;    ///< Amount of desired padding. Cannot be negative.
        bool mOverwrite; ///< True if the input file is to be overwritten
        bool mError;     ///< True if an internal program error has occurred

        ///
        /// If mError is true, contains the nature of the error. If mError is
        /// false, the contents are unspecified.
        std::string mErrorWhat;

        ///
        /// Contains a valid path to a configuration file if one was found
        std::string mConfigPath;

        ///
        /// Contains a valid path to a file to operate on
        std::filesystem::path mFullFilePath;
};




/// @details Performs a recursive up-search from the current directory for a
///          configuration file. If one was found, the value inside is read and
///          used as padding. The program's state variables are also updated. If
///          some error occurred, the program's error state variable is set
///          along with a message containing the nature of the error.
void scrapeConfiguration(ProgramVariables& programVariables);

/// @details Parses the array of c-strings as command-line arguments to the
///          program and builds a set of program variables based on the
///          arguments. If some error occurred, the program's error state
///          variable is set along with a message containing the nature of the
///          error.
///
/// @param count Shall not exceed the size of the array
/// @param arguments
ProgramVariables parseArguments(int const count, char* arguments[]);

int main(int argc, char* argv[])
{
        // Usage
        //
        // mpad filename [padding] [-w]

        // Fail on too few or too many arguments
        if ((argc < 2) || (argc > 4)) {
                printUsage();
                return EXIT_FAILURE;
        }

        ProgramVariables programVariables = parseArguments(argc, argv);
        if (programVariables.mError) {
                std::cout << programVariables.mErrorWhat;
                return EXIT_FAILURE;
        }

        if (programVariables.paddingNotSet()) {
                scrapeConfiguration(programVariables);
                if (programVariables.configFileFound()) {
                        std::cout << "Consuming config file found at \""
                                  << programVariables.mConfigPath << "\"\n";
                }

                if (programVariables.mError) {
                        std::cout << programVariables.mErrorWhat;
                        return EXIT_FAILURE;
                }
        }

        return program(
            argv[1], programVariables.mPadding, programVariables.mOverwrite);
}




/// @brief Parses the string as a command-line option and sets the appropriate
///        program variable. If the option is unknown, the program's error state
///        variable is set, and an appropriate error message is built.
///
/// @note Command-line options are specified with a hyphen and a single
///       character
///
/// @param s Shall not be empty
/// @param variables
void parseOption(std::string const& s, ProgramVariables& variables)
{
        if (s == "-w") {
                variables.mOverwrite = true;
        }
        else {
                // Unknown option
                variables.mError = true;
                variables.mErrorWhat = "Unknown option " + s + "\n";
        }
}




/// @brief Parses the string as a padding amount and sets the appropriate
///        program variable. If the padding is invalid, or some other error
///        occurred, the program's error state variable is set, and an
///        appropriate error message is built.
///
/// @param s Shall not be empty
/// @param variables
void parsePadding(std::string const& s, ProgramVariables& variables)
{
        int const desiredPadding = cStringToNonnegativeInt(s.c_str());
        switch (desiredPadding) {
        case -1: {
                variables.mError = true;
                variables.mErrorWhat = "Invalid padding\n";
                break;
        }
        case -2: {
                variables.mError = true;
                variables.mErrorWhat = "Padding cannot be negative\n";
                break;
        default: {
                variables.mPadding = desiredPadding;
        }
        }
        }
}




ProgramVariables parseArguments(int const count, char* arguments[])
{
        ProgramVariables parsed;

        std::filesystem::path pathArgument = arguments[1];
        if (!std::filesystem::is_regular_file(pathArgument)) {
                parsed.mError = true;
                parsed.mErrorWhat = "Not a valid file\n";
                return parsed;
        }
        else {
                parsed.mFullFilePath = pathArgument;
        }

        for (int i = 2; i < count; ++i) {
                std::string asString(arguments[i]);
                if (asString.front() == '-') {
                        // Treat this as an option
                        parseOption(asString, parsed);
                        if (parsed.mError) {
                                return parsed;
                        }
                }
                else {
                        // Treat this as padding
                        parsePadding(asString, parsed);
                        if (parsed.mError) {
                                return parsed;
                        }
                }
        }

        return parsed;
}




/// @details Searches for a file, beginning the search in the given directory
///          and recursively searching the containing parent until it is found,
///          or until the search fails in the root directory, whichever occurs
///          first.
///
/// @param path Path to begin the search
/// @param filename
/// @return Path to the file if it was found or std::nullopt otherwise
std::optional<std::filesystem::path> recursiveUpFind(
    std::filesystem::path path,
    std::string const& filename)
{
        std::filesystem::path full(path);
        full.append(filename);
        if (std::filesystem::is_regular_file(full)) {
                return full;
        }
        while (path.parent_path() != path) {
                path.assign(path.parent_path());
                full.assign(path);
                full.append(filename);
                if (std::filesystem::is_regular_file(full)) {
                        return full;
                }
        }
        return std::nullopt;
}




void scrapeConfiguration(ProgramVariables& programVariables)
{
        std::string const configFilename(".mpad");

        std::filesystem::path const beginSearchAt =
            programVariables.mFullFilePath
                .parent_path(); // Since this is a full file path which includes
                                // the file name, the parent is the containing
                                // directory.

        std::optional<std::filesystem::path> configPath =
            recursiveUpFind(beginSearchAt, configFilename);

        if (!configPath.has_value()) {
                programVariables.mError = true;
                programVariables.mErrorWhat = "No config file found\n";
                return;
        }
        std::string const pathAsString(configPath.value().string());
        programVariables.mConfigPath = pathAsString;

        // @Todo Have to try/catch the constructor since even though the path
        // exists, opening the file can still fail for any number of reasons.
        File configFile(pathAsString);
        if (configFile.exhausted()) {
                programVariables.mError = true;
                programVariables.mErrorWhat = "Corrupted config file\n";
                return;
        }

        std::optional<int> paddingFromFile = configFile.readAsInt();
        if (!paddingFromFile.has_value()) {
                programVariables.mError = true;
                programVariables.mErrorWhat = "Invalid padding\n";
        }
        else if (paddingFromFile.value() < 0) {
                programVariables.mError = true;
                programVariables.mErrorWhat = "Padding cannot be negative\n";
        }
        else {
                programVariables.mPadding = paddingFromFile.value();
        }
}
