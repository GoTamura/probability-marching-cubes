#include "Tools.h"
void loadQV(const std::string &file, kvs::StructuredVolumeObject &vol)
{
    WeatherData::loadWeatherData(file, WeatherData::Parameter::QV, vol);
}

// time >= 1
std::string truth_filename(const std::string &path, const int time)
{
    std::stringstream ss;
    // path/true2008/0300.bin
    ss << path << "/true2008" << std::setw(4) << std::setfill('0') << time + 299 << ".bin";
    return ss.str();
}

// time >= 1
std::string ensemble_filename(const std::string &path, const int member, const int time)
{
    std::stringstream ss;
    // path/001/gs0060.bin
    ss << path << "/" << std::setw(3) << std::setfill('0') << member << "/gs" << std::setw(4) << std::setfill('0') << time + 59 << ".bin";
    return ss.str();
}

// Make a list of files to load
std::vector<std::string> sameTime(const int nloops, const std::string &ensemble_path)
{
    std::vector<std::string> files;
    for (int i = 1; i <= 20; ++i)
    {
        auto filename = ensemble_filename(ensemble_path, i, nloops);
        files.push_back(filename);
    }
    return files;
}

void calc_pmc(const int nloops, kvs::ValueArray<float> &prob, kvs::ValueArray<float> &length_array, kvs::ValueArray<float> &point, const std::string &ensemble_path)
{
    auto files = sameTime(nloops, ensemble_path);
    auto ocmvCalc = OnlineCovMatrixVolumeCalcurator(NX - 1, NY - 1, NZ - 1, files, loadQV);

    const auto average_array = ocmvCalc.average();
    const auto ave_matrix = ocmvCalc.average_matrix();
    const auto cov_matrix = ocmvCalc.cholesky_covariance_volume();
    const auto variance_array = ocmvCalc.variance();

    const float threshold = 0.01;
    const int samples = 100;
    prob = ProbabilisticMarchingCubes::calc_pdf(cov_matrix, ave_matrix, threshold, samples);
    length_array = calc_length_volume(average_array, threshold, NX - 1, NY - 1, NZ - 1);
    point = calc_point(length_array, prob, NX - 1, NY - 1, NZ - 1);
}

void write_kvsml(std::string file, const kvs::ValueArray<float> &array, kvs::StructuredVolumeObject *vol)
{
    vol->setGridTypeToUniform();
    vol->setVeclen(1);
    vol->setResolution(kvs::Vector3ui(NX - 1, NY - 1, NZ - 1));
    vol->setValues(array);
    vol->updateMinMaxValues();
    vol->write(file, false, true);
}

void read_kvsml(std::string file, kvs::StructuredVolumeObject *vol)
{
    vol->read(file);
    vol->setMinMaxExternalCoords(vol->minObjectCoord(), vol->maxObjectCoord() * kvs::Vec3(1, 1, 5));
}

void setVisibleArea(kvs::PolygonObject *object)
{
    kvs::ValueArray<kvs::UInt8> opacities(object->numberOfVertices());
    for (int i = 0; i < object->numberOfVertices(); ++i)
    {
        opacities[i] = 255;
        auto x = object->coords()[3 * i];
        auto y = object->coords()[3 * i + 1];
        auto z = object->coords()[3 * i + 2];
        if (!(90 <= x && x <= 210 && 90 <= y && y <= 210))
        {
            opacities[i] = 0;
        }
    }
    object->setOpacities(opacities);
}

kvs::ValueArray<kvs::Real32> makeAlpha()
{
    auto volume = kvs::ValueArray<kvs::Real32>(301 * 301 * 50);
    for (int i = 0; i < 50; ++i)
    {
        for (int j = 0; j < 301; ++j)
        {
            for (int k = 0; k < 301; ++k)
            {
                volume[i * 301 * 301 + j * 301 + k] = 0.0;
                //if (90 <= j && j <= 210 && 90 <= k && k <= 210)
                volume[i * 301 * 301 + j * 301 + k] = 1.0;
            }
        }
    }
    return volume;
}