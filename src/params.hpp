#ifndef PARAMS_HPP
#define PARAMS_HPP

struct Params
{
  int bin_size = 1; // Bin size for averaging when applied Binning to data
  int tau_max = 1;  // Maximum time displacement for autocorrelation

  //std::string dataPath = "/home/eduardo-salgado/gluon_prop/Navigator/to_send_48_3_10/copy_of_48_3_10"; // Path to directory containing data files
  std::string dataPath = "/home/eduardo-salgado/gluon_prop/Navigator/output_48_3_12/output_48_3_12";
  std::string fileExtension = ".out"; // File extension of data files to be analyzed
  std::string outputDirectory = "/home/eduardo-salgado/Lattice_QFT/Data_Analysis/output/GP_0000_12/"; // Path to directory where output files will be saved
};

Params sysParams;

#endif // PARAMS_HPP
