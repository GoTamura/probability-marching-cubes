#pragma once
#include "Length.h"
#include "WeatherData.h"
#include "Tools.h"
#include <kvs/PolygonObject>
#include <kvs/Isosurface>
#include <kvs/TransferFunction>

#include <iostream>
#include "Program.h"

namespace Program8
{

    class Program : public kvs::Program
    {
        int exec(int argc, char **argv);
    };
} // namespace Program3