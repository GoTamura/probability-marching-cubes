#pragma once

#include <fstream>
#include <vector>
#include <kvs/StructuredVolumeObject>
#include <kvs/ValueArray>
#include <kvs/Endian>

class WeatherData {
public:
  enum Parameter {
    U,   // 東西風 : X-wind component (m s-1)
    V,   // 南北風 : Y-wind component (m s-1)
    W,   // 鉛直風 : Z-wind component (m s-1)
    T,   // 気温   : Temperature (K)
    P,   // 気圧   : Pressure (Pa)
    QV,  // 水蒸気混合比 : Water vapor mixing ratio (kg kg-1) 
    QC,  // 雲水混合比   : Cloud water mixing ratio (kg kg-1)
    QR,  // 雨混合比     : Rain mixing ratio (kg kg-1)
    QCI, // 雲氷混合比   : Cloud ice mixing ratio (kg kg-1)
    QS,  // 雪混合比     : Snow mixing ratio (kg kg-1)
    QG   // あられ混合比 : Graupel mixing ratio (kg kg-1)
  };
  static kvs::StructuredVolumeObject *loadWeatherData(std::string filename, Parameter p);
private:
  static const int NX = 301;
  static const int NY = 301;
  static const int NZ = 50;
  static const int SIZE = NX*NY*NZ;
  static kvs::StructuredVolumeObject *loadVolume(std::ifstream &ifs, const int size);
  static kvs::ValueArray<float> loadBinary(std::ifstream &ifs, const int size);
};
