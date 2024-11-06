
#include <iostream>
#include "../datalib/filehandler.hpp"
#include "../datalib/stattools.hpp"
#include "../datalib/spaceoperator.hpp"

#include "params.hpp"

int main()
{ 
  // Define the patterns to search for
  const std::vector<std::string> patterns = {
      "GP_T 0  0  0  0",
      "GP_L 0  0  0  0"
      // Add other patterns as needed
  };

  // Directory and extension path to data files
  const std::string dataPath = sysParams.dataPath;
  const std::string fileExtension = sysParams.fileExtension;
  const std::string outputDirectory = sysParams.outputDirectory;
  
  std::cout << "\n*********************************************************\n\n"
            << "WARNING: Data will be read from: " << dataPath << "\n" 
            << "\n*********************************************************\n\n";



  bool generateFile = true;
  if (generateFile)
  {
    // Generate file
    const std::string outputFileName = "sorted_raw_GP0000.dat";
    specialGen_sort_trunc_file_operator(patterns, outputFileName, dataPath, fileExtension, outputDirectory);

    #pragma comment ( DANGER!!!: OS might break due to large file size )
    // Grep files in directory 
    //grep_directory(dataPath, outputDirectory + "grepFilter_raw_GP_T_0000.dat", "GP_T", fileExtension);
    //grep_directory(dataPath, outputDirectory + "grepFilter_raw_GP_L_0000.dat", "GP_L", fileExtension);
  }
  //===============================================================================


  // Collect column data from file
  std::string sortedFileData = outputDirectory + "sorted_raw_GP0000.dat";
  int colToRead = 1; // Specify the column index to read (0-based)
  std::vector<double> GP_T_0000_dat = readColumn(sortedFileData, colToRead);
  colToRead = 2;
  std::vector<double> GP_L_0000_dat = readColumn(sortedFileData, colToRead);
  

  // Calculate mean value
  double mean_value_GP_T = mean(GP_T_0000_dat);
  double mean_value_GP_L = mean(GP_L_0000_dat);

  // Calculate variance
  double variance_value_GP_T = variance(GP_T_0000_dat);
  double variance_value_GP_L = variance(GP_L_0000_dat);

  double jackErr_GP_T = jack_error(GP_T_0000_dat);
  double jackErr_GP_L = jack_error(GP_L_0000_dat);

  std::string str_mean_GP_T = "# mean: GP_T_0000: "  + std::to_string(mean_value_GP_T);
  std::string str_mean_GP_L = "# mean: GP_L_0000: "  + std::to_string(mean_value_GP_L);
 
  std::string str_var_GP_T = "# variance: GP_T_0000: " + std::to_string(variance_value_GP_T);
  std::string str_var_GP_L = "# variance: GP_L_0000: " + std::to_string(variance_value_GP_L);
 
  std::string str_jackErr_GP_T_str = "# jacknife error: GP_T_0000: " + std::to_string(jackErr_GP_T);
  std::string str_jackErr_GP_L_str = "# jacknife error: GP_L_0000: " + std::to_string(jackErr_GP_L);


  //============================ Autocorrelation =================================

  // Bin the data (bin_size = 1 for no binning effect)
  int bin_size = sysParams.bin_size;
  std::vector<double> GP_T_0000_dat_bin = bin_data(GP_T_0000_dat, bin_size);
  std::vector<double> GP_L_0000_dat_bin = bin_data(GP_L_0000_dat, bin_size);

  int tau_max = GP_T_0000_dat_bin.size();
  std::vector<std::pair<int, double>> corrCoefPair_GP_T_0000;
  std::vector<std::pair<int, double>> corrCoefPair_GP_L_0000;

  // Calculate autocorrelation coefficients for each tau
  autoCorrel_sample_operator(GP_T_0000_dat_bin, corrCoefPair_GP_T_0000, tau_max);
  autoCorrel_sample_operator(GP_L_0000_dat_bin, corrCoefPair_GP_L_0000, tau_max);
  
  std::vector<double> dataCorrCoef_GP_T_0000;
  std::vector<double> dataCorrCoef_GP_L_0000;

  // Extract autocorrelation coefficients 
  for (auto pair : corrCoefPair_GP_T_0000)
  {
    dataCorrCoef_GP_T_0000.push_back(pair.second);
  }
  for (auto pair : corrCoefPair_GP_L_0000)
  {
    dataCorrCoef_GP_L_0000.push_back(pair.second);
  }
  
  // Write autocorrelation results to file.
  std::vector<std::string> extraInfo_GP_T_0000 = {str_mean_GP_T, str_var_GP_T, str_jackErr_GP_T_str};
  std::vector<std::string> extraInfo_GP_L_0000 = {str_mean_GP_L, str_var_GP_L, str_jackErr_GP_L_str};

  write_pair_data_to_file(corrCoefPair_GP_T_0000, 
                          outputDirectory + "autocorr_GP_T_0000.dat",
                          {"#tau", "corr_coef"},
                          extraInfo_GP_T_0000);
 
  write_pair_data_to_file(corrCoefPair_GP_L_0000,
                          outputDirectory + "autocorr_GP_L_0000.dat",
                          {"#tau", "corr_coef"},
                          extraInfo_GP_L_0000);

  //===============================================================================

  // Output the values in the column
  if (false)
  {
    std::cout << "Column " << colToRead << " data:\n";
    for (const double &value : GP_T_0000_dat)
    {
      std::cout << value << "\n";
    }
  }
  return 0;
}
