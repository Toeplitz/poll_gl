#ifndef ARMATURE_H
#define ARMATURE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <vector>

#include "bone.h"
#include "node.h"
#include "glbuffer.h"
#include "gldefaults.h"

class Armature {
  private:
    std::vector<std::unique_ptr<Bone>> bones;
    std::vector<glm::mat4> skinningMatrices;

  public:
    Armature();
    ~Armature();

    void addBone(std::unique_ptr<Bone> &&bone);
    unsigned int getNumBones();
    Node *findArmatureRootNode();
    void updateBones();
    void update(GLuniformBuffer *buffer);

};


#endif
