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
#include <kvs/RayCastingRenderer>
#include <chrono>
#include <random>
#include <functional>
//#include <kvs/osmesa/Screen>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <omp.h>
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
        kvs::glsl::RayCastingRenderer* renderer = (kvs::glsl::RayCastingRenderer*)base;
        tfunc = transferFunction();
        renderer->setTransferFunction( transferFunction() );
        screen()->redraw();
    }
};

static const int NX = 301;
static const int NY = 301;
static const int NZ = 50;
static const int SIZE = NX*NY*NZ;

kvs::ValueArray<float> createCoords(int nx, int ny, int nz);

//static const kvs::ValueArray<float> coords = createCoords(NX, NY, NZ);

enum Parameter {
  U,   // 東西風 : X-wind component (m s-1)
  V,   // 南北風 : Y-wind component (m s-1)
  W,   // 鉛直風 : Z-wind component (m s-1)
  T,   // 気温   : Temperature (K)
  P,   // 気圧   : Pressure (Pa)
  QV,  // 水蒸気混合比 : Water vapor mixing ratio (kg kg-1) 
  QC,  // 雲水混合比   : Cloud water mixing ratio (kg kg-1)
  QR,  // 雨混合比     : Rain mixing ratio (kg kg-1)
  QCI, // 雲氷混合比   : Cloud ice mixing ratio (kg kg-1)
  QS,  // 雪混合比     : Snow mixing ratio (kg kg-1)
  QG   // あられ混合比 : Graupel mixing ratio (kg kg-1)
};

kvs::ValueArray<float> createCoords(int nx, int ny, int nz) {
  // x方向は1.09545294622*10^-3°間隔
  std::vector<float> x(nx);
  float radius_earth = 6360000.0; // [m]
  // 基準地点=(東経133.590905 , 北緯33.51538902)
  float latitude = 33.5153;
  float longitude_interval = 0.00109545294622;
  float radius_point = radius_earth * cos(latitude/360.0);
  float circumference_point = radius_point * 2.0 * 3.141592;

  for (int i = 0; i < nx; ++i) {
    x[i] = (i-nx/2.) * circumference_point * longitude_interval / 360.0;
  }
  // y方向は8.99279260651*10^-4°間隔
  float latitude_interval = 0.000899279260;
  std::vector<float> y(ny);
  for (int i = 0; i < ny; ++i) {
    y[i] = (i-ny/2.) * radius_earth * 2.0 * 3.141592 * latitude_interval / 360.0;
  }

  // z方向 : (単位はメートル[m])
  //   1   -20.0000  2    20.0000  3    60.0000  4   118.0000  5   194.0000
  //   6   288.0000  7   400.0000  8   530.0000  9   678.0000 10   844.0000
  //  11  1028.0000 12  1230.0000 13  1450.0000 14  1688.0000 15  1944.0000
  //  16  2218.0000 17  2510.0000 18  2820.0000 19  3148.0000 20  3494.0000
  //  21  3858.0000 22  4240.0000 23  4640.0000 24  5058.0000 25  5494.0000
  //  26  5948.0000 27  6420.0000 28  6910.0000 29  7418.0000 30  7944.0000
  //  31  8488.0000 32  9050.0000 33  9630.0000 34 10228.0000 35 10844.0000
  //  36 11478.0000 37 12130.0000 38 12800.0000 39 13488.0000 40 14194.0000
  //  41 14918.0000 42 15660.0000 43 16420.0000 44 17198.0000 45 17994.0000
  //  46 18808.0000 47 19640.0000 48 20490.0000 49 21358.0000 50 22244.0000
  std::vector<float> z = {
        -20.0000  ,    20.0000  ,    60.0000  ,   118.0000  ,   194.0000
     ,   288.0000  ,   400.0000  ,   530.0000  ,   678.0000 ,   844.0000
    ,  1028.0000 , 1230.0000 ,  1450.0000 ,  1688.0000 ,  1944.0000
    ,  2218.0000 , 2510.0000 ,  2820.0000 ,  3148.0000 ,  3494.0000
    ,  3858.0000 , 4240.0000 ,  4640.0000 ,  5058.0000 ,  5494.0000
    ,  5948.0000 , 6420.0000 ,  6910.0000 ,  7418.0000 ,  7944.0000
    ,  8488.0000 , 9050.0000 ,  9630.0000 , 10228.0000 , 10844.0000
    , 11478.0000 ,12130.0000 , 12800.0000 , 13488.0000 , 14194.0000
    , 14918.0000 ,15660.0000 , 16420.0000 , 17198.0000 , 17994.0000
    , 18808.0000 ,19640.0000 , 20490.0000 , 21358.0000 , 22244.0000
  };

  // normalize
  // 倍率は適当
  auto norm = std::max(x[nx-1], std::max(y[ny-1], z[nz-1]));
  for (int i = 0; i < nx; ++i) {
    x[i] /= norm/301.0;
  }
  for (int i = 0; i < ny; ++i) {
    y[i] /= norm/301.0;
  }
  for (int i = 0; i < nz; ++i) {
    z[i] /= norm/301.0;
  }
  
  x.insert(x.end(),y.begin(), y.end());
  x.insert(x.end(),z.begin(), z.end());
  return kvs::ValueArray<float>(x);
}

