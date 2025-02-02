#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include <kvs/Program>
#include <kvs/StructuredVolumeObject>
#include <kvs/PolygonObject>
#include <kvs/Bounds>
#include <kvs/OrthoSlice>
#include <kvs/DivergingColorMap>
#include <kvs/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/ColorMapBar>

#include "OnlineCovarianceVolume.h"
#include "Animation.h"
#include "Length.h"
#include "WeatherData.h"
#include "Tools.h"

namespace Program2
{

    class Program : public kvs::Program
    {
        int exec(int argc, char **argv);
    };
} // namespace Program2