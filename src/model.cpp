#include "assets.h"
#include "model.h"
#include "scene.h"
#include <glm/gtx/string_cast.hpp>

template <typename Key, typename Value>
static const Value &lookup_node(const Key &key, const std::map<Key, Value> &map);


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Node *Model::load(Scene &scene, Node &root, const std::string &prefix, const std::string &filename, const unsigned int options)
{
  unsigned int pflags;
  unsigned int remove_flags;
  std::string full_name = prefix + "/" + filename;
  this->prefix = prefix;


  if (!file_exists(full_name)) {
    std::cout << "Model file '" << full_name << "' does not exist. Exiting ..." << std::endl;
    exit(-1);
  }

  pflags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | 
           aiProcess_FindDegenerates | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | 
           aiProcess_LimitBoneWeights | aiProcess_RemoveComponent | aiProcess_FixInfacingNormals;
  
  remove_flags = aiComponent_COLORS | aiComponent_LIGHTS | aiComponent_CAMERAS;

  if (options & MODEL_IMPORT_OPTIMIZED) {
    pflags |= aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_RemoveComponent | aiProcess_FindInstances;
  }

  if (options & MODEL_IMPORT_LIGHTS) {
    remove_flags = aiComponent_COLORS | aiComponent_CAMERAS;
  }

  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 3);
  importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, remove_flags);
  assimp_scene = importer.ReadFile(full_name.c_str(), pflags);

  if (!assimp_scene) {
    std::cout << "Error parsing '" <<  full_name.c_str() << "': " << importer.GetErrorString() << std::endl;
    exit(-1);
  }

  Node *rootPtr = node_map_create(scene, *assimp_scene->mRootNode, &root, root.tree_level_get());
//  rootPtr->transform_update_global_recursive(*rootPtr);
  BoneForAssimpBone boneForAssimpBone;
  bone_map_create(scene, boneForAssimpBone);
  materials_parse(scene);
  lights_parse(scene);

  mesh_create_all(scene, *assimp_scene->mRootNode, boneForAssimpBone);
  key_frames_parse();

  return rootPtr;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


  template <typename Key, typename Value>
static const Value &lookup_node(const Key &key, const std::map<Key, Value> &map)
{
  typename std::map<Key, Value>::const_iterator iter = map.find(key);
  assert(iter != map.end());
  return iter->second;
}


void Model::assimp_material_add_texture(Material &material, aiMaterial &assimp_material, Model_Texture_Type type)
{
  int texIndex = 0;
  aiString path;
  aiTextureType assimp_type;

  switch (type) {
    case MODEL_TEXTURE_DIFFUSE:
      assimp_type = aiTextureType_DIFFUSE;
      break;
    case MODEL_TEXTURE_NORMAL:
      assimp_type = aiTextureType_NORMALS;
      break;
    case MODEL_TEXTURE_HEIGHT:
      assimp_type = aiTextureType_HEIGHT;
      break;
    case MODEL_TEXTURE_ALPHA:
      assimp_type = aiTextureType_OPACITY;
      break;
    case MODEL_TEXTURE_SPECULAR:
      assimp_type = aiTextureType_SPECULAR;
      break;
    default:
      break;
  }

  aiReturn texFound = assimp_material.GetTexture(assimp_type, texIndex, &path);

  if (texFound == AI_SUCCESS) {
    if (texIndex > 0) {
      std::cout << "Fragmic warning: more then one texture of this type for material" << std::endl;
      std::cout << "NOT CURRENTLY SUPPORTED!" << std::endl;
    }

    //std::cout << "\tTexture file: " << path.data << std::endl;
    std::unique_ptr<Texture> texturePtr(new Texture());
    Texture &texture = *texturePtr;
    texture.image_load(prefix + std::string(path.data));

    switch (type) {
      case MODEL_TEXTURE_DIFFUSE:
        material.diffuse = std::move(texturePtr);
        break;
      case MODEL_TEXTURE_NORMAL:
        material.normal = std::move(texturePtr);
        break;
      case MODEL_TEXTURE_HEIGHT:
        material.height = std::move(texturePtr);
        break;
      case MODEL_TEXTURE_ALPHA:
        material.alpha = std::move(texturePtr);
        break;
      case MODEL_TEXTURE_SPECULAR:
        material.specular = std::move(texturePtr);
        break;
      default:
        break;
    }

    texIndex++;
    texFound = assimp_material.GetTexture(assimp_type, texIndex, &path);
  }

}


