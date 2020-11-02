#include "Program7.h"

namespace Program7
{
  // calcurate probabilistic marching cubes and output to file
  // ./command <ensemble directory> <probability output filename> <score output filename>
  int Program::exec(int argc, char **argv)
  {
    for (int i = 1; i <= 60; ++i)
    {
      // Calcurate pmc
      kvs::ValueArray<float> prob;
      kvs::ValueArray<float> length_array;
      kvs::ValueArray<float> score;
      std::string ensemble_path = argv[1];
      //Tools::calc_pmc(i, prob, length_array, score, ensemble_path);

      auto files = Tools::sameTime(i, ensemble_path);
      auto ocmvCalc = OnlineCovMatrixVolumeCalcurator(Tools::NX - 1, Tools::NY - 1, Tools::NZ - 1, files, Tools::loadQV);

      const auto average_array = ocmvCalc.average();
      const auto ave_matrix = ocmvCalc.average_matrix();
      const auto cov_matrix = ocmvCalc.cholesky_covariance_volume();
      const auto variance_array = ocmvCalc.variance();

      const float threshold = 0.01;
      const int samples = 100;
      prob = ProbabilisticMarchingCubes::calc_pdf(cov_matrix, ave_matrix, threshold, samples);
      length_array = calc_length_volume(average_array, threshold, Tools::NX - 1, Tools::NY - 1, Tools::NZ - 1);
      score = calc_point(length_array, prob, Tools::NX - 1, Tools::NY - 1, Tools::NZ - 1);

      auto prob_vol = new kvs::StructuredVolumeObject();
      prob_vol->setGridTypeToUniform();
      prob_vol->setVeclen(1);
      prob_vol->setResolution(kvs::Vector3ui(Tools::NX-1, Tools::NY-1, Tools::NZ-1));
      prob_vol->setValues(prob);
      prob_vol->updateMinMaxValues();
      prob_vol->write(argv[2] + std::to_string(i), false, true);

      auto score_vol = new kvs::StructuredVolumeObject();
      score_vol->setGridTypeToUniform();
      score_vol->setVeclen(1);
      score_vol->setResolution(kvs::Vector3ui(Tools::NX-1, Tools::NY-1, Tools::NZ-1));
      score_vol->setValues(score);
      score_vol->updateMinMaxValues();
      score_vol->write(argv[3] + std::to_string(i), false, true);
    }
  }
} // namespace Program7