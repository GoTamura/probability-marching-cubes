#include "Program2.h"

namespace Program2
{

  std::function<std::vector<kvs::ObjectBase *>(int)> makeLoadFunction(std::string prob_path, std::string true_path, std::string ensemble_path)
  {
    return [=](int nloops) {
      auto files = Tools::sameTime(nloops, ensemble_path);
      auto ocmvCalc = OnlineCovMatrixVolumeCalcurator(300, 300, 49, files, Tools::loadQV);

      //const auto average_array = ocmvCalc.average();
      const auto variance_array = ocmvCalc.variance();
      auto var_vol = new kvs::StructuredVolumeObject();
      var_vol->setGridTypeToUniform();
      var_vol->setVeclen(1);
      // ocmvを使って分散を計算したので各辺-1される
      var_vol->setResolution(kvs::Vector3ui(Tools::NX - 1, Tools::NY - 1, Tools::NZ - 1));
      var_vol->setValues(variance_array);
      var_vol->updateMinMaxValues();

      const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
      const bool d = false;
      const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
      const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::ZAxis;
      kvs::PolygonObject *object = new kvs::OrthoSlice(var_vol, 14, a, t);

      std::vector<kvs::ObjectBase *> objects;
      auto for_Bounds = new kvs::PolygonObject();
      for_Bounds->setMinMaxExternalCoords(var_vol->minExternalCoord(), var_vol->maxExternalCoord());
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

    screen.create();

    // Set ColorMap Bar
    kvs::ColorMapBar color_map_bar(&screen);
    color_map_bar.setColorMap(kvs::DivergingColorMap::CoolWarm(256));
    //color_map_bar.setColorMap(tfunc.colorMap());
    color_map_bar.setRange(0, 1.0);
    color_map_bar.show();

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
} // namespace Program2