void Model::ai_mat_copy(const aiMatrix4x4 *from, glm::mat4 &to)
{
  to[0][0] = from->a1;
  to[1][0] = from->a2;
  to[2][0] = from->a3;
  to[3][0] = from->a4;
  to[0][1] = from->b1;
  to[1][1] = from->b2;
  to[2][1] = from->b3;
  to[3][1] = from->b4;
  to[0][2] = from->c1;
  to[1][2] = from->c2;
  to[2][2] = from->c3;
  to[3][2] = from->c4;
  to[0][3] = from->d1;
  to[1][3] = from->d2;
  to[2][3] = from->d3;
  to[3][3] = from->d4;
}


void Model::bone_map_create(Scene &scene, BoneForAssimpBone & boneForAssimpBone)
{
  Assets &assets = scene.assets_get();
  size_t boneIndex = 0;
  auto armature = std::unique_ptr<Armature> (new Armature());

  for (size_t i = 0; i < assimp_scene->mNumMeshes; ++i) {
    const aiMesh *mesh = assimp_scene->mMeshes[i];

    if (mesh->mNumBones <= 0) {
      continue;
    }

    for (size_t n = 0; n < mesh->mNumBones; ++n) {
      const aiBone *assimpBone = mesh->mBones[n];
      auto &node = nodes.at(assimpBone->mName.data);
      auto key = std::string(assimpBone->mName.data);
      glm::mat4 offsetMatrix;
      ai_mat_copy(&assimpBone->mOffsetMatrix, offsetMatrix);
      std::unique_ptr<Bone> internalBone(new Bone(key, boneIndex, offsetMatrix, node));
      assert(!boneForAssimpBone[assimpBone]);
      boneForAssimpBone[assimpBone] = internalBone.get();
      armature->bones_add(std::move(internalBone));
      ++boneIndex;
    }
  }

  if (armature->bones_num_get() <= 0) {
    return;
  }

  Node *armature_root = armature->find_toplevel_node();
  POLL_DEBUG(std::cout, "armature top level node: " << armature_root->name_get());
  if (armature_root) {
    armature_root->armature_set(armature.get());
    armature_root_ptr = armature_root;
  }
  armature_ptr = armature.get();
  assets.armature_add(std::move(armature));
}

void Model::lights_parse(Scene &scene)
{
  if (!assimp_scene->HasLights())
    return;

  /*
  for (unsigned int i = 0; i < assimp_scene->mNumLights; i++) {
    aiLight &assimp_light = *assimp_scene->mLights[i];
    std::string key(assimp_light.mName.data);
    Node *light_node = lookup_node(key, nodes);
    assert(light_node);

    Light &light = *light_node->light_create(scene);
    {
      float r = assimp_light.mColorAmbient.r;
      float g = assimp_light.mColorAmbient.g;
      float b = assimp_light.mColorAmbient.b;
      glm::vec3 ambient(r, g, b);

      r = assimp_light.mColorDiffuse.r;
      g = assimp_light.mColorDiffuse.g;
      b = assimp_light.mColorDiffuse.b;
      glm::vec3 diffuse(r, g, b);

      r = assimp_light.mColorSpecular.r;
      g = assimp_light.mColorSpecular.g;
      b = assimp_light.mColorSpecular.b;
      glm::vec3 specular(r, g, b);

      light.properties_set(ambient, diffuse, specular);
    }


    {
      float x = assimp_light.mDirection.x;
      float y = assimp_light.mDirection.y;
      float z = assimp_light.mDirection.z;
      glm::vec3 light_direction(x, y, z);
      light.properties_direction_set(glm::vec3(0, -1, 0));
    }

    switch (assimp_light.mType) {
      case aiLightSource_DIRECTIONAL:
        light.properties_type_set(LIGHT_DIRECTIONAL);
        break;
      case aiLightSource_POINT:
        light.properties_type_set(LIGHT_POINT);
        break;
      case aiLightSource_SPOT:
        light.properties_type_set(LIGHT_SPOT);
        break;
      default:
        light.properties_type_set(LIGHT_UNDEFINED);
        break;
    }

  }
  */

}


