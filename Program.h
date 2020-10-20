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
#include <kvs/ScreenCaptureEvent>
#include <kvs/DivergingColorMap>
#include <kvs/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/ColorMapBar>

#include "Animation.h"
#include "WeatherData.h"
#include "Rmse.h"
#include "Tools.h"

namespace Program1
{
    class Program : public kvs::Program
    {
        int exec(int argc, char **argv);
    };
} // namespace Program1