#include "Program2.h"

namespace Program2
{

  kvs::TransferFunction tfunc("./tfunc_20191223_121206.kvsml");

  class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
  {
  private:
    int renderer_id;

  public:
    TransferFunctionEditor(kvs::glut::Screen *screen, int id) : kvs::glut::TransferFunctionEditor(screen), renderer_id(id) {}

    void apply(void)
    {
      const kvs::RendererBase *base = static_cast<kvs::glut::Screen *>(screen())->scene()->rendererManager()->renderer(renderer_id);
      kvs::glsl::myRayCastingRenderer *renderer = (kvs::glsl::myRayCastingRenderer *)base;
      tfunc = transferFunction();
      renderer->setTransferFunction(transferFunction());
      screen()->redraw();
    }
  };

  //std::vector<kvs::ObjectBase*> loadObjects(int nloops) {
  std::function<std::vector<kvs::ObjectBase *>(int)> makeLoadIso(std::string true_path, std::string ensemble_path)
  {
    return [=](int nloops) {
      std::vector<kvs::ObjectBase *> objects;
      auto for_Bounds = new kvs::PolygonObject();

      auto files = sameTime(nloops, ensemble_path);
      //for (const auto& file: files) {
      auto vol = kvs::StructuredVolumeObject();
      WeatherData::loadWeatherData(files[0], WeatherData::Parameter::QV, vol);
      vol.setGridTypeToUniform();
      vol.setVeclen(1);
      vol.setResolution(kvs::Vector3ui(NX, NY, NZ));
      vol.updateMinMaxValues();
      vol.setMinMaxExternalCoords(vol.minObjectCoord(), vol.maxObjectCoord() * kvs::Vec3(1, 1, 5));

      const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
      const bool d = false;
      const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
      float threshold = 0.1;
      kvs::PolygonObject *object = new kvs::Isosurface(&vol, threshold, n, d, t);
      if (!object)
      {
        std::cout << "error" << std::endl;
      }
      object->setColorTypeToVertex();
      for_Bounds->setMinMaxExternalCoords(vol.minExternalCoord(), vol.maxExternalCoord());
      objects.push_back(for_Bounds);
      objects.push_back(object);
      std::cout << "test" << std::endl;
      //}
      return objects;
    };
  }

  //std::vector<kvs::ObjectBase*> loadObjects(int nloops) {
  std::function<std::vector<kvs::ObjectBase *>(int)> makeLoadFunction(std::string prob_path, std::string true_path, std::string ensemble_path)
  {
    return [=](int nloops) {
      // Calcurate pmc
      //kvs::ValueArray<float> prob;
      //kvs::ValueArray<float> length_array;
      //kvs::ValueArray<float> point;
      //calc_pmc(nloops, prob, length_array, point, ensemble_path);

      auto files = sameTime(nloops, ensemble_path);
      auto ocmvCalc = OnlineCovMatrixVolumeCalcurator(NX - 1, NY - 1, NZ - 1, files, loadQV);

      const auto average_array = ocmvCalc.average();
      const auto variance_array = ocmvCalc.variance();
      auto var_vol = new kvs::StructuredVolumeObject();
      var_vol->setGridTypeToUniform();
      var_vol->setVeclen(1);
      var_vol->setResolution(kvs::Vector3ui(NX - 1, NY - 1, NZ - 1));
      var_vol->setValues(variance_array);
      var_vol->updateMinMaxValues();

      kvs::StructuredVolumeObject *probability_vol = new kvs::StructuredVolumeObject();

      const float threshold = 0.01;

      // make file name
      std::stringstream ss;
      ss << prob_path << "/" << std::setw(4) << std::setfill('0') << nloops + 59 << ".kvsml";
      std::cout << ss.str() << std::endl;
      //write_kvsml(ss.str(), prob, probability_vol);
      read_kvsml(ss.str(), probability_vol);

      // Calcurate point
      //kvs::ValueArray<float> prob = probability_vol->values().asValueArray<float>();
      //kvs::ValueArray<float> point = calc_point(length_array, prob, NX-1, NY-1, NZ-1);
      kvs::StructuredVolumeObject *point_vol = new kvs::StructuredVolumeObject();
      // make file name
      std::stringstream ss1;
      ss1 << "./point/" << std::setw(4) << std::setfill('0') << nloops + 59 << ".kvsml";
      std::cout << ss1.str() << std::endl;
      //write_kvsml(ss1.str(), point, point_vol);
      //point_vol->read("./point/0060.kvsml");//ss1.str());
      //
      //kvs::ValueArray<float> point = point_vol->values().asValueArray<float>();

      //kvs::StructuredVolumeObject *average_vol = new kvs::StructuredVolumeObject();
      //write_kvsml(argv[3], average_array, average_vol);
      //read_kvsml(argv[2], average_vol);

      std::stringstream ss2;
      //ss2 << "/Users/go/Documents/prob/true/true2008" << std::setw(4) << std::setfill('0') << nloops + 299 << ".bin";
      ss2 << true_path << "/true2008" << std::setw(4) << std::setfill('0') << nloops + 299 << ".bin";
      std::cout << ss2.str() << std::endl;
      //kvs::StructuredVolumeObject *true_vol = WeatherData::loadWeatherData(ss2.str(), WeatherData::Parameter::QV);
      auto true_vol = kvs::StructuredVolumeObject();
      WeatherData::loadWeatherData(ss2.str(), WeatherData::Parameter::QV, true_vol);
      true_vol.setMinMaxExternalCoords(probability_vol->minObjectCoord(), probability_vol->maxObjectCoord() * kvs::Vec3(1, 1, 5));

      //auto files = sameTime(nloops, ensemble_path);
      //auto rmseCalc = RMSECalcurator(NX, NY, NZ, true_vol->values().asValueArray<float>(), files, loadQV);
      //auto rmse_vol = new kvs::StructuredVolumeObject();
      //rmse_vol->setGridTypeToUniform();
      //rmse_vol->setVeclen(1);
      //rmse_vol->setResolution(kvs::Vector3ui(NX, NY, NZ));
      //rmse_vol->setValues(rmseCalc.rmse->get());
      //rmse_vol->updateMinMaxValues();

      //return 0;
      const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
      const bool d = false;
      const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
      //kvs::PolygonObject *object = new kvs::Isosurface(true_vol, threshold, n, d, t);
      //kvs::PolygonObject *object = new kvs::Isosurface(rmse_vol, (rmse_vol->minValue() + rmse_vol->maxValue()) / 2., n, d, t);
      //object->setColor(kvs::RGBColor(220, 220, 220));
      ////object->setColors(point_to_color(point, object->coords(), t, NX-1, NY-1, NZ-1));
      ////object->setColors(point_to_color(probability_vol->values().asValueArray<float>(), object->coords(), t, NX-1, NY-1, NZ-1));
      //object->setColorTypeToVertex();

      //const float p = rmse_vol->resolution().z() * 0.5f;
      const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::ZAxis;
      //const kvs::TransferFunction t(256);
      kvs::PolygonObject *object = new kvs::OrthoSlice(var_vol, 14, a, t);

      std::vector<kvs::ObjectBase *> objects;
      auto for_Bounds = new kvs::PolygonObject();
      for_Bounds->setMinMaxExternalCoords(probability_vol->minExternalCoord(), probability_vol->maxExternalCoord());
      objects.push_back(for_Bounds);
      objects.push_back(object);
      //objects.push_back(probability_vol);
      return objects;
    };
  }
  std::vector<kvs::RendererBase *> loadRenIso()
  {
    std::vector<kvs::RendererBase *> renderers;
    renderers.push_back(new kvs::Bounds());
    renderers.push_back(nullptr);
    //for (int i = 0; i < 20; i++) {
    //  renderers.push_back(nullptr);
    //}
    return renderers;
  }

