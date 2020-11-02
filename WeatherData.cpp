#include "WeatherData.h"

kvs::ValueArray<float> WeatherData::loadBinary(std::ifstream &ifs, const int size)
{
  kvs::ValueArray<float> array(size);
  ifs.read((char *)array.data(), size * 4);

  // convert endian
  if (kvs::Endian::IsLittle())
  {
    for (auto &&i : array)
    {
      kvs::Endian::Swap(&i);
    }
  }
  return array;
}

void WeatherData::loadVolume(std::ifstream &ifs, const int size, kvs::StructuredVolumeObject &vol)
{
  kvs::ValueArray<float> binary = loadBinary(ifs, size);

  vol.setGridTypeToUniform();
  vol.setVeclen(1);
  vol.setResolution(kvs::Vector3ui(NX, NY, NZ));
  vol.setValues(binary);
  vol.updateMinMaxValues();

  //vol->setGridTypeToRectilinear();
  //vol->setCoords(coords.clone());
  //vol->updateMinMaxCoords();
}

int WeatherData::loadWeatherData(std::string filename, Parameter p, kvs::StructuredVolumeObject &vol)
{
  std::ifstream ifs(filename, std::ios::in | std::ios::binary);
  if (ifs.fail())
  {
    std::cerr << "File not found" << std::endl;
    return -1;
  }

  // 格子点数 * sizeof(float) * 変数の順番
  ifs.seekg(SIZE * 4 * p, std::ios_base::beg);
  loadVolume(ifs, SIZE, vol);
  ifs.close();
  return 0;
}

//kvs::ValueArray<float> createCoords(int nx, int ny, int nz) {
//  // x方向は1.09545294622*10^-3°間隔
//  std::vector<float> x(nx);
//  float radius_earth = 6360000.0; // [m]
//  // 基準地点=(東経133.590905 , 北緯33.51538902)
//  float latitude = 33.5153;
//  float longitude_interval = 0.00109545294622;
//  float radius_point = radius_earth * cos(latitude/360.0);
//  float circumference_point = radius_point * 2.0 * 3.141592;
//
//  for (int i = 0; i < nx; ++i) {
//    x[i] = (i-nx/2.) * circumference_point * longitude_interval / 360.0;
//  }
//  // y方向は8.99279260651*10^-4°間隔
//  float latitude_interval = 0.000899279260;
//  std::vector<float> y(ny);
//  for (int i = 0; i < ny; ++i) {
//    y[i] = (i-ny/2.) * radius_earth * 2.0 * 3.141592 * latitude_interval / 360.0;
//  }
//
//  // z方向 : (単位はメートル[m])
//  //   1   -20.0000  2    20.0000  3    60.0000  4   118.0000  5   194.0000
//  //   6   288.0000  7   400.0000  8   530.0000  9   678.0000 10   844.0000
//  //  11  1028.0000 12  1230.0000 13  1450.0000 14  1688.0000 15  1944.0000
//  //  16  2218.0000 17  2510.0000 18  2820.0000 19  3148.0000 20  3494.0000
//  //  21  3858.0000 22  4240.0000 23  4640.0000 24  5058.0000 25  5494.0000
//  //  26  5948.0000 27  6420.0000 28  6910.0000 29  7418.0000 30  7944.0000
//  //  31  8488.0000 32  9050.0000 33  9630.0000 34 10228.0000 35 10844.0000
//  //  36 11478.0000 37 12130.0000 38 12800.0000 39 13488.0000 40 14194.0000
//  //  41 14918.0000 42 15660.0000 43 16420.0000 44 17198.0000 45 17994.0000
//  //  46 18808.0000 47 19640.0000 48 20490.0000 49 21358.0000 50 22244.0000
//  std::vector<float> z = {
//        -20.0000  ,    20.0000  ,    60.0000  ,   118.0000  ,   194.0000
//     ,   288.0000  ,   400.0000  ,   530.0000  ,   678.0000 ,   844.0000
//    ,  1028.0000 , 1230.0000 ,  1450.0000 ,  1688.0000 ,  1944.0000
//    ,  2218.0000 , 2510.0000 ,  2820.0000 ,  3148.0000 ,  3494.0000
//    ,  3858.0000 , 4240.0000 ,  4640.0000 ,  5058.0000 ,  5494.0000
//    ,  5948.0000 , 6420.0000 ,  6910.0000 ,  7418.0000 ,  7944.0000
//    ,  8488.0000 , 9050.0000 ,  9630.0000 , 10228.0000 , 10844.0000
//    , 11478.0000 ,12130.0000 , 12800.0000 , 13488.0000 , 14194.0000
//    , 14918.0000 ,15660.0000 , 16420.0000 , 17198.0000 , 17994.0000
//    , 18808.0000 ,19640.0000 , 20490.0000 , 21358.0000 , 22244.0000
//  };
//
//  // normalize
//  // 倍率は適当
//  auto norm = std::max(x[nx-1], std::max(y[ny-1], z[nz-1]));
//  for (int i = 0; i < nx; ++i) {
//    x[i] /= norm/301.0;
//  }
//  for (int i = 0; i < ny; ++i) {
//    y[i] /= norm/301.0;
//  }
//  for (int i = 0; i < nz; ++i) {
//    z[i] /= norm/301.0;
//  }
//
//  x.insert(x.end(),y.begin(), y.end());
//  x.insert(x.end(),z.begin(), z.end());
//  return kvs::ValueArray<float>(x);
//}