Node *Model::node_map_create(Scene &scene, const aiNode &node, Node *parent, int level)
{
  glm::mat4 localTransform;
  std::string key(node.mName.data);
  Node *node_internal = scene.node_create(key, parent);

  {
    aiVector3t<float> scaling;
    aiQuaterniont<float> rotation;
    aiVector3t<float> position;

    node.mTransformation.Decompose(scaling, rotation, position);
    glm::vec3 scale_vec(scaling.x, scaling.y, scaling.z);
    glm::vec3 position_vec(position.x, position.y, -position.z);
    glm::quat rotation_quat(rotation.x, rotation.y, -rotation.z, rotation.w);
    ai_mat_copy(&node.mTransformation, localTransform);

    node_internal->transform_local_original_set(localTransform);
    node_internal->transform_local_current_set(scene, localTransform);
  }

  nodes[key] = node_internal;

  for (size_t i = 0; i < node.mNumChildren; i++) {
    node_map_create(scene, *node.mChildren[i], node_internal, level + 1);
  }

  return node_internal;
}



void Model::materials_parse(Scene &scene)
{
  Assets &assets = scene.assets_get();

  if (!assimp_scene->HasMaterials()) {
    std::cout << "Fragmic warning: model '" << this << "'has no materials" << std::endl;
    return;
  }

  //std::cout << "Number of materials: " << assimp_scene->mNumMaterials << std::endl;

  for (unsigned int i = 0; i < assimp_scene->mNumMaterials; i++) {
    aiMaterial &assimpMaterial = *assimp_scene->mMaterials[i];

    std::unique_ptr<Material> materialPtr(new Material());
    Material &material = *materialPtr;
    
        /*
    std::cout << "\tProperties: " << assimpMaterial.mNumProperties <<std::endl;
    std::cout << "\tAllocated: " << assimpMaterial.mNumAllocated <<std::endl;
    std::cout << "\tNone textures: " << assimpMaterial.GetTextureCount(aiTextureType_NONE) <<std::endl;
    std::cout << "\tAmbient textures: " << assimpMaterial.GetTextureCount(aiTextureType_AMBIENT) <<std::endl;
    std::cout << "\tDiffuse textures: " << assimpMaterial.GetTextureCount(aiTextureType_DIFFUSE) <<std::endl;
    std::cout << "\tSpecular textures: " << assimpMaterial.GetTextureCount(aiTextureType_SPECULAR) <<std::endl;
    std::cout << "\tEmissive textures: " << assimpMaterial.GetTextureCount(aiTextureType_EMISSIVE) <<std::endl;
    std::cout << "\tNormals textures: " << assimpMaterial.GetTextureCount(aiTextureType_NORMALS) <<std::endl;
    std::cout << "\tLightmap textures: " << assimpMaterial.GetTextureCount(aiTextureType_LIGHTMAP) <<std::endl;
    */

    {
      aiColor4D color;
      aiReturn ret;

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_AMBIENT, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tAmbient (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
          */
        material.properties.Ka.x = color.r;
        material.properties.Ka.y = color.g;
        material.properties.Ka.z = color.b;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tDiffuse (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" << std::endl;
          */
        material.properties.Kd.x = color.r;
        material.properties.Kd.y = color.g;
        material.properties.Kd.z = color.b;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tSpecular (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" << std::endl;
          */
        material.properties.Ks.x = color.r;
        material.properties.Ks.y = color.g;
        material.properties.Ks.z = color.b;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_EMISSIVE, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tEmissive (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
          */
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_REFLECTIVE, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tReflective (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
          */
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_SHININESS, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tShininess (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
          */
        material.properties.shininess = color.r;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_SHININESS_STRENGTH, &color);
      if (ret == AI_SUCCESS) {
        /*
        std::cout << "\tShininess strength (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" << std::endl;
          */
      }
    }

    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_DIFFUSE);
    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_NORMAL);
    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_HEIGHT);
    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_ALPHA);
    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_SPECULAR);

    materials.push_back(materialPtr.get());
    assets.material_add(std::move(materialPtr));
  }
}