  std::vector<kvs::RendererBase *> loadRenderer()
  {
    kvs::glsl::myRayCastingRenderer *ren = new kvs::glsl::myRayCastingRenderer();
    //kvs::glsl::RayCastingRenderer* ren = new kvs::glsl::RayCastingRenderer();
    //ren->enableLODControl();
    auto alpha = makeAlpha();
    ren->setAlphaTexture(NX, NY, NZ, &alpha);

    ren->enableShading();
    //ren->disableShading();
    ren->setSamplingStep(0.3f);
    ren->setOpaqueValue(0.97f);
    ren->setTransferFunction(tfunc);

    auto poly_ren = new kvs::PolygonRenderer();
    poly_ren->disableShading();

    std::vector<kvs::RendererBase *> renderers;
    renderers.push_back(new kvs::Bounds());
    renderers.push_back(poly_ren);
    //renderers.push_back(ren);

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
    //screen.show();

    // Set ColorMap Bar
    kvs::ColorMapBar color_map_bar(&screen);
    color_map_bar.setColorMap(kvs::DivergingColorMap::CoolWarm(256));
    //color_map_bar.setColorMap(tfunc.colorMap());
    color_map_bar.setRange(0, 1.0);
    color_map_bar.show();

    //kvs::osmesa::Screen screen;
    //screen.draw();
    //screen.capture().write(argv[2]);

    auto loadObjects = makeLoadFunction(argv[1], argv[2], argv[3]);
    //auto loadObjects = makeLoadIso(argv[2], argv[3]);
    auto anim = local::Animation(&screen, loadObjects, loadRenderer);
    //auto anim = local::Animation(&screen, loadObjects, loadRenIso);

    //screen.paintEvent();
    //screen.paintEvent();
    //screen.scene()->camera()->snapshot().write("img1.bmp");

    // Set keyboard contoroller
    const int msec = 200;
    kvs::glut::Timer timer(msec);
    timer.stop();
    local::KeyPressEvent key_press_event(&timer, &anim);
    local::TimerEvent timer_event(&anim);
    screen.addEvent(&key_press_event);
    //    screen.addTimerEvent(&timer_event, &timer);

    // Set Transfer Function Editior
    //TransferFunctionEditor editor(&screen, anim.active_RendererIDs[1]);
    //editor.setTransferFunction(tfunc);
    //editor.show();

    return (app.run());
  }
} // namespace Program2