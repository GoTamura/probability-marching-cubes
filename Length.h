#pragma once
#include <kvs/ValueArray>
#include <kvs/ColorMap>
#include <kvs/TransferFunction>
#include <algorithm>

kvs::ValueArray<float> calc_length_volume(const kvs::ValueArray<float> &array, const float theta, const int x, const int y, const int z);
kvs::ValueArray<float> calc_point(const kvs::ValueArray<float> &prob, const kvs::ValueArray<float> &length, const int x, const int y, const int z);

kvs::ValueArray<kvs::UInt8> point_to_color(const kvs::ValueArray<float> &point, const kvs::ValueArray<kvs::Real32> &coords, const kvs::TransferFunction &tfunc, const int x, const int y, const int z);
