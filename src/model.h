#pragma once

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
#include "utils.h"


enum Model_Texture_Type
{
  MODEL_TEXTURE_DIFFUSE,
  MODEL_TEXTURE_NORMAL,
  MODEL_TEXTURE_HEIGHT,
  MODEL_TEXTURE_ALPHA,
  MODEL_TEXTURE_SPECULAR
};


enum Model_Import_Options
{
  MODEL_IMPORT_OPTIMIZED = 1 << 0,
  MODEL_IMPORT_LIGHTS = 1 << 1,
  MODEL_IMPORT_NO_DRAW = 1 << 2,
  MODEL_IMPORT_DEFAULT = 1 << 3
};


class Model {
  private:
    typedef std::map <const aiBone *, Bone *> BoneForAssimpBone;
    typedef std::map <std::string, Node *> NodeMap;
    
    std::vector<Material *>  materials;
    NodeMap                  nodes;
    const aiScene           *scene;
    std::string              prefix;
    Armature                *armature_ptr;

    void   assimp_material_add_texture(Material &material, aiMaterial &assimp_material, Model_Texture_Type type);

    void   ai_mat_copy(const aiMatrix4x4 *from, glm::mat4 &to);
    void   bone_map_create(Assets &assets, BoneForAssimpBone &boneForAssimpBone);
    void   lights_parse(Assets &assets);
    void   materials_parse(Assets &assets);
    void   mesh_create_all(Assets &assets, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone);
    void   mesh_create(Assets &assets, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone);
    Node  *node_map_create(const aiNode &node, Node *parent, int level);
    void   key_frames_parse(void);

  public:
    Model();
    ~Model();

    Node *load(Assets &assets, Node &root, const std::string &prefix, const std::string &filename, const unsigned int options); 
};
