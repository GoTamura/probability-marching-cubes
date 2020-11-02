#include "Program8.h"

namespace Program8
{
  // calcurate score on isosurface and output to file
  // ./command <truth directory> <score directory>
  int Program::exec(int argc, char **argv)
  {
    for (int i = 1; i <= 60; ++i)
    {
      auto true_vol = kvs::StructuredVolumeObject();
      WeatherData::loadWeatherData(argv[1], WeatherData::Parameter::QV, true_vol);

      auto score_vol = kvs::StructuredVolumeObject();
      // 拡張子を.kvsmlにする必要がある
      score_vol.read(argv[2] + "score" + std::to_string(i) + ".kvsml");

      const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
      const bool d = false;
      const float threshold = 0.01;
      const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
      kvs::PolygonObject *object = new kvs::Isosurface(&true_vol, threshold, n, d, t);
      float score = sum_score(score_vol.values().asValueArray<float>(), object->coords(), t, Tools::NX-1, Tools::NY-1, Tools::NZ-1);
    }
  }
} // namespace Program4