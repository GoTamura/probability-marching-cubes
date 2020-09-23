#include <kvs/glut/TransferFunctionEditor>
#include "Core/Utility/Stat.h"
#include <kvs/RendererManager>
#include "Core/Utility/Type.h"
#include "Core/Utility/Value.h"
#include "Core/Utility/ValueArray.h"
#include <kvs/Message>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/PolygonObject>
#include <kvs/Bounds>
#include <kvs/OrthoSlice>
#include <kvs/HydrogenVolumeData>
#include <kvs/ScreenCaptureEvent>
#include <kvs/Stat>
#include <kvs/ImageObject>
#include <kvs/ImageRenderer>
#include <kvs/Isosurface>
#include <kvs/Endian>
#include <kvs/ColorImage>
#include <kvs/DivergingColorMap>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
//#include <kvs/RayCastingRenderer>
#include "myRayCastingRendererGLSL.h"
#include <chrono>
#include <random>
#include <functional>
//#include <kvs/osmesa/Screen>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include "Core/Utility/ValueTable.h"
#include "Core/Visualization/Object/TableObject.h"
#include "Core/Visualization/Renderer/Axis2D.h"
#include "ProbabilisticMarchingCubes.h"
#include "OnlineCovarianceVolume.h"
#include <kvs/ValueTable>
#include <kvs/ScatterPlotRenderer>
#include <kvs/TableObject>
#include <kvs/Axis2D>
#include "Animation.h"
#include "Length.h"
#include <kvs/ColorMapBar>
#include "WeatherData.h"

#include <tuple>

kvs::TransferFunction tfunc("./tfunc_20191223_121206.kvsml");

class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{
private:
  int renderer_id;
public:

    TransferFunctionEditor( kvs::glut::Screen* screen, int id ):
        kvs::glut::TransferFunctionEditor( screen ), renderer_id(id){}

    void apply( void )
    {
        const kvs::RendererBase* base = static_cast<kvs::glut::Screen*>(screen())->scene()->rendererManager()->renderer(renderer_id);
        kvs::glsl::myRayCastingRenderer* renderer = (kvs::glsl::myRayCastingRenderer*)base;
        tfunc = transferFunction();
        renderer->setTransferFunction( transferFunction() );
        screen()->redraw();
    }
};

static const int NX = 301;
static const int NY = 301;
static const int NZ = 50;
static const int SIZE = NX*NY*NZ;

//std::vector<kvs::ObjectBase*> loadObjects(int nloops) {


kvs::ValueArray<float> loadQV(std::string file) {
    std::cout << "test"<< std::endl;
  kvs::StructuredVolumeObject* vol = WeatherData::loadWeatherData(file, WeatherData::Parameter::QV);
  auto array = vol->values().asValueArray<float>();
  delete vol;
  return array;

}

std::vector<std::string> sameTime(int nloops, const std::string ensemble_path) {
  std::vector<std::string> files;
  for (int i = 1; i <= 20; ++i) {
    std::stringstream ss;
    ss << ensemble_path << "/" << std::setw(3) << std::setfill('0') << i << "/gs" << std::setw(4) << std::setfill('0') << nloops + 59 << ".bin";
    std::cout << ss.str() << std::endl;
    files.push_back(ss.str());
  }
  return files;
}

void calc_pmc(const int nloops, kvs::ValueArray<float> &prob,kvs::ValueArray<float> &length_array,kvs::ValueArray<float> &point, const std::string ensemble_path) {
  auto files = sameTime(nloops, ensemble_path);
    std::cout << "test"<< std::endl;
  auto ocmvCalc = OnlineCovMatrixVolumeCalcurator(NX - 1, NY - 1, NZ - 1, files, loadQV);

  const auto average_array = ocmvCalc.ocmv->average();
  const auto ave_matrix = ocmvCalc.ocmv->average_matrix();
  const auto cov_matrix = ocmvCalc.ocmv->cholesky_covariance_volume();

  const float threshold = 0.01;
  const int samples = 100;
  prob = ProbabilisticMarchingCubes::calc_pdf(cov_matrix, ave_matrix, threshold, samples);
  length_array = calc_length_volume(average_array, threshold, NX-1, NY-1, NZ-1);
  point = calc_point(length_array, prob, NX-1, NY-1, NZ-1);
}

void write_kvsml(std::string file, const kvs::ValueArray<float> &array, kvs::StructuredVolumeObject *vol) {
  vol->setGridTypeToUniform();
  vol->setVeclen(1);
  vol->setResolution(kvs::Vector3ui(NX - 1, NY - 1, NZ - 1));
  vol->setValues(array);
  vol->updateMinMaxValues();
  vol->write(file, false, true);
}

void read_kvsml(std::string file, kvs::StructuredVolumeObject *vol) {
  vol->read(file);
  vol->setMinMaxExternalCoords(vol->minObjectCoord(), vol->maxObjectCoord() * kvs::Vec3(1, 1, 5));
}

void setVisibleArea(kvs::PolygonObject *object) {
  kvs::ValueArray<kvs::UInt8> opacities(object->numberOfVertices());
  for (int i = 0; i < object->numberOfVertices(); ++i) {
    opacities[i] = 255;
    auto x = object->coords()[3*i];
    auto y = object->coords()[3*i+1];
    auto z = object->coords()[3*i+2];
    if (!(90 <= x && x <= 210 && 90 <= y && y <= 210)) {
      opacities[i] = 0;
    }
  }
  object->setOpacities(opacities);
}
    

