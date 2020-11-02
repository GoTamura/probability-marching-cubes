#include "Program4.h"

namespace Program4
{
  // calcurate RMSE and output to file
  // ./command <truth directory> <ensemble directory> <output filename>
  int Program::exec(int argc, char **argv)
  {
    for (int i = 1; i <= 100; ++i)
    {
      auto true_vol = kvs::StructuredVolumeObject();
      WeatherData::loadWeatherData(Tools::truth_filename(argv[1], i), WeatherData::Parameter::QV, true_vol);

      auto files = Tools::sameTime(i, argv[2]);
      auto rmseCalc = RMSECalcurator(Tools::NX, Tools::NY, Tools::NZ, true_vol.values().asValueArray<float>(), files, Tools::loadQV);
      auto rmse_vol = new kvs::StructuredVolumeObject();
      rmse_vol->setGridTypeToUniform();
      rmse_vol->setVeclen(1);
      rmse_vol->setResolution(kvs::Vector3ui(Tools::NX, Tools::NY, Tools::NZ));
      rmse_vol->setValues(rmseCalc.get());
      rmse_vol->updateMinMaxValues();
      rmse_vol->write(argv[3] + std::to_string(i), false, true);
    }
  }
} // namespace Program4