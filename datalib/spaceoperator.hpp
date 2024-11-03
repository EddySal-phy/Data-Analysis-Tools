#ifndef SPACEOPERATOR_HPP
#define SPACEOPERATOR_HPP

#include "filehandler.hpp"


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

#endif // SPACEOPERATOR_HPP