#include "Program6.h"

namespace Program6
{
  // calcurate average and output to file
  // ./command <ensemble directory> <output filename>
  int Program::exec(int argc, char **argv)
  {
    for (int i = 1; i <= 100; ++i)
    {
      auto files = Tools::sameTime(i, argv[2]);
      auto ocmvCalc = OnlineCovMatrixVolumeCalcurator(Tools::NX-1, Tools::NY-1, Tools::NZ-1, files, Tools::loadQV);
      auto ocmv_vol = new kvs::StructuredVolumeObject();
      ocmv_vol->setGridTypeToUniform();
      ocmv_vol->setVeclen(1);
      ocmv_vol->setResolution(kvs::Vector3ui(Tools::NX-1, Tools::NY-1, Tools::NZ-1));
      ocmv_vol->setValues(ocmvCalc.average());
      ocmv_vol->updateMinMaxValues();
      ocmv_vol->write(argv[3] + std::to_string(i), false, true);
    }
  }
} // namespace Program4