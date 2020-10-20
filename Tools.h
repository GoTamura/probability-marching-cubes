#pragma once
#include <iostream>
#include <cmath>
#include <string>
#include <functional>

#include <kvs/ValueArray>
#include <kvs/StructuredVolumeObject>
#include <kvs/PolygonObject>

#include "WeatherData.h"
#include "OnlineCovarianceVolume.h"
#include "ProbabilisticMarchingCubes.h"
#include "Length.h"

static const int NX = 301;
static const int NY = 301;
static const int NZ = 50;

void loadQV(const std::string &file, kvs::StructuredVolumeObject &vol);

std::string truth_filename(const std::string &path, const int i);
std::string ensemble_filename(const std::string &path, const int member, const int time);
// Make a list of files to load
std::vector<std::string> sameTime(const int nloops, const std::string &ensemble_path);

void calc_pmc(const int nloops, kvs::ValueArray<float> &prob, kvs::ValueArray<float> &length_array, kvs::ValueArray<float> &point, const std::string ensemble_path);

void write_kvsml(std::string file, const kvs::ValueArray<float> &array, kvs::StructuredVolumeObject *vol);

void read_kvsml(std::string file, kvs::StructuredVolumeObject *vol);

void setVisibleArea(kvs::PolygonObject *object);

kvs::ValueArray<kvs::Real32> makeAlpha();