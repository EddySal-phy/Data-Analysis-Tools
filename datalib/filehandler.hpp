#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <sstream>



namespace fs = std::filesystem;

struct MatchData {
    std::string fileName;
    int finalNumber; // New field to store the final number extracted from the filename
    std::unordered_map<std::string, std::vector<double>> values; // Maps patterns to their corresponding values
};

std::vector<MatchData> extract_pattern_values_from_file(const std::string& directoryPath, std::string& fileType, const std::vector<std::string>& patterns) {
    std::vector<MatchData> gpDataList;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.path().extension() == fileType) {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << entry.path() << std::endl;
                continue;
            }

            MatchData fileData;
            fileData.fileName = entry.path().filename().string();
            std::string line;

            // Initialize values map for each pattern
            for (const auto& pattern : patterns) {
                fileData.values[pattern] = {};
            }

            while (std::getline(file, line)) {
                for (const auto& pattern : patterns) {
                    if (line.find(pattern) != std::string::npos) {
                        std::istringstream iss(line);
                        std::string label;
                        double value;

                        // Read the pattern and skip the corresponding parts
                        iss >> label; // Read the first part (the pattern)
                        std::vector<std::string> patternParts;
                        std::istringstream patternStream(pattern);
                        std::string part;

                        // Split the pattern into parts
                        while (patternStream >> part) {
                            patternParts.push_back(part);
                        }

                        // Skip the number of parts in the pattern minus one (for the label)
                        for (size_t i = 1; i < patternParts.size(); ++i) {
                            iss >> label; // Skip the next parts
                        }

                        // Read the value
                        iss >> value;
                        fileData.values[pattern].push_back(value);
                    }
                }
            }
            file.close();

            // Store the file data if any values were found
            bool hasValues = false;
            for (const auto& pattern : patterns) {
                if (!fileData.values[pattern].empty()) {
                    hasValues = true;
                    break;
                }
            }
            if (hasValues) {
                gpDataList.push_back(fileData);
            } else {
                std::cerr << "No values found for the specified patterns in file: " << entry.path() << std::endl;
            }
        }
    }
    return gpDataList;
}

void writeDataToFile(const std::vector<MatchData>& data, const std::string& outputFileName) {
    std::ofstream outFile(outputFileName);

    if (!outFile.is_open()) {
        std::cerr << "Error opening output file: " << outputFileName << std::endl;
        return;
    }

    // Write headers
    outFile << "#config\t";
    for (const auto& [pattern, _] : data.front().values) {
        outFile << pattern << "\t";
    }
    outFile << std::endl;

    // Write the values for each file
    for (const auto& gpData : data) {
        size_t maxRows = 0;
        for (const auto& [_, values] : gpData.values) {
            if (values.size() > maxRows) {
                maxRows = values.size();
            }
        }

        for (size_t i = 0; i < maxRows; ++i) {
            outFile << gpData.fileName << "\t";  // Write file name in #config column

            for (const auto& [_, values] : gpData.values) {
                if (i < values.size()) {
                    outFile << values[i] << "\t";  // Write the value
                } else {
                    outFile << "NaN" << "\t";  // Placeholder for missing values
                }
            }
            outFile << std::endl;
        }
    }

    outFile.close();
}


void processDatFile(const std::string& inputFileName, const std::string& outputFileName) {
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);
    
    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    std::string line;
    std::regex landauRegex("landau-(\\d+)\\.out");

    while (std::getline(inputFile, line)) {
        std::istringstream lineStream(line);
        std::string firstColumn;
        lineStream >> firstColumn;

        std::smatch match;
        if (std::regex_search(firstColumn, match, landauRegex)) {
            std::string extractedNumber = match[1];
            outputFile << extractedNumber;

            // Write the rest of the line to the output file
            std::string remainingLine;
            std::getline(lineStream, remainingLine);
            outputFile << remainingLine << std::endl;
        } else {
            std::cerr << "Pattern not found in line: " << line << std::endl;
        }
    }

    inputFile.close();
    outputFile.close();
}


#endif //filehandler.hpp
