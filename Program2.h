#pragma once
#include <kvs/Program>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <chrono>
#include <random>
#include <functional>

#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RendererManager>
#include <kvs/StructuredVolumeObject>
#include <kvs/PolygonObject>
#include <kvs/Bounds>
#include <kvs/OrthoSlice>
#include <kvs/ScreenCaptureEvent>
#include <kvs/Isosurface>
#include <kvs/ColorImage>
#include <kvs/DivergingColorMap>
#include <kvs/RayCastingRenderer>
#include <kvs/PolygonRenderer>
#include "myRayCastingRendererGLSL.h"
//#include <kvs/osmesa/Screen>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/ColorMapBar>

#include "ProbabilisticMarchingCubes.h"
#include "OnlineCovarianceVolume.h"
#include "Animation.h"
#include "Length.h"
#include "WeatherData.h"
#include "Rmse.h"
#include "Tools.h"

namespace Program2
{

    class Program : public kvs::Program
    {
        int exec(int argc, char **argv);
    };
} // namespace Program2