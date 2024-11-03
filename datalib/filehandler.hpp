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
#include <algorithm>

namespace fs = std::filesystem;

// MatchData struct to store data extracted from each file
struct MatchData
{
	std::string fileName;
	int finalNumber;																						 // New field to store the final number extracted from the filename
	std::unordered_map<std::string, std::vector<double>> values; // Maps patterns to their corresponding values
};

/**
 * @brief Extracts values from a set of files in a given directory based on a set of patterns.
 *
 * @param directoryPath The path to the directory containing the files to be processed.
 * @param fileType The extension of the files to be processed.
 * @param patterns The set of patterns to search for in the files. The values will be extracted from the files and stored in the MatchData struct.
 *
 * @return A vector of MatchData structs, each containing the filename and the extracted values for the corresponding file.
 */
std::vector<MatchData> extract_pattern_values_from_file(const std::string &directoryPath, std::string &fileType, const std::vector<std::string> &patterns)
{
	std::vector<MatchData> gpDataList;

	for (const auto &entry : fs::directory_iterator(directoryPath))
	{
		if (entry.path().extension() == fileType)
		{
			std::ifstream file(entry.path());
			if (!file.is_open())
			{
				std::cerr << "Error opening file: " << entry.path() << std::endl;
				continue;
			}

			MatchData fileData;
			fileData.fileName = entry.path().filename().string();
			std::string line;

			// Initialize values map for each pattern
			for (const auto &pattern : patterns)
			{
				fileData.values[pattern] = {};
			}

			while (std::getline(file, line))
			{
				for (const auto &pattern : patterns)
				{
					if (line.find(pattern) != std::string::npos)
					{
						std::istringstream iss(line);
						std::string label;
						double value;

						// Read the pattern and skip the corresponding parts
						iss >> label; // Read the first part (the pattern)
						std::vector<std::string> patternParts;
						std::istringstream patternStream(pattern);
						std::string part;

						// Split the pattern into parts
						while (patternStream >> part)
						{
							patternParts.push_back(part);
						}

						// Skip the number of parts in the pattern minus one (for the label)
						for (size_t i = 1; i < patternParts.size(); ++i)
						{
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
			for (const auto &pattern : patterns)
			{
				if (!fileData.values[pattern].empty())
				{
					hasValues = true;
					break;
				}
			}
			if (hasValues)
			{
				gpDataList.push_back(fileData);
			}
			else
			{
				std::cerr << "No values found for the specified patterns in file: " << entry.path() << std::endl;
			}
		}
	}
	return gpDataList;
}



/**
 * @brief Writes match data to a file with optional custom headers.
 *
 * @param[in] data --- Vector of MatchData objects containing data to be written.
 * @param[in] outputFileName --- Path to the output file.
 * @param[in] headers --- Optional custom headers to use for the columns. If empty, headers are inferred from data.
 */
void write_match_data_to_file(const std::vector<MatchData> &data,
															const std::string &outputFileName,
															const std::vector<std::string> &headers = {})
{
	std::ofstream outFile(outputFileName);

	if (!outFile.is_open())
	{
		std::cerr << "Error opening output file: " << outputFileName << std::endl;
		return;
	}

	// Determine column headers
	if (!headers.empty())
	{
		for (const auto &header : headers)
		{
			outFile << header << "\t\t";
		}
	}
	else if (!data.empty())
	{
		for (const auto &[pattern, _] : data.front().values)
		{
			outFile << pattern << "\t\t";
		}
	}
	outFile << std::endl;

	// Write the values for each file
	for (const auto &gpData : data)
	{
		size_t maxRows = 0;
		for (const auto &[_, values] : gpData.values)
		{
			maxRows = std::max(maxRows, values.size());
		}

		for (size_t i = 0; i < maxRows; ++i)
		{
			outFile << gpData.fileName << "\t\t"; // Write file name in #config column

			for (const auto &[_, values] : gpData.values)
			{
				if (i < values.size())
				{
					outFile << values[i] << "\t\t"; // Write the value
				}
				else
				{
					outFile << "NaN" << "\t\t"; // Placeholder for missing values
				}
			}
			outFile << std::endl;
		}
	}

	outFile.close();
	if (!outFile)
	{
		std::cerr << "Error writing to output file: " << outputFileName << std::endl;
	}
}



/**
 * @brief Function to write data with optional headers to a file.
 *
 * @param[in] data --- Vector of pairs containing the data to be written (int, double).
 * @param[in] filename --- Path to the output file where data will be written.
 * @param[in] headers --- Optional vector of headers to write as the first line in the file.
 *
 * @details If headers are provided, they are written to the first line of the file,
 * separated by a space. Each subsequent line contains a pair of data values.
 */
void write_pair_data_to_file(const std::vector<std::pair<int, double>> &data,
														 const std::string &filename,
														 const std::vector<std::string> &headers = {})
{
	std::ofstream outfile(filename); // Open the file for writing

	// Check if the file was opened successfully
	if (!outfile)
	{
		std::cerr << "Error opening file for writing: " << filename << std::endl;
		return;
	}

	// Write headers if provided
	if (!headers.empty())
	{
		for (size_t i = 0; i < headers.size(); ++i)
		{
			outfile << headers[i];
			if (i < headers.size() - 1)
				outfile << "\t\t"; // Separate headers with a space
		}
		outfile << std::endl; // End the header line
	}

	// Write the data to the file
	for (const auto &pair : data)
	{
		outfile << pair.first << "\t\t" << pair.second << std::endl;
	}

	// Close the file
	outfile.close();
}



/**
 * @brief Processes a .dat file by extracting specific numbers from filenames and writing them to an output file.
 *
 * This function reads each line from the specified input .dat file, extracts a numerical identifier
 * from filenames matching the pattern "landau-(\\d+)\\.out", and writes the extracted number along
 * with the rest of the line to the specified output file. If the pattern is not found in a line,
 * an error message is printed to the standard error.
 *
 * @param inputFileName The name of the input .dat file to be processed.
 * @param outputFileName The name of the output file where the processed data will be written.
 */
void extract_config_of_file(const std::string &inputFileName, const std::string &outputFileName)
{
	std::ifstream inputFile(inputFileName);
	std::ofstream outputFile(outputFileName);

	if (!inputFile.is_open() || !outputFile.is_open())
	{
		std::cerr << "Error opening file!" << std::endl;
		return;
	}

	std::string line;
	std::regex landauRegex("landau-(\\d+)\\.out");

	while (std::getline(inputFile, line))
	{
		std::istringstream lineStream(line);
		std::string firstColumn;
		lineStream >> firstColumn;

		std::smatch match;
		if (std::regex_search(firstColumn, match, landauRegex))
		{
			std::string extractedNumber = match[1];
			outputFile << extractedNumber;

			// Write the rest of the line to the output file
			std::string remainingLine;
			std::getline(lineStream, remainingLine);
			outputFile << remainingLine << std::endl;
		}
		else
		{
			std::cerr << "Pattern not found in line: " << line << std::endl;
		}
	}

	inputFile.close();
	outputFile.close();
}



struct Row
{
	std::vector<double> data;
};

// Comparator to sort based on a specific column
inline bool
compareRows(const Row &a, const Row &b, int col)
{
	return a.data[col] < b.data[col];
}

/**
 * @brief Sorts a file by a specific column and writes the sorted data to an output file.
 *
 * This function reads the input file line by line, splits each line into columns, and stores the columns in a vector of Row structs.
 * The rows are then sorted in ascending order based on the specified column index.
 * If the column index is invalid, an error message is printed to the standard error and false is returned.
 * The sorted data is then written to the output file.
 *
 * @param inputFile The name of the input file to be sorted.
 * @param outputFile The name of the output file where the sorted data will be written.
 * @param columnIndex The index of the column to sort by.
 *
 * @return True if the sorting and writing were successful, false otherwise.
 */
bool sort_column_in_file(const std::string &inputFile, const std::string &outputFile, int columnIndex)
{
	std::ifstream inFile(inputFile);
	if (!inFile)
	{
		std::cerr << "Error: Cannot open input file.\n";
		return false;
	}

	std::vector<Row> rows;
	std::string line;
	while (std::getline(inFile, line))
	{
		std::istringstream ss(line);
		Row row;
		double value;
		while (ss >> value)
		{
			row.data.push_back(value);
		}
		rows.push_back(row);
	}
	inFile.close();

	// Check if the specified column index is valid
	if (rows.empty() || columnIndex < 0 || columnIndex >= rows[0].data.size())
	{
		std::cerr << "Error: Invalid column index.\n";
		return false;
	}

	// Sort rows by the specified column
	std::sort(rows.begin(), rows.end(), [columnIndex](const Row &a, const Row &b)
						{ return a.data[columnIndex] < b.data[columnIndex]; });

	// Write sorted data to the output file
	std::ofstream outFile(outputFile);
	if (!outFile)
	{
		std::cerr << "Error: Cannot open output file.\n";
		return false;
	}

	for (const auto &row : rows)
	{
		for (size_t i = 0; i < row.data.size(); ++i)
		{
			outFile << row.data[i];
			if (i < row.data.size() - 1)
			{
				outFile << " ";
			}
		}
		outFile << "\n";
	}
	outFile.close();

	return true;
}



/**
 * @brief Reads a specified column from a file as a vector of doubles.
 *
 * @param[in] filename Path to the file to read.
 * @param[in] columnIndex Index of the column to read (0-based).
 *
 * @return Vector of doubles containing the data from the specified column.
 *
 * @details The function assumes that the file has the same number of columns in
 * each row. If the file is empty or cannot be opened, an empty vector is returned.
 * If the specified column index is out of bounds, an empty vector is returned.
 */
std::vector<double> readColumn(const std::string &filename, int columnIndex)
{
	std::vector<double> columnData;
	std::ifstream file(filename);

	if (!file)
	{
		std::cerr << "Error: Cannot open file " << filename << "\n";
		return columnData;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		double value;
		int currentColumn = 0;

		// Read up to the desired column
		while (ss >> value)
		{
			if (currentColumn == columnIndex)
			{
				columnData.push_back(value);
				break;
			}
			currentColumn++;
		}
	}

	file.close();
	return columnData;
}



/**
 * @brief Function to remove a file.
 *
 * @param[in] filename --- The path to the file to be removed.
 *
 * @details Checks if the file exists before attempting deletion.
 * If successful, it confirms removal; otherwise, it outputs an error.
 */
void remove_file(const std::string &filename)
{
	try
	{
		if (fs::exists(filename))
		{
			if (fs::remove(filename))
			{
				std::cout << "File '" << filename << "' successfully removed." << std::endl;
			}
			else
			{
				std::cerr << "Failed to remove file '" << filename << "'." << std::endl;
			}
		}
		else
		{
			std::cerr << "File '" << filename << "' does not exist." << std::endl;
		}
	}
	catch (const fs::filesystem_error &e)
	{
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
}

#endif // filehandler.hpp
