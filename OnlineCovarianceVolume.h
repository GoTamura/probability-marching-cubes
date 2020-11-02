#pragma once
#include <vector>
#include <cmath>
#include <kvs/ValueArray>
#include <kvs/StructuredVolumeObject>

#include <string>
#include <functional>

std::vector<float> cholesky_decomposition(std::vector<float> &cov);

class OnlineCov
{
private:
  double mean1;
  double mean2;
  double cov;
  long long int num;

public:
  OnlineCov() : mean1(0), mean2(0), cov(0), num(0){};
  void addValue(float value1, float value2)
  {
    num++;
    const float delta1 = (value1 - mean1) / (float)num;
    const float delta2 = (value2 - mean2) / (float)num;
    mean1 += delta1;
    mean2 += delta2;
    cov += (num - 1) * delta1 * delta2 - cov / (float)num;
  }
  float covariance()
  {
    return cov * num / (num - 1);
  }
  float average()
  {
    return mean1;
  }
};

class OnlineCovMatrix
{
private:
  std::vector<OnlineCov> covMatrix;
  /*
   *  Covariance index
   *    a0  a1  a2  a3
   * a0  0   1   2   3
   * a1      4   5   6
   * a2          7   8
   * a3              9
   */
  const int n = 8;

public:
  OnlineCovMatrix()
  {
    covMatrix = std::vector<OnlineCov>((n + 1) * n / 2);
  }
  void addValue(std::vector<float> a)
  {
    int count = 0;
    for (int i = 0; i < n; ++i)
    {
      for (int j = i; j < n; ++j)
      {
        covMatrix[count++].addValue(a[i], a[j]);
      }
    }
  }
  std::vector<float> average()
  {
    std::vector<float> ret(n);
    for (int i = 0; i < n; ++i)
    {
      ret[i] = covMatrix[i].average();
    }
    return ret;
  }

  std::vector<float> covariance_matrix()
  {
    std::vector<float> ret(n * n);
    int count = 0;
    for (int i = 0; i < n; ++i)
    {
      for (int j = i; j < n; ++j)
      {
        ret[n * i + j] = covMatrix[count++].covariance();
        ret[n * j + i] = ret[n * i + j];
      }
    }
    return ret;
  }
};

class OnlineCovMatrixVolume
{
private:
  std::vector<OnlineCovMatrix> covVol;
  const int _x;
  const int _y;
  const int _z;

public:
  OnlineCovMatrixVolume(int x, int y, int z) : _x(x), _y(y), _z(z)
  {
    std:: cout << _x << _y << _z << std::endl;
    covVol = std::vector<OnlineCovMatrix>(_x * _y * _z);
    std:: cout << "vec2" << std::endl;
  }
  void addArray(kvs::ValueArray<float> array)
  {
    int count = 0;
    for (int i = 0; i < _z; ++i)
    {
      for (int j = 0; j < _y; ++j)
      {
        for (int k = 0; k < _x; ++k)
        {
          std::vector<float> value = {
              array[(_y + 1) * (_x + 1) * i + (_x + 1) * j + k],
              array[(_y + 1) * (_x + 1) * i + (_x + 1) * j + k + 1],
              array[(_y + 1) * (_x + 1) * i + (_x + 1) * (j + 1) + k],
              array[(_y + 1) * (_x + 1) * i + (_x + 1) * (j + 1) + k + 1],
              array[(_y + 1) * (_x + 1) * (i + 1) + (_x + 1) * j + k],
              array[(_y + 1) * (_x + 1) * (i + 1) + (_x + 1) * j + k + 1],
              array[(_y + 1) * (_x + 1) * (i + 1) + (_x + 1) * (j + 1) + k],
              array[(_y + 1) * (_x + 1) * (i + 1) + (_x + 1) * (j + 1) + k + 1],
          };
          covVol[count++].addValue(value);
        }
      }
    }
  }
  std::vector<std::vector<float>> average_matrix()
  {
    std::vector<std::vector<float>> ret(_x * _y * _z);
    for (int i = 0; i < _z * _y * _x; ++i)
    {
      ret[i] = covVol[i].average();
    }
    return ret;
  }

  kvs::ValueArray<float> variance()
  {
    kvs::ValueArray<float> ret(_x * _y * _z);
    for (int i = 0; i < _z * _y * _x; ++i)
    {
      ret[i] = covVol[i].covariance_matrix()[0];
    }
    return ret;
  }

  kvs::ValueArray<float> average()
  {
    kvs::ValueArray<float> ret(_x * _y * _z);
    for (int i = 0; i < _z * _y * _x; ++i)
    {
      ret[i] = covVol[i].average()[0];
    }
    return ret;
  }

  std::vector<std::vector<float>> covariance_volume()
  {
    const int n = 8;
    std::vector<std::vector<float>> ret(_x * _y * _z, std::vector<float>(n * n));
    int count = 0;
    for (auto &&i : ret)
    {
      i = covVol[count].covariance_matrix();
    }
    return ret;
  }

  std::vector<std::vector<float>> cholesky_covariance_volume()
  {
    const int n = 8;
    std::vector<std::vector<float>> ret(_x * _y * _z, std::vector<float>(n * n));
    int count = 0;
    for (auto &&i : ret)
    {
      i = covVol[count].covariance_matrix();
      i = cholesky_decomposition(i);
    }
    return ret;
  }
};

class OnlineCovMatrixVolumeCalcurator : public OnlineCovMatrixVolume
{
private:
  std::function<void(const std::string &, kvs::StructuredVolumeObject &)> loadFunction;

public:
  void addFile(const std::string &file)
  {
    kvs::StructuredVolumeObject vol;
    loadFunction(file, vol);
    addArray(vol.values().asValueArray<float>());
  }

  void addFiles(const std::vector<std::string> &files)
  {
    for (const auto &file : files)
    {
      addFile(file);
    }
  }

  OnlineCovMatrixVolumeCalcurator(const int x, const int y, const int z, const std::vector<std::string> &files, std::function<void(const std::string &, kvs::StructuredVolumeObject &)> lf) : loadFunction(lf), OnlineCovMatrixVolume(x, y, z)
  {
    addFiles(files);
  }
};
