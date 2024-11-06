#ifndef SPACEOPERATOR_HPP
#define SPACEOPERATOR_HPP

#include "filehandler.hpp"
#include "stattools.hpp"

/**
 * @brief Calculate autocorrelation for a given input vector up to a specified maximum tau (time displacement).
 *
 * @param[in] dataToCreateCorrSample Vector of double values to calculate autocorrelation.
 * @param[out] corr_coef_pair Vector of pairs to store the autocorrelation coefficient for each tau.
 * @param[in] tau_max Maximum time displacement for autocorrelation calculation.
 *
 * @details The function calculates the autocorrelation coefficient for each tau
 * up to tau_max and stores them in the corr_coef_pair vector as pairs of tau
 * and the autocorrelation coefficient at that tau. The autocorrelation
 * coefficient is calculated as c_tau / c_0, where c_0 is the autocorrelation
 * at tau = 0 and c_tau is the autocorrelation at tau.
 */
void autoCorrel_sample_operator(const std::vector<double> &dataToCreateCorrSample,
                                std::vector<std::pair<int, double>>& vectorToStoreCorrCoefPair,
                                const int tau_max) 
{
  double c_0 = auto_correl(dataToCreateCorrSample, 0);
  double corr_coef = 0;
  
  // Write autocorrelation results for each tau
  for (int tau = 0; tau < tau_max; ++tau)
  {
    // Calculate autocorrelation.
    double c_tau = auto_correl(dataToCreateCorrSample, tau);
    corr_coef = c_tau / c_0;

    // Store tau and autocorrelation coefficient in vector.
    vectorToStoreCorrCoefPair.push_back(std::make_pair(tau, corr_coef));
  }
}


void specialGen_sort_trunc_file_operator(const std::vector<std::string>& patterns,
                                    const std::string &outputFilename,
                                    const std::string &dataPath,
                                    const std::string &fileExtension,
                                    const std::string &outputDirectory)
{
  // Collect and store data from files
  std::vector<MatchData> dataExtracted = extract_pattern_values_from_file(dataPath, fileExtension, patterns);

  // Write data that mathches patterns into file
  write_match_data_to_file(dataExtracted, outputDirectory + "tempFile1.dat", {"#file_name", "values extracted"});

  // Print Output results -- for debug purposes
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
  extract_config_of_file(outputDirectory + "tempFile1.dat", outputDirectory + "tempFile2.dat");

  // Remove the preocessed (previous) file
  //remove_file(outputDirectory + "tempFile1.dat");
  
  // Sort the column colSort of a file
  const int colToSort = 0;
  sort_column_in_file(outputDirectory + "tempFile2.dat", outputDirectory + outputFilename, colToSort);
  
  // Remove the preocessed (previous) file
  //remove_file(outputDirectory + "tempFile2.dat");
}




#endif // SPACEOPERATOR_HPP