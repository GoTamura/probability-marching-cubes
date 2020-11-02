#pragma once
#include "Length.h"
#include "WeatherData.h"
#include "Tools.h"
#include "Rmse.h"

#include <iostream>
#include "Program.h"

namespace Program4
{

    class Program : public kvs::Program
    {
        int exec(int argc, char **argv);
    };
} // namespace Program3