#ifndef PROBABILITY_MARCHING_CUBES_H_INCLUDE
#define PROBABILITY_MARCHING_CUBES_H_INCLUDE
#include <vector>
#include <random>
#include <kvs/ValueArray>
#include <omp.h>

class ProbabilisticMarchingCubes {
  static bool crossing(std::vector<float> y, float threshold);
  static std::vector<float>make_multivariate_distribution(const std::vector<float>& dist, const std::vector<float>& cov, const std::vector<float>& mean);
public:

  static kvs::ValueArray<float> calc_pdf(const std::vector<std::vector<float>>& cov, const std::vector<std::vector<float>>& mean, const float threshold, const int samples);
};

#endif // PROBABILITY_MARCHING_CUBES_H_INCLUDE