kvs::ValueArray<float> loadValueArray(std::ifstream &ifs, int size) {
  kvs::ValueArray<float> array(size);
  ifs.read((char*)array.data(), size*4);
  
  // convert endian
  if (kvs::Endian::IsLittle()) {
    for (auto&& i: array) {
      kvs::Endian::Swap(&i);
    }
  }
  return array;
}

kvs::StructuredVolumeObject *load(std::ifstream &ifs, kvs::ValueArray<float> array) {
  kvs::ValueArray<float> kvs_value = loadValueArray(ifs, SIZE);
  
  kvs::StructuredVolumeObject *vol = new kvs::StructuredVolumeObject();
  vol->setGridTypeToUniform();
  vol->setVeclen(1);
  vol->setResolution(kvs::Vector3ui(NX, NY, NZ));
  vol->setValues(array);
  vol->updateMinMaxValues();

  //vol->setGridTypeToRectilinear();
  //vol->setCoords(coords.clone());
  //vol->updateMinMaxCoords();
  return vol;
}

kvs::StructuredVolumeObject *loadData(std::string filename, Parameter p) {
  std::ifstream ifs (filename, std::ios::in | std::ios::binary);
  if (ifs.fail()) {
    std::cerr << "file not found" << std::endl;
    return nullptr;
  }

  ifs.seekg(SIZE*4*p, std::ios_base::beg);
  kvs::StructuredVolumeObject* vol = load(ifs, loadValueArray(ifs, SIZE));
   
  ifs.close();
  return vol;
}

