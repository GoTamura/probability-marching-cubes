#include "Program8.h"

namespace Program8
{
  // calcurate average and output to file
  // ./command <ensemble directory> <output filename>
  int Program::exec(int argc, char **argv)
  {
    for (int i = 1; i <= 100; ++i)
    {
      auto true_vol = kvs::StructuredVolumeObject();
      WeatherData::loadWeatherData(argv[1], WeatherData::Parameter::QV, true_vol);

      auto score_vol = kvs::StructuredVolumeObject();
      score_vol.read(argv[2]);

      const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
      const bool d = false;
      const float threshold = 0.01;
      const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
      kvs::PolygonObject *object = new kvs::Isosurface(&true_vol, threshold, n, d, t);
      float score = sum_score(score_vol.values().asValueArray<float>(), object->coords(), t, Tools::NX-1, Tools::NY-1, Tools::NZ-1);
    }
  }
} // namespace Program4