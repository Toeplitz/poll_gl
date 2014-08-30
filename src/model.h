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
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "utils.h"


class Scene;


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
    typedef std::map<const aiBone *, Bone *> BoneForAssimpBone;
    typedef std::map<std::string, Node *> NodeMap;
    
    std::vector<Material *>  materials;
    NodeMap                  nodes;
    const aiScene           *assimp_scene;
    std::string              prefix;
    Armature                *armature_ptr = nullptr;

    void   assimp_material_add_texture(Material &material, aiMaterial &assimp_material, Model_Texture_Type type);
    void   ai_mat_copy(const aiMatrix4x4 *from, glm::mat4 &to);
    void   bone_map_create(Scene &scene, BoneForAssimpBone &boneForAssimpBone);
    void   lights_parse(Scene &scene);
    void   materials_parse(Scene &scene);
    void   mesh_create_all(Scene &scene, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone);
    void   mesh_create(Scene &scene, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone);
    Node  *node_map_create(Scene &scene, const aiNode &node, Node *parent, int level);
    void   key_frames_parse(void);

  public:

    Node *load(Scene &scene, Node &root, const std::string &prefix, const std::string &filename, const unsigned int options); 
};