//std::vector<kvs::ObjectBase*> loadObjects(int nloops) {
std::function<std::vector<kvs::ObjectBase*>(int)> makeLoadFunction(std::string prob_path, std::string true_path) {
    return [=](int nloops) {
    //OnlineCovMatrixVolume ocmv(NX-1, NY-1, NZ-1);
    //for (int i = 1; i <= 20; ++i) {
    //  std::stringstream ss;
    //  ss << "/mnt/weather_ensemble/" << std::setw(3) << std::setfill('0') << i << "/gs" << argv[1] << ".bin";
 
    //  kvs::StructuredVolumeObject* vol = loadData(ss.str(), Parameter::QV);
    //  ocmv.addArray(vol->values().asValueArray<float>());
    //  delete vol;
    //}

    //const kvs::ValueArray<float> average_array = ocmv.average();
    //const std::vector<std::vector<float>> ave_matrix = ocmv.average_matrix();
    //const std::vector<std::vector<float>> cov_matrix = ocmv.cholesky_covariance_volume();
    const float threshold = 0.01;
    //const int samples = 100;
    //kvs::ValueArray<float> prob = ProbabilisticMarchingCubes::calc_pdf(cov_matrix, ave_matrix, threshold, samples);

    kvs::StructuredVolumeObject *probability_vol = new kvs::StructuredVolumeObject();
    //probability_vol->setGridTypeToUniform();
    //probability_vol->setVeclen(1);
    //probability_vol->setResolution(kvs::Vector3ui(NX-1, NY-1, NZ-1));
    //probability_vol->setValues(prob);
    //probability_vol->updateMinMaxValues();
    //probability_vol->write(argv[2], false, true);
    //probability_vol->read("../ensemble-visualization/prob/0100.kvsml");
    
    std::stringstream ss;
    //ss << "/Users/go/Documents/ensemble-visualization/prob/" << std::setw(4) << std::setfill('0') << nloops + 59 << ".kvsml";
    ss << prob_path << "/" << std::setw(4) << std::setfill('0') << nloops + 59 << ".kvsml";
    std::cout << ss.str() << std::endl;

    probability_vol->read(ss.str());
    probability_vol->setMinMaxExternalCoords(probability_vol->minObjectCoord(), probability_vol->maxObjectCoord()*kvs::Vec3(1, 1, 5));

    //kvs::StructuredVolumeObject *average_vol = new kvs::StructuredVolumeObject();
    ////average_vol->setGridTypeToUniform();
    ////average_vol->setVeclen(1);
    ////average_vol->setResolution(kvs::Vector3ui(NX-1, NY-1, NZ-1));
    ////average_vol->setValues(average_array);
    ////average_vol->updateMinMaxValues();
    ////average_vol->write(argv[3], false, true);
    ////average_vol->read("../ensemble-visualization/ave/0100.kvsml");
    //average_vol->read(argv[2]);
    //average_vol->setMinMaxExternalCoords(average_vol->minObjectCoord(), average_vol->maxObjectCoord()*kvs::Vec3(1, 1, 5));

    std::stringstream ss2;
    //ss2 << "/Users/go/Documents/prob/true/true2008" << std::setw(4) << std::setfill('0') << nloops + 299 << ".bin";
    ss2 << true_path << "/true2008" << std::setw(4) << std::setfill('0') << nloops + 299 << ".bin";
    kvs::StructuredVolumeObject* true_vol = loadData(ss2.str(), Parameter::QV);
    std::cout << ss2.str() << std::endl;
    true_vol->setMinMaxExternalCoords(probability_vol->minObjectCoord(), probability_vol->maxObjectCoord()*kvs::Vec3(1, 1, 5));

    //return 0;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    const kvs::TransferFunction t = kvs::DivergingColorMap::CoolWarm(256);
    kvs::PolygonObject* object = new kvs::Isosurface( true_vol, threshold, n, d, t );
    //if ( !object )
    //{
    //    kvsMessageError( "Cannot create a polygon object." );
    //    return( false );
    //}
    object->setColor(kvs::RGBColor(220, 220, 220));
    
    std::vector<kvs::ObjectBase*> objects;
    objects.push_back(object);
    objects.push_back(probability_vol);
    return objects;
    };
}

std::vector<kvs::RendererBase*> loadRenderer() {
    kvs::glsl::RayCastingRenderer* ren = new kvs::glsl::RayCastingRenderer();
    //kvs::RayCastingRenderer* ren = new kvs::RayCastingRenderer();
    //ren->enableLODControl();

    ren->enableShading();
    //ren->disableShading();
    ren->setSamplingStep(0.3f);
    ren->setOpaqueValue(0.97f);
    ren->setTransferFunction(tfunc);

    std::vector<kvs::RendererBase*> renderers;
    renderers.push_back(nullptr);
    renderers.push_back(ren);

    return renderers;
}

int main( int argc, char** argv ) {
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setGeometry( 0, 0, 1024, 1024 );
    screen.setTitle( "ProabilityMarchingCubes" );
    screen.create();
    screen.show();

    //kvs::osmesa::Screen screen;
    //screen.draw();
    //screen.capture().write(argv[2]);

    auto loadObjects = makeLoadFunction(argv[1], argv[2]);
    auto anim = local::Animation(&screen, loadObjects, loadRenderer);

    const int msec = 200;
    kvs::glut::Timer timer(msec);
    timer.stop();
    local::KeyPressEvent key_press_event(&timer, &anim);
    local::TimerEvent timer_event(&anim);
    screen.addEvent(&key_press_event);
    screen.addTimerEvent(&timer_event, &timer);

    TransferFunctionEditor editor( &screen, anim.active_ObjectIDs[1] );
    editor.setTransferFunction(tfunc);
    editor.show();

    return( app.run() );
}