void Model::mesh_create_all(Scene &scene, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone)
{
  mesh_create(scene, node, boneForAssimpBone);
  for (unsigned int i = 0; i < node.mNumChildren; i++) {
    mesh_create_all(scene, *node.mChildren[i], boneForAssimpBone);
  }
}


void Model::mesh_create(Scene &scene, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone)
{
  if (!node.mNumMeshes) {
    return;
  }

  std::string node_name = std::string(node.mName.data);
  Node *mesh_node = lookup_node(node_name, nodes);
  assert(mesh_node);
  //std::cout << "Node '" << mesh_node->name << "' has " << node.mNumMeshes << " mesh(es)" << std::endl;

  for (unsigned int i = 0; i < node.mNumMeshes; i++) {
    unsigned int meshIndex = node.mMeshes[i];
    aiMesh *assimpMesh = assimp_scene->mMeshes[meshIndex];

    //std::unique_ptr<Mesh> mesh_ptr(new Mesh());
    //Mesh &m = *mesh_ptr;

    //auto sub_node = std::unique_ptr<Node>(new Node(key + std::string("_") + std::to_string(i)));
    Node *parent_node = mesh_node;
    std::string key(node.mName.data);

    if (node.mNumMeshes > 1) {
      auto sub_node = scene.node_create(key + std::string("_") + std::to_string(i), parent_node);
      mesh_node = sub_node;
    }

    Mesh &m = *mesh_node->mesh_create(scene);

    // Point the mesh to the existing material, corresponding to the index
    mesh_node->material_set(materials[assimpMesh->mMaterialIndex]);
    //std::cout << "Node: " << mesh_node->name << " has material " << mesh_node->material << " and num faces/vertices: " << assimpMesh->mNumFaces << "/" << assimpMesh->mNumVertices << std::endl;

    m.num_faces = assimpMesh->mNumFaces;
    for (unsigned int ii = 0; ii < assimpMesh->mNumFaces; ii++) {
      const aiFace &face = assimpMesh->mFaces[ii];

      if (face.mNumIndices < 3) {
        std::cout << "Found degenerate polygon" << std::endl;
        // degenerate polygon -- no need to draw it
      } else {
        assert(face.mNumIndices == 3);

        for (unsigned int ii = 0; ii < face.mNumIndices; ii++) {
          unsigned int index = face.mIndices[ii];
          m.indices.push_back(index);
        }
      }
    }

    // Add the bones for skinning.
    std::vector <std::vector<aiVertexWeight>>weightsPerVertex(assimpMesh->mNumVertices);
    for (unsigned int ib = 0; ib < assimpMesh->mNumBones; ib++) {
      const aiBone *assimpBone = assimpMesh->mBones[ib];

      Bone *bone = lookup_node(assimpBone, boneForAssimpBone);
      assert(bone);

      for (unsigned int iw = 0; iw < assimpBone->mNumWeights; iw++) {
        int id = assimpBone->mWeights[iw].mVertexId;
        aiVertexWeight vertexWeight;
        vertexWeight = aiVertexWeight(bone->index_get(), assimpBone->mWeights[iw].mWeight);
        weightsPerVertex[id].push_back(vertexWeight);
      }

      // The bone matrix specifies how to transform the mesh globally to make it
      // match the bones.  This should end up being the same for all the bones that
      // are driving the same mesh.  Unforutnately this assumes that the skeleton
      // isn't parented to a moving node.  To do this more correctly, we'll need
      // to make a guess at where the skeleton root is and calculate how to
      // transform mesh coordinates to skeleton root coordinates.  The Mesh::model
      // will then need to be calculated on each frame as 
      // mesh.model = mesh.skeletonNode->transform_global * mesh.skeletonTransform
      // We can set mesh.skeletonNode to mesh.node and mesh.skeletonTransform
      // to identity if the mesh has no bones.
      glm::mat4 m = bone->skinning_matrix_update();
      mesh_node->transform_local_current_set(scene, m);


      //m.model = bone->joint_node->transform_global * bone->offset_matrix;
    }

    if (!assimpMesh->mNumBones) {
      //      m.model = glm::rotate(mesh_node->transform_global, -90.f, glm::vec3(1.f, 0.f, 0.f));
 //     glm::mat4 m = mesh_node->transform_global_get();
     // mesh_node->transform_local_current_set(scene, m);
      //m.model = right_handed_to_left_handed(mesh_node->transform_global);
    }

    if (assimpMesh->mNumBones > 0) {
      m.bone_indices.resize(assimpMesh->mNumVertices);
      m.bone_weights.resize(assimpMesh->mNumVertices);
    }

    for (unsigned int iv = 0; iv < assimpMesh->mNumVertices; iv++) {
      m.positions.push_back(glm::vec3(assimpMesh->mVertices[iv].x, assimpMesh->mVertices[iv].y, assimpMesh->mVertices[iv].z));

      if (assimpMesh->HasNormals()) {
        m.normals.push_back(glm::vec3(assimpMesh->mNormals[iv].x, assimpMesh->mNormals[iv].y, assimpMesh->mNormals[iv].z));
      }
      if (assimpMesh->HasTangentsAndBitangents()) {
        m.tangents.push_back(glm::vec3(assimpMesh->mTangents[iv].x, assimpMesh->mTangents[iv].y,assimpMesh->mTangents[iv].z));
        m.bitangents.push_back(glm::vec3(assimpMesh->mBitangents[iv].x, assimpMesh->mBitangents[iv].y, assimpMesh->mBitangents[iv].z));
      }

      unsigned int p = 0;
      while (assimpMesh->HasTextureCoords(p)) {
        m.texture_st.push_back(glm::vec2(assimpMesh->mTextureCoords[p][iv].x, assimpMesh->mTextureCoords[p][iv].y));
        if (p > 0) {
          std::cout << "Fragmic warning: more than one set of texture coordinates for one mesh" << std::endl;
          std::cout << "NOT CURRENTLY SUPPORTED!" << std::endl;
        }
        p++;
      }

      assert(weightsPerVertex[iv].size() <= 3);

      for (unsigned int a = 0; a < weightsPerVertex[iv].size(); a++) {
        int id = weightsPerVertex[iv][a].mVertexId;
        m.bone_indices[iv][a] = id;
        m.bone_weights[iv][a] = weightsPerVertex[iv][a].mWeight;
      }
    }

    //m.scale_matrix = glm::scale(glm::mat4(1.0f), mesh_node->original_scaling_get());
    //mesh_node->mesh_set(mesh_ptr.get());
    mesh_node->armature_set(armature_ptr);
    if (node.mNumMeshes > 1) {
//      mesh_node->copy_transform_data(*parent_node);
      //parent_node->child_add(std::move(sub_node), parent_node->tree_level_get() + 1);
    }


    //mesh_node->physics_rigidbody_create(scene);
    //assets.mesh_add(std::move(mesh_ptr));
    mesh_node = lookup_node(node_name, nodes);
  }

}


