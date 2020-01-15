#pragma once
#include <vector>
#include <kvs/glut/Timer>
#include <kvs/TimerEventListener>
#include <kvs/KeyPressEventListener>

namespace local {
class Animation {
public:
  kvs::glut::Screen *screen;
private:
  int nloop = 1;
  int loop_size = 60;
  std::vector<kvs::ObjectBase*> (*loadFunc)(int);
  std::vector<kvs::RendererBase*> (*loadRen)();
public:
  std::vector<int> active_ObjectIDs;

  Animation(kvs::glut::Screen *sc, std::vector<kvs::ObjectBase*> (*func)(int), std::vector<kvs::RendererBase*> (*ren)()): screen(sc), loadFunc(func), loadRen(ren) {
    std::vector<kvs::ObjectBase*> objects = loadFunc(nloop);
    std::vector<kvs::RendererBase*> renderers = loadRen();
    for (int i = 0; i < objects.size(); ++i) {
      if (renderers[i]) {
        active_ObjectIDs.push_back(screen->registerObject(objects[i], renderers[i]).first);
      }
      else {
        active_ObjectIDs.push_back(screen->registerObject(objects[i]).first);
      }
    }
  }


  void prev() {
    if(nloop == 1) nloop = loop_size;
    else nloop--;
    std::cout << "J:prev " << nloop << std::endl;
    switchObjects();
  }

  void next() {
    if(nloop == loop_size) nloop = 1;
    else nloop++;
    std::cout << "K:next " << nloop << std::endl;
    switchObjects();
  }


  void switchObjects() {
    std::vector<kvs::ObjectBase*> objects = loadFunc(nloop);
    std::vector<kvs::RendererBase*> renderers = loadRen();
    for (int i = 0; i < objects.size(); ++i) {
      screen->scene()->replaceObject(active_ObjectIDs[i], objects[i], true);
      if (renderers[i])
        screen->scene()->replaceRenderer(active_ObjectIDs[i], renderers[i], true);
    }
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
    _a->next();
    //_a->screen->redraw();
  }
public:
  TimerEvent(Animation* a): _a(a) {
  }
};
}