//std::vector<kvs::ObjectBase*> loadObjects(int nloops) {
std::function<std::vector<kvs::ObjectBase*>(int)> makeLoadFunction(std::string prob_path, std::string true_path, std::string ensemble_path) {
    return [=](int nloops) {
    // Calcurate pmc
    kvs::ValueArray<float> prob;
    kvs::ValueArray<float> length_array;
    kvs::ValueArray<float> point;
    calc_pmc(nloops, prob, length_array, point, ensemble_path);

    const float threshold = 0.01;

    kvs::StructuredVolumeObject *probability_vol = new kvs::StructuredVolumeObject();

    // make file name
    std::stringstream ss;
    ss << prob_path << "/" << std::setw(4) << std::setfill('0') << nloops + 59 << ".kvsml";
    std::cout << ss.str() << std::endl;
    write_kvsml(ss.str(), prob, probability_vol);
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
    kvs::StructuredVolumeObject* true_vol = WeatherData::loadWeatherData(ss2.str(), WeatherData::Parameter::QV);
    true_vol->setMinMaxExternalCoords(probability_vol->minObjectCoord(), probability_vol->maxObjectCoord()*kvs::Vec3(1, 1, 5));

    //return 0;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
    kvs::PolygonObject* object = new kvs::Isosurface( true_vol, threshold, n, d, t );
    //object->setColor(kvs::RGBColor(220, 220, 220));
    //object->setColors(point_to_color(point, object->coords(), t, NX-1, NY-1, NZ-1));
    //object->setColors(point_to_color(probability_vol->values().asValueArray<float>(), object->coords(), t, NX-1, NY-1, NZ-1));
    object->setColorTypeToVertex();

    std::vector<kvs::ObjectBase*> objects;
    auto for_Bounds = new kvs::PolygonObject();
    for_Bounds->setMinMaxExternalCoords(probability_vol->minExternalCoord(), probability_vol->maxExternalCoord());
    objects.push_back(for_Bounds);
    //objects.push_back(object);
    objects.push_back(probability_vol);
    return objects;
    };
}

kvs::ValueArray<kvs::Real32> makeAlpha() {
  auto volume = kvs::ValueArray<kvs::Real32>(301 * 301 * 50);
  for (int i = 0; i < 50; ++i) {
    for (int j = 0; j < 301; ++j) {
      for (int k = 0; k < 301; ++k) {
        volume[i*301*301 + j*301 + k] = 0.0;
        //if (90 <= j && j <= 210 && 90 <= k && k <= 210)
          volume[i*301*301 + j*301 + k] = 1.0;
      }
    }
  }
  return volume;
}

std::vector<kvs::RendererBase*> loadRenderer() {
    kvs::glsl::myRayCastingRenderer* ren = new kvs::glsl::myRayCastingRenderer();
    //kvs::RayCastingRenderer* ren = new kvs::RayCastingRenderer();
    //ren->enableLODControl();
    auto alpha = makeAlpha();
    ren->setAlphaTexture(NX, NY, NZ, &alpha);

    ren->enableShading();
    //ren->disableShading();
    ren->setSamplingStep(0.3f);
    ren->setOpaqueValue(0.97f);
    ren->setTransferFunction(tfunc);

    std::vector<kvs::RendererBase*> renderers;
    renderers.push_back(new kvs::Bounds());
    //renderers.push_back(nullptr);
    renderers.push_back(ren);

    return renderers;
}
int main( int argc, char** argv ) {
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setGeometry( 0, 0, 1024, 1024 );
    screen.setTitle( "ProabilityMarchingCubes" );
    screen.setBackgroundColor(kvs::RGBColor::White());

    // Set camera angle
    screen.scene()->camera()->setPosition(kvs::Vec3(8,-10,4), kvs::Vec3(0,0,0), kvs::Vec3(0, 0, 1));

    screen.create();
    screen.show();

    // Set ColorMap Bar
    kvs::ColorMapBar color_map_bar(&screen);
    color_map_bar.setColorMap(tfunc.colorMap());
    color_map_bar.setRange(0, 1.0);
    color_map_bar.show();

    //kvs::osmesa::Screen screen;
    //screen.draw();
    //screen.capture().write(argv[2]);

    auto loadObjects = makeLoadFunction(argv[1], argv[2], argv[3]);
    auto anim = local::Animation(&screen, loadObjects, loadRenderer);

    screen.paintEvent();
    screen.paintEvent();
    screen.scene()->camera()->snapshot().write("img1.bmp");
 
    // Set keyboard contoroller
    const int msec = 200;
    kvs::glut::Timer timer(msec);
    timer.stop();
    local::KeyPressEvent key_press_event(&timer, &anim);
    local::TimerEvent timer_event(&anim);
    screen.addEvent(&key_press_event);
//    screen.addTimerEvent(&timer_event, &timer);

    // Set Transfer Function Editior
    TransferFunctionEditor editor( &screen, anim.active_RendererIDs[1] );
    editor.setTransferFunction(tfunc);
    editor.show();

    return( app.run() );
}
