#ifndef STATTOOLS_HPP
#define STATTOOLS_HPP

#include <iostream>
#include <vector>
#include <numeric> // for std::accumulate
#include <cmath>   // for std::pow
#include <algorithm>
#include <random>

// Returns the mean value of values in a vector
double mean(const std::vector<double> &x)
{
  
  double mean = 0.0;
  for (int i = 0; i < x.size(); i++)
  {
     mean += x[i];
  }
    mean /= x.size();
    return mean;
 }


// Calculate the autocorrelation for a given correl. dist. tau
double auto_correl(const std::vector<double>& x, int tau)
{
    /**
     * Calculate the autocorrelation function for a given time series.
     *
     * @param x A time series of doubles.
     * @param tau The time displacement for the autocorrelation.
     *
     * @return The autocorrelation at the given time displacement.
     */

    double x_mean = 0., c_tau = 0.;
    int div_1 = 0., div_2 = 0.;

    for (int i = 0; i < x.size(); i++)
    {
        x_mean += x[i];
        div_1++;
    }
    x_mean /= div_1;

    // Calculate the correlation
    for (int i = 0; i < x.size() - tau; i++)
    {
        c_tau += (x[i] - x_mean) * (x[i + tau] - x_mean);
        div_2++;
    }
    c_tau /= div_2;

    return c_tau;
}


// Compute variance from a vector with data
double variance(const std::vector<double>& data)
{
    if (data.empty()) {
        std::cerr << "Error: Data vector is empty.\n";
        return 0.0;
    }

    // Calculate the mean (E[X])
    double meanVal = mean(data);

    // Calculate the mean of squared values (E[X^2])
    double meanOfSquares = 0.0;
    for (const double& value : data) {
        meanOfSquares += std::pow(value, 2);
    }
    meanOfSquares /= data.size();

    // Variance using the formula Var[X] = E[X^2] - (E[X])^2
    double variance = meanOfSquares - std::pow(meanVal, 2);

    return variance;
}


// Compute the jack_sample-th jackknife sample
// from input vector data into output vector jack_data
void jack_set(const std::vector<double> &data, std::vector<double> &jack_data, int jack_sample)
{

    if (jack_sample < 0 || jack_sample >= data.size())
    {
        std::cerr << "jack_sample=" << jack_sample << " outside allowable range [0, " << data.size() - 1 << "]" << std::endl;
        exit(1);
    }

    jack_data.resize(0);
    for (int i = 0; i < data.size(); i++)
    {
        if (i != jack_sample)
        {
            jack_data.push_back(data[i]);
        }
    }
}


// Compute the jackknife error
double jack_error(const std::vector<double> &data)
{
    double m = mean(data); // Get original mean
    double sumsq = 0;            // Use this for variance: Sum ( jackMean - m)^2

    // Compute mean on each jackknife sample
    for (int i = 0; i < data.size(); i++)
    {
        std::vector<double> jack_sample;

        jack_set(data, jack_sample, i);       // Get i-th jackknife sample
        double jackMean = mean(jack_sample); // Compute ith jackknife mean
        sumsq += (jackMean - m) * (jackMean - m);  // accumulate variance term
    }

    // Normalize variance
    sumsq *= (double)(data.size() - 1) / (double)data.size();
    return std::sqrt(sumsq); // return square root of variance ie error
}


// Function to calculate averages of bin-sized parts of data
std::vector<double> bin_data(const std::vector<double>& data, int bin_size)
{
    std::vector<double> bin_averages;  // Vector to store the averages
    int data_size = data.size();
    int index = 0;  // Track the index in the data

    // Loop through the data and process full bins
    while (index + bin_size <= data_size) {
        double sum = std::accumulate(data.begin() + index, data.begin() + index + bin_size, 0.0);
        double average = sum / bin_size;
        bin_averages.push_back(average);
        index += bin_size;
    }

    // Handle any remaining elements (remainder) for the last bin
    if (index < data_size) {
        double sum = std::accumulate(data.begin() + index, data.end(), 0.0);
        double average = sum / (data_size - index);  // Average of remaining elements
        bin_averages.push_back(average);
    }

    return bin_averages;

}

// Returns a single bootstrap sample from a vector of double
inline std::vector<double> bootstrap_generate_sample(const std::vector<double>& data)
{
    // Random number generator with a non-deterministic seed
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, data.size() - 1);

    // Create a bootstrap sample by randomly selecting elements with replacement
    std::vector<double> bootstrapSample;
    bootstrapSample.reserve(data.size());  // Reserve space for efficiency

    for (std::size_t i = 0; i < data.size(); ++i) {
        int index = dist(gen);
        bootstrapSample.push_back(data[index]);
    }

    return bootstrapSample;
}


// Bootstrap estimate of Standard Error
inline double bootstrap_stdError(const std::vector<double> bootReplicParam)
{
  /* Bootstrap estimate of standard error base on 
   * Efron, B., & Tibshirani, R.J. (1994). An Introduction to the Bootstrap (1st ed.). Chapman and Hall/CRC (p.48)
   *
   */
  
  double B = bootReplicParam.size(); // Number of replication of the estimator param.
  double replicMean = mean(bootReplicParam); // Mean of the bootstrap replications
  double stdErr = 0; // standard error 

  for (int i=0;  i < B; i++ )
  {
    stdErr += (bootReplicParam[i] - replicMean)*(bootReplicParam[i] - replicMean); 
  }
 
  stdErr = std::sqrt(stdErr / (B-1));
  
  return stdErr
}

#endif //stattools.hpp
