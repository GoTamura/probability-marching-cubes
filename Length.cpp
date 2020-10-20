#include "Length.h"

kvs::ValueArray<float> calc_length_volume(const kvs::ValueArray<float> &array, const float theta, const int x, const int y, const int z)
{
  kvs::ValueArray<float> length(x * y * z);
  for (int i = 0; i < x; ++i)
  {
    for (int j = 0; j < y; ++j)
    {
      float min = 10000000;
      float min_index = 0;
      for (int k = 0; k < z; ++k)
      {
        float diff = std::abs(array[k * y * x + j * x + i] - theta);
        if (diff < min)
        {
          min = diff;
          min_index = k;
        }
      }
      for (int k = 0; k < z; ++k)
      {
        length[k * y * x + j * x + i] = std::abs(k - min_index);
      }
    }
  }
  return length;
}

kvs::ValueArray<float> calc_point(const kvs::ValueArray<float> &prob, const kvs::ValueArray<float> &length, const int x, const int y, const int z)
{
  kvs::ValueArray<float> ret(x * y * z);
  for (int i = 0; i < x * y * z; ++i)
  {
    ret[i] = prob[i] / (length[i] + 1);
  }
  return ret;
}

kvs::ValueArray<kvs::UInt8> point_to_color(const kvs::ValueArray<float> &point, const kvs::ValueArray<kvs::Real32> &coords, const kvs::TransferFunction &tfunc, const int x, const int y, const int z)
{
  kvs::ValueArray<kvs::UInt8> colors(coords.size());
  const kvs::ColorMap &color_map(tfunc.colorMap());
  for (int i = 0; i < coords.size() / 3; ++i)
  {
    int pos_x = (int)coords[3 * i];
    int pos_y = (int)coords[3 * i + 1];
    int pos_z = (int)coords[3 * i + 2];
    if (pos_x >= x)
      pos_x--;
    if (pos_y >= y)
      pos_y--;
    if (pos_z >= z)
      pos_z--;
    int position = pos_z * x * y + pos_y * x + pos_x;
    kvs::UInt8 value = static_cast<kvs::UInt8>(255 * point[position]);
    colors[3 * i] = color_map[value].r();
    colors[3 * i + 1] = color_map[value].g();
    colors[3 * i + 2] = color_map[value].b();
  }
  return colors;
}
