#pragma once
#include <cmath>
#include <kvs/ValueArray>
#include <kvs/StructuredVolumeObject>

#include <string>
#include <functional>
class RMSE
{
public:
    kvs::ValueArray<float> truth;
    kvs::ValueArray<float> mean;
    long long int num;
    RMSE(int size)
    {
        mean = kvs::ValueArray<float>(size);
    }
    void add(kvs::ValueArray<float> array)
    {
        num++;
        for (int i = 0; i < mean.size(); i++)
        {
            double diff = array[i] - truth[i];
            diff *= diff;
            const double delta = (diff - mean[i]) / (float)num;
            mean[i] += delta;
        }
    }

    kvs::ValueArray<float> get()
    {
        auto rmse = kvs::ValueArray<float>(mean.size());
        for (int i = 0; i < mean.size(); i++)
        {
            rmse[i] = std::sqrt(mean[i]);
        }
        return rmse;
    }
};

class RMSECalcurator : public RMSE
{
    std::function<void(const std::string &, kvs::StructuredVolumeObject &)> loadFunction;

public:
    void addFile(const std::string &file)
    {
        kvs::StructuredVolumeObject vol;
        loadFunction(file, vol);
        add(vol.values().asValueArray<float>());
    }

    void addFiles(const std::vector<std::string> &files)
    {
        for (const auto &file : files)
        {
            addFile(file);
        }
    }

    RMSECalcurator(int x, int y, int z, kvs::ValueArray<float> _truth, std::vector<std::string> files, std::function<void(const std::string &, kvs::StructuredVolumeObject &)> lf) : loadFunction(lf), RMSE(x * y * z)
    {
        truth = _truth;
        addFiles(files);
    }
};
