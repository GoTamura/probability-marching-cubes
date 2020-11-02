#pragma once

#include "Length.h"
#include "WeatherData.h"
#include "Tools.h"
#include "OnlineCovarianceVolume.h"
#include <iostream>
#include "Program.h"

namespace Program7
{

    class Program : public kvs::Program
    {
        int exec(int argc, char **argv);
    };
} // namespace Program7