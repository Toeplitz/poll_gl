#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "animated.h"
#include "gldefaults.h"
#include "uiwindow.h"

enum CameraMove { FORWARD, BACKWARD, SIDESTEP_RIGHT, SIDESTEP_LEFT};

class CameraPan:public Animated {
  public:
    bool toggle;
    glm::vec3 position;

    glm::vec3 getInterpolatedTranslation(double factor) 
    {
      glm::vec3 tInterp;
      int prevFrame = this->getPrevKeyFrame();
      int nextFrame = this->getNextKeyFrame();
      if (keyFrames[prevFrame]->t != keyFrames[nextFrame]->t) {
        tInterp =
          glm::mix(keyFrames[prevFrame]->t, keyFrames[nextFrame]->t,
              (float) factor);
      } else {
        tInterp = keyFrames[prevFrame]->t;
      }
      return tInterp;
    }

    bool stepTime(double dt) 
    {
      
      if (keyFrames.size() == 0)
        return false;
      
      animationTime = animationTime + dt;
      if (!stepAnimation(animationTime)) {
        return false;
      }

      double factor = getStepFactor(animationTime);
      position = getInterpolatedTranslation(factor);
      return true;
    }
};

class Camera {
  private:
    float fov;
    float initial_fov;
    float horizontalAngle;
    float verticalAngle;
    float speed;
    float mouseSpeed;
    glm::vec3 up;
    glm::vec3 direction;
    glm::vec3 right;
    std::vector < CameraMove > cameraMoveQueue;
    CameraPan cameraPan;

    void updateViewProjection(void);
    void updateProjection(void);
    bool processInputMoves(void);
    void processWayPoints(double dt);

  public: 

    Camera(int width, int height);
    ~Camera(void);
    glm::mat4 perspective;
    glm::mat4 ortho;
    glm::mat4 view;
    glm::vec3 position;
    glm::vec3 target;
    glm::mat4 modelView;
    glm::mat4 viewProjection;
    void addMove(CameraMove move);
    void deleteMove(CameraMove move);
    void update(double dt);
    void computeDirections();
    void updateView();
    bool mouseViewUpdate(const UIwindow & uiwindow, int xPos, int yPos);
    void setWayPoints(std::vector < glm::vec3 > points,
        std::vector < Uint32 > time, glm::vec3 _target,
        bool useCurPos);
    void setStartPosition(void);
    void createPerspectiveProjection(float width, float height);
    void createOrthoProjection(float width, float height);
    void translate(glm::vec3 dir);
};


#endif  /*  */
