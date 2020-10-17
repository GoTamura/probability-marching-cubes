#pragma once
#include <vector>
#include <kvs/glut/Timer>
#include <kvs/TimerEventListener>
#include <kvs/KeyPressEventListener>
#include <future>

namespace local {
class Animation {
public:
  kvs::glut::Screen *screen;
private:
  int nloop = 1;
  int loop_size = 60;
  //std::vector<kvs::ObjectBase*> (*loadFunc)(int);
  std::function<std::vector<kvs::ObjectBase*>(int)> loadFunc;

  //std::vector<kvs::RendererBase*> (*loadRen)();
  std::function<std::vector<kvs::RendererBase*>()> loadRen;

  //std::future<std::vector<kvs::ObjectBase*>> next_object;
  //std::future<std::vector<kvs::ObjectBase*>> prev_object;

public:
  std::vector<int> active_ObjectIDs;
  std::vector<int> active_RendererIDs;

  Animation(kvs::glut::Screen *sc, std::function<std::vector<kvs::ObjectBase*>(int)> func, std::function<std::vector<kvs::RendererBase*>()> ren): screen(sc), loadFunc(func), loadRen(ren) {
    std::vector<kvs::ObjectBase*> objects = loadFunc(nloop);
    std::vector<kvs::RendererBase*> renderers = loadRen();
    for (int i = 0; i < objects.size(); ++i) {
      if (renderers[i]) {
        auto ids = screen->registerObject(objects[i], renderers[i]);
        active_ObjectIDs.push_back(ids.first);
        active_RendererIDs.push_back(ids.second);
      }
      else {
        auto ids = screen->registerObject(objects[i]);
        active_ObjectIDs.push_back(ids.first);
        active_RendererIDs.push_back(ids.second);
      }
    }
    //next_object = std::async(std::launch::async, [this] { return loadFunc(next_loop(nloop));});
    //prev_object = std::async(std::launch::async, [this] { return loadFunc(prev_loop(nloop));});
  }

  int prev_loop(int n) {
    if(n == 1) n = loop_size;
    else n--;
    return n;
  }
  int next_loop(int n) {
    if(n == loop_size) n = 1;
    else n++;
    return n;
  }

  void prev() {
    nloop = prev_loop(nloop);
    std::cout << "J:prev " << nloop << std::endl;
    //switchObjects(prev_object.get());
    switchObjects(loadFunc(nloop));
  }

  void next() {
    nloop = next_loop(nloop);
    std::cout << "K:next " << nloop << std::endl;
    //switchObjects(next_object.get());
    switchObjects(loadFunc(nloop));
  }


  void switchObjects(std::vector<kvs::ObjectBase*> objects) {
    std::vector<kvs::RendererBase*> renderers = loadRen();
    for (int i = 0; i < objects.size(); ++i) {
      screen->scene()->replaceObject(active_ObjectIDs[i], objects[i], true);
      if (renderers[i])
        screen->scene()->replaceRenderer(active_RendererIDs[i], renderers[i], true);
    }

    //next_object = std::async(std::launch::async, [this] { return loadFunc(next_loop(nloop));});
    //prev_object = std::async(std::launch::async, [this] { return loadFunc(prev_loop(nloop));});

    screen->redraw();
  }
};


class KeyPressEvent : public kvs::KeyPressEventListener
{
kvs::glut::Timer* m_timer;
Animation *_a;

public:
KeyPressEvent(kvs::glut::Timer* timer, Animation *a): m_timer(timer), _a(a){
}

void update (kvs::KeyEvent* event)
{
  switch(event->key())
  {
    case kvs::Key::s:
      if(m_timer->isStopped()){
        m_timer->start();
        std::cout << "S: Start" << std::endl;
      }
      else{
        m_timer->stop();
        std::cout << "S: Stop" << std::endl;
      }
      break;
    case kvs::Key::j:
      _a->prev();
      break;
    case kvs::Key::k:
      _a->next();
      break;
    default:
      break;
  }
}
};

class TimerEvent : public kvs::TimerEventListener
{
private:
  Animation *_a;
  void update(kvs::TimeEvent* event)
  {
    //_a->next();
    //_a->screen->redraw();
  }
public:
  TimerEvent(Animation* a): _a(a) {
  }
};
}
