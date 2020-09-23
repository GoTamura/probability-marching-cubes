#include "ProbabilisticMarchingCubes.h"

bool ProbabilisticMarchingCubes::crossing(std::vector<float> y, float threshold) {
  if (y[0] < threshold && y[1] < threshold && y[2] < threshold && y[3] < threshold && y[4] < threshold && y[5] < threshold && y[6] < threshold && y[7] < threshold) return false;
  if (y[0] > threshold && y[1] > threshold && y[2] > threshold && y[3] > threshold && y[4] > threshold && y[5] > threshold && y[6] > threshold && y[7] > threshold) return false;
  return true;
}

std::vector<float> ProbabilisticMarchingCubes::make_multivariate_distribution(const std::vector<float>& dist, const std::vector<float>& cov, const std::vector<float>& mean) {
  std::vector<float> ret(8);
  for (int i = 0; i < 8; ++i) {
    //for (int j = 0; j < i+1; ++j) {
    for (int j = 0; j < 8; ++j) {
      ret[i] += cov[i*8+j] * dist[j];
    }
    ret[i] += mean[i];
  }
  return ret;
}

kvs::ValueArray<float> ProbabilisticMarchingCubes::calc_pdf(const std::vector<std::vector<float>>& cov, const std::vector<std::vector<float>>& mean, const float threshold, const int samples) {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::normal_distribution<> normal_dist(0.0, 1.0);
  int size = mean.size();
  kvs::ValueArray<float> prob(size);

  #if defined(OMP)
  #pragma omp parallel for
  #endif
  for (int i = 0; i < size; ++i) {
    int crossings = 0;
    for (int j = 0; j < samples; ++j) {
      std::vector<float> dist_array(8);
      std::generate(dist_array.begin(), dist_array.end(), [&]() {return normal_dist(engine);});
      dist_array = make_multivariate_distribution(dist_array, cov[i], mean[i]);
      if (crossing(dist_array, threshold)) crossings++;
    }
    prob[i] = (float)crossings / (float)samples;
  }
  return prob;
}