void Model::key_frames_parse(void)
{
  if (!assimp_scene->HasAnimations()) {
    //std::cout << "Fragmic WARNING: model '" << this << "'has no keyframes" << std::endl;
    return;
  }

  for (unsigned int i = 0; i < assimp_scene->mNumAnimations; i++) {
    aiAnimation *animation = assimp_scene->mAnimations[i];

    for (unsigned int j = 0; j < animation->mNumChannels; j++) {
      aiNodeAnim *animNode = animation->mChannels[j];
      std::string key = std::string(animNode->mNodeName.data);
      Node &node = *nodes.at(key);

      for (unsigned int npk = 0; npk < animNode->mNumPositionKeys; npk++) {
        aiVector3D pos = animNode->mPositionKeys[npk].mValue;
        aiVector3D scale = animNode->mScalingKeys[npk].mValue;
        aiQuaternion rot = animNode->mRotationKeys[npk].mValue;

        glm::vec3 s(scale.x, scale.y, scale.z);
        glm::quat q(rot.w, rot.x, rot.y, rot.z);
        glm::vec3 t(pos.x, pos.y, pos.z);

        if (animNode->mPositionKeys[npk].mTime != animNode->mScalingKeys[npk].mTime ||
            animNode->mPositionKeys[npk].mTime != animNode->mRotationKeys[npk].mTime ||
            animNode->mScalingKeys[npk].mTime != animNode->mRotationKeys[npk].mTime) {
          std::cout << "Fragmic WARNING: different animation times exist!" << std::endl;
        }

        node.keyframe_add(s, q, t, animNode->mPositionKeys[npk].mTime);
      }
    }
  }
}
