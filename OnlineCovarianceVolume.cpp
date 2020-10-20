#include "OnlineCovarianceVolume.h"

std::vector<float> cholesky_decomposition(std::vector<float> &cov)
{
  int n = 8;
  std::vector<float> l(n * n);
  float epsilon = 0.000001;
  for (int i = 0; i < n; ++i)
  {
    for (int k = 0; k < (i + 1); ++k)
    {
      double sum = 0;
      for (int j = 0; j < k; ++j)
      {
        sum += l[i * n + j] * l[k * n + j];
      }
      if (i == k)
      {
        l[i * n + k] = std::sqrt(cov[i * n + i] + epsilon - sum);
      }
      else
      {
        l[i * n + k] = 1.0 / l[k * n + k] * (cov[i * n + k] - sum);
      }
    }
  }
  return l;
}
