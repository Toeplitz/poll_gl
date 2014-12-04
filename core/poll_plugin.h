#pragma once

//#include <SDL2/SDL_events.h>            // for SDL_MouseButtonEvent, etc
//#include <SDL2/SDL_keyboard.h>          // for SDL_Keysym
#include <vector>
#include <QMouseEvent>
#include <QKeyEvent>
#include  <QWidget>

class Node;


class Poll_Plugin
{
  public:
    Poll_Plugin();
    ~Poll_Plugin();

    /* This is called every loop, straight after the SDL event polling */
    virtual void cb_custom(const float dt);

    /* This is called after the lighting pass, but before the final screen quad */
    virtual void cb_custom_draw();

    /* This is called after a light is created */
    virtual void cb_light_create(Node *node);

    /* This is called after a node is created */
    virtual void cb_node_create(Node *node);

    virtual void cb_mouse_pressed(QMouseEvent *ev);
    virtual void cb_mouse_released(QMouseEvent *ev);
    virtual void cb_mouse_motion(QWidget *w, QMouseEvent *ev, const double dt);
    virtual void cb_keyboard_pressed(QWidget *w, QKeyEvent *ev);
    virtual void cb_keyboard_released(QWidget *w, QKeyEvent *ev);

};

typedef std::vector<Poll_Plugin *> Poll_Plugin_List;
