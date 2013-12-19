#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "armature.h"
#include "assets.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "transform.h"
#include "printmatrix.h"


class Model {
  private:
    typedef std::map <const aiBone *, Bone *> BoneForAssimpBone;
    typedef std::map <std::string, Node *> NodeMap;
    std::vector<Material *> materials;

    NodeMap nodes;
    const aiScene *scene;
    std::string prefix;

    Armature *armaturePtr;

    void createBoneMap(Assets & assets, BoneForAssimpBone & boneForAssimpBone);
    void createMeshes(Assets & assets, const aiNode & node,
        const BoneForAssimpBone & boneForAssimpBone);
    void createMesh(Assets & assets, const aiNode & node,
        const BoneForAssimpBone & boneForAssimpBone);
    Node *createNodeMap(const aiNode & node, Node * parent, int level);
    void parseKeyFrames(void);
    void parseMaterials(Assets &assets);


  public:
    Model();
    ~Model();

    Node *load(Assets &assets, Node &root,
        const std::string &prefix, const std::string &filename);

};


#endif
