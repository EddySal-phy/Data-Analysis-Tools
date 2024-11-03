#ifndef PARAMS_HPP
#define PARAMS_HPP

struct Params
{

  int bin_size = 1; // Bin size for averaging when applied Binning to data
  int tau_max = 1;  // Maximum time displacement for autocorrelation



  std::string dataPath = ""; // Path to directory containing data files
  std::string fileExtension = ".out"; // File extension of data files to be analyzed
  std::string outputDirectory = ""; // Path to directory where output files will be saved
};

Params sysParams;

#endif // PARAMS_HPP
