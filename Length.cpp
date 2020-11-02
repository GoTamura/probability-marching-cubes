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

struct VolumeSize {
  int x;
  int y;
  int z;
};

struct iPosition {
  int x;
  int y;
  int z;
};

int cube_vertex_index(int i, VolumeSize size, iPosition pos) {
  constexpr int  a[8] = {0, 1, 1, 0, 0, 1, 1, 0};
  constexpr int  b[8] = {0, 0, 1, 1, 0, 0, 1, 1};
  constexpr int  c[8] = {0, 0, 0, 0, 1, 1, 1, 1};
  return (pos.z+c[i])*size.x*size.y + (pos.y+b[i])*size.x + (pos.x+a[i]);
}

kvs::ValueArray<kvs::UInt8> point_to_color(const kvs::ValueArray<float> &point, const kvs::ValueArray<kvs::Real32> &coords, const kvs::TransferFunction &tfunc, const int x, const int y, const int z)
{
  kvs::ValueArray<kvs::UInt8> colors(coords.size());
  const kvs::ColorMap &color_map(tfunc.colorMap());
  for (int i = 0; i < coords.size() / 3; ++i)
  {
      float pos_x = coords[3 * i];
      float pos_y = coords[3 * i + 1];
      float pos_z = coords[3 * i + 2];
      int ipos_x = std::floor(pos_x);
      int ipos_y = std::floor(pos_y);
      int ipos_z = std::floor(pos_z);
      float fpos_x = pos_x - ipos_x;
      float fpos_y = pos_y - ipos_y;
      float fpos_z = pos_z - ipos_z;
      if (ipos_x >= x)
        ipos_x--;
      if (ipos_y >= y)
        ipos_y--;
      if (ipos_z >= z)
        ipos_z--;
      float value = 0;
      for (int i = 0; i < 8; i++) {
        constexpr int  ak[8] = {-1, 1, 1, -1, -1, 1, 1, -1};
        constexpr int  bk[8] = {-1, -1, 1, 1, -1, -1, 1, 1};
        constexpr int  ck[8] = {-1, -1, -1, -1, 1, 1, 1, 1};
        int position = cube_vertex_index(i, VolumeSize{x, y, z}, iPosition{ipos_x, ipos_y, ipos_z});
        float N = (1+ak[i]*fpos_x)*(1+bk[i]*fpos_y)*(1+ck[i]*fpos_z);
        value += N * point[position] / 8.;
      }
      kvs::UInt8 ivalue = static_cast<kvs::UInt8>(255 * value);
      colors[3 * i]     = color_map[ivalue].r();
      colors[3 * i + 1] = color_map[ivalue].g();
      colors[3 * i + 2] = color_map[ivalue].b();
  }
  return colors;
}

float sum_score(const kvs::ValueArray<float> &point, const kvs::ValueArray<kvs::Real32> &coords, const kvs::TransferFunction &tfunc, const int x, const int y, const int z)
{
  float sum = 0;
  for (int i = 0; i < coords.size() / 3; ++i)
  {
      float pos_x = coords[3 * i];
      float pos_y = coords[3 * i + 1];
      float pos_z = coords[3 * i + 2];
      int ipos_x = std::floor(pos_x);
      int ipos_y = std::floor(pos_y);
      int ipos_z = std::floor(pos_z);
      float fpos_x = pos_x - ipos_x;
      float fpos_y = pos_y - ipos_y;
      float fpos_z = pos_z - ipos_z;
      if (ipos_x >= x)
        ipos_x--;
      if (ipos_y >= y)
        ipos_y--;
      if (ipos_z >= z)
        ipos_z--;
      float value = 0;
      for (int i = 0; i < 8; i++) {
        constexpr int  ak[8] = {-1, 1, 1, -1, -1, 1, 1, -1};
        constexpr int  bk[8] = {-1, -1, 1, 1, -1, -1, 1, 1};
        constexpr int  ck[8] = {-1, -1, -1, -1, 1, 1, 1, 1};
        int position = cube_vertex_index(i, VolumeSize{x, y, z}, iPosition{ipos_x, ipos_y, ipos_z});
        float N = (1+ak[i]*fpos_x)*(1+bk[i]*fpos_y)*(1+ck[i]*fpos_z);
        value += N * point[position] / 8.;
      }
      sum += value;
  }
  return sum;
}
