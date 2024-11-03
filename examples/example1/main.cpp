
#include <iostream>
#include "../datalib/filehandler.hpp"
#include "../datalib/stattools.hpp"
#include "../datalib/spaceoperator.hpp"

#include "params.hpp"

int main()
{
  // Directory and extension path to data files
  std::string dataPath = sysParams.dataPath;
  std::string fileExtension = sysParams.fileExtension;
  std::string outputDirectory = sysParams.outputDirectory;

  // Define the patterns to search for
  std::vector<std::string> patterns = {
      "GP_T 0  0  0  0",
      "GP_L 0  0  0  0"
      // Add other patterns as needed
  };


  //============================ FILE HANDLING =================================

  // Collect and store data from files
  std::vector<MatchData> dataExtracted = extract_pattern_values_from_file(dataPath, fileExtension, patterns);

  // Write data that mathches patterns into file
  write_match_data_to_file(dataExtracted, outputDirectory + "raw_GP0000.dat", {"#file_name", "values extracted"});

  // Print Ooutput results
  if (false)
  {
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
  }

  // Post-Process function to get configuration from data-file name
  extract_config_of_file(outputDirectory + "raw_GP0000.dat", outputDirectory + "pos_raw_GP0000.dat");

  // Remove the preocessed (previous) file
  remove_file(outputDirectory + "raw_GP0000.dat");
  
  // Sort the column colSort of a file
  const int colToSort = 0;
  sort_column_in_file(outputDirectory + "pos_raw_GP0000.dat", outputDirectory + "sorted_raw_GP0000.dat", colToSort);
  
  // Remove the preocessed (previous) file
  remove_file(outputDirectory + "pos_raw_GP0000.dat");
  //===============================================================================


  // Collect column data from file
  std::string sortedFileData = outputDirectory + "sorted_raw_GP0000.dat";
  int colToRead = 1; // Specify the column index to read (0-based)
  std::vector<double> GP_T_0000_10_dat = readColumn(sortedFileData, colToRead);
  colToRead = 2;
  std::vector<double> GP_L_0000_10_dat = readColumn(sortedFileData, colToRead);
  

  // Calculate mean value
  double mean_value_GP_T = mean(GP_T_0000_10_dat);
  double mean_value_GP_L = mean(GP_L_0000_10_dat);

  // Calculate variance
  double variance_value_GP_T = variance(GP_T_0000_10_dat);
  double variance_value_GP_L = variance(GP_L_0000_10_dat);

  double jackErr_GP_T = jack_error(GP_T_0000_10_dat);
  double jackErr_GP_L = jack_error(GP_L_0000_10_dat);

  // std::cout << "Mean: " << mean_value << std::endl;
  // std::cout << "Var: " << variance_value << std::endl;
  // std::cout << "JackKnife Error: " << jackErr << std::endl;


  //============================ Autocorrelation =================================

  // Bin the data (bin_size = 1 for no binning effect)
  int bin_size = sysParams.bin_size;
  std::vector<double> GP_T_0000_10_dat_bin = bin_data(GP_T_0000_10_dat, bin_size);
  std::vector<double> GP_L_0000_10_dat_bin = bin_data(GP_L_0000_10_dat, bin_size);

  
  int tau_max = GP_T_0000_10_dat_bin.size();
  std::vector<std::pair<int, double>> corrCoefPair_GP_T_0000_10;
  std::vector<std::pair<int, double>> corrCoefPair_GP_L_0000_10;

  // Calculate autocorrelation coefficients for each tau
  autoCorrel_sample_operator(GP_T_0000_10_dat_bin, corrCoefPair_GP_T_0000_10, tau_max);
  autoCorrel_sample_operator(GP_L_0000_10_dat_bin, corrCoefPair_GP_L_0000_10, tau_max);
  
  std::vector<double> dataCorrCoef_GP_T_0000_10;
  std::vector<double> dataCorrCoef_GP_L_0000_10;

  // Extract autocorrelation coefficients
  for (auto pair : corrCoefPair_GP_T_0000_10)
  {
    dataCorrCoef_GP_T_0000_10.push_back(pair.second);
  }
  for (auto pair : corrCoefPair_GP_L_0000_10)
  {
    dataCorrCoef_GP_L_0000_10.push_back(pair.second);
  }

  // Write autocorrelation results to file.
  write_pair_data_to_file(corrCoefPair_GP_T_0000_10, outputDirectory + "autocorr_GP_T_0000_10.dat", {"#tau", "corr_coef"});
  write_pair_data_to_file(corrCoefPair_GP_L_0000_10, outputDirectory + "autocorr_GP_L_0000_10.dat", {"#tau", "corr_coef"});

  //===============================================================================

  // Output the values in the column
  if (false)
  {
    std::cout << "Column " << colToRead << " data:\n";
    for (const double &value : GP_T_0000_10_dat)
    {
      std::cout << value << "\n";
    }
  }
  return 0;
}
