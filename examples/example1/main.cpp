
#include <iostream>
#include "../../datalib/filehandler.hpp"

int main()
{ 
  // Directory and extension path to data files
	std::string directoryPath = "/path/to/directory/with/data"; 
  std::string fileExtension = ".<file_extension>";
  std::string outputDirectory = "../../output/";

	// Define the patterns to search for
	std::vector<std::string> patterns = {
			"<string_with_pattern1>",
			"<string_with_pattern2>"
			// Add other patterns as needed
	};
  
  // Collect and store  data
	std::vector<MatchData> dataExtracted = extract_pattern_values_from_file(directoryPath, fileExtension, patterns);
  
  // Write to file
  write_match_data_to_file(dataExtracted, outputDirectory + "<file_to_write>");
  
	// Output the results
	for (const auto &gpData : dataExtracted)
	{
		std::cout << "File: " << gpData.fileName << std::endl;
		for (const auto &[pattern, values] : gpData.values)
		{
			std::cout << pattern << " Values: ";
			for (const auto &value : values)
			{
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}
	}
  

  // Post-Process function to get configuration from data-file name
  extract_config_of_file(outputDirectory + "<file_to_write>", outputDirectory + "<posproc_file_to_write>");

  const int colSort = 0;
  sort_column_in_file(outputDirectory + "<file_unsorted>", outputDirectory + "<file_sorted>", colSort);

  return 0;
}
