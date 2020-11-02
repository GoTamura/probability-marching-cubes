#include "Program.h"

// z=2km平面での実測値との誤差2乗平均
namespace Program1
{
    std::function<std::vector<kvs::ObjectBase *>(int)> makeLoadFunction(std::string prob_path, std::string true_path, std::string ensemble_path)
    {
        return [=](int nloops) {
            auto true_vol = kvs::StructuredVolumeObject();
            WeatherData::loadWeatherData(Tools::truth_filename(true_path, nloops), WeatherData::Parameter::QV, true_vol);

            auto files = Tools::sameTime(nloops, ensemble_path);
            auto rmseCalc = RMSECalcurator(Tools::NX, Tools::NY, Tools::NZ, true_vol.values().asValueArray<float>(), files, Tools::loadQV);
            auto rmse_vol = new kvs::StructuredVolumeObject();
            rmse_vol->setGridTypeToUniform();
            rmse_vol->setVeclen(1);
            rmse_vol->setResolution(kvs::Vector3ui(Tools::NX, Tools::NY, Tools::NZ));
            rmse_vol->setValues(rmseCalc.get());
            rmse_vol->updateMinMaxValues();

            //return 0;
            const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
            const bool d = false;
            const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
            const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::ZAxis;
            // z=2km == 14
            kvs::PolygonObject *object = new kvs::OrthoSlice(rmse_vol, 14, a, t);

            std::vector<kvs::ObjectBase *> objects;
            auto for_Bounds = new kvs::PolygonObject();
            for_Bounds->setMinMaxExternalCoords(rmse_vol->minExternalCoord(), rmse_vol->maxExternalCoord());
            objects.push_back(for_Bounds);
            objects.push_back(object);
            return objects;
        };
    }

    std::vector<kvs::RendererBase *> loadRenderer()
    {
        auto poly_ren = new kvs::PolygonRenderer();
        poly_ren->disableShading();

        std::vector<kvs::RendererBase *> renderers;
        renderers.push_back(new kvs::Bounds());
        renderers.push_back(poly_ren);

        return renderers;
    }

    int Program::exec(int argc, char **argv)
    {
        kvs::glut::Application app(argc, argv);
        kvs::glut::Screen screen(&app);
        screen.setGeometry(0, 0, 1024, 1024);
        screen.setTitle("ProabilityMarchingCubes");
        screen.setBackgroundColor(kvs::RGBColor::White());

        // Set camera angle
        //screen.scene()->camera()->setPosition(kvs::Vec3(8, -10, 4), kvs::Vec3(0, 0, 0), kvs::Vec3(0, 0, 1));

        screen.create();

        // Set ColorMap Bar
        {
            kvs::ColorMapBar color_map_bar(&screen);
            color_map_bar.setColorMap(kvs::DivergingColorMap::CoolWarm(256));
            color_map_bar.setRange(0, 1.0);
            color_map_bar.show();
        }

        auto loadObjects = makeLoadFunction(argv[1], argv[2], argv[3]);
        auto anim = local::Animation(&screen, loadObjects, loadRenderer);

        // Set keyboard contoroller
        const int msec = 200;
        kvs::glut::Timer timer(msec);
        timer.stop();
        local::KeyPressEvent key_press_event(&timer, &anim);
        local::TimerEvent timer_event(&anim);
        screen.addEvent(&key_press_event);

        return (app.run());
    }
} // namespace Program1