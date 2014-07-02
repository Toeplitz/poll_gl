#include "model.h"

template <typename Key, typename Value>
static const Value &lookupIn(const Key &key, const std::map<Key, Value> &map);


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Model::Model(): armaturePtr(nullptr)
{
}


Model::~Model()
{
  //std::cout << "Deleting model object" << std::endl;
}



/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Node *Model::load(Assets &assets, Node &root, const std::string &prefix, const std::string &filename, bool lefthanded)
{
  Transform t;
  std::string full_name = prefix + "/" + filename;
  this->prefix = prefix;

  if (!file_exists(full_name)) {
    std::cout << "Model file '" << full_name << "' does not exist. Exiting ..." << std::endl;
    exit(-1);
  }

  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 3);
  if (lefthanded) {
    scene = importer.ReadFile(full_name.c_str(), aiProcess_Triangulate |
        aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded
        | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
  } else {
    scene = importer.ReadFile(full_name.c_str(), aiProcess_Triangulate |
        aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs | aiProcess_LimitBoneWeights);
  }

  Node *rootPtr = node_map_create(*scene->mRootNode, &root, root.tree_level);
  t.calculateGlobalTransformTopDown(*rootPtr);
  BoneForAssimpBone boneForAssimpBone;
  bone_map_create(assets, boneForAssimpBone);
  materials_parse(assets);

  mesh_create_all(assets, *scene->mRootNode, boneForAssimpBone);
  key_frames_parse();

  return rootPtr;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


  template <typename Key, typename Value>
static const Value &lookupIn(const Key &key, const std::map<Key, Value> &map)
{
  typename std::map < Key, Value >::const_iterator iter = map.find(key);
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
    default:
      break;
  }

  aiReturn texFound = assimp_material.GetTexture(assimp_type, texIndex, &path);

  if (texFound == AI_SUCCESS) {
    if (texIndex > 0) {
      std::cout << "Fragmic warning: more then one texture of this type for material" << std::endl;
      std::cout << "NOT CURRENTLY SUPPORTED!" << std::endl;
    }

    std::cout << "\tTexture file: " << path.data << std::endl;
    std::unique_ptr<Texture> texturePtr(new Texture());
    Texture &texture = *texturePtr;
    texture.loadImage(prefix + std::string(path.data));

    switch (type) {
      case MODEL_TEXTURE_DIFFUSE:
        material.diffuse = std::move(texturePtr);
        break;
      case MODEL_TEXTURE_NORMAL:
        material.normal = std::move(texturePtr);
        break;
      default:
        break;
    }

    texIndex++;
    texFound = assimp_material.GetTexture(assimp_type, texIndex, &path);
  }

}


void Model::ai_mat_copy(const aiMatrix4x4 * from, glm::mat4 & to)
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


void Model::bone_map_create(Assets & assets, BoneForAssimpBone & boneForAssimpBone)
{
  size_t boneIndex = 0;
  auto armature = std::unique_ptr < Armature > (new Armature());

  for (size_t i = 0; i < scene->mNumMeshes; ++i) {
    const aiMesh *mesh = scene->mMeshes[i];

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

  std::cout << "Found num bones" << armature->bones_num_get() << std::endl;

  Node *armatureRoot = armature->find_toplevel_node();
  if (armatureRoot) {
    std::cout << "Setting armature: " << armatureRoot->name << std::endl;
    armatureRoot->armature = armature.get();
  }
  armaturePtr = armature.get();
  assets.armature_add(std::move(armature));
}

Node *Model::node_map_create(const aiNode & node, Node *parent, int level)
{
  glm::mat4 localTransform;
  std::string key(node.mName.data);
  auto internalNode = std::unique_ptr<Node>(new Node(key));
  Node *nodePtr = internalNode.get();
  aiVector3t<float> scaling;
  aiQuaterniont<float> rotation;
  aiVector3t<float> position;

  node.mTransformation.Decompose(scaling, rotation, position);
  /*
     std::cout << "Report for: " << node.mName.C_Str() << std::endl;
     std::cout << "Scaling: " << scaling.x << ", " << scaling.y << ", " << scaling.z << std::endl;
     std::cout << "Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
     std::cout << "Rotation quaternion: " << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << std::endl;
     */

  glm::vec3 scale_vec(scaling.x, scaling.y, scaling.z);
  glm::vec3 position_vec(position.x, position.y, -position.z);
  glm::quat rotation_quat(rotation.x, rotation.y, -rotation.z, rotation.w);
  nodePtr->original_scaling = scale_vec;
  nodePtr->original_position = position_vec;
  nodePtr->original_rotation = rotation_quat;

  //  glm::mat4 scale_matrix = glm::scale(glm::mat4(1.f), scale_vec);
  /*
     glm::mat4 translate_matrix = glm::translate(glm::mat4(1.f), position_vec);
     glm::mat4 neg_translate_matrix = glm::translate(glm::mat4(1.f), -position_vec);
     glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(rotation.x, rotation.y, rotation.z, rotation.w));
     glm::mat4 blender_compensatinon = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(0.f, 1.f, 0.f));
     */

  //  localTransform = translate_matrix * rotation_matrix * scale_matrix * blender_compensatinon;

  nodes[key] = internalNode.get();
  ai_mat_copy(&node.mTransformation, localTransform);
  //internalNode->local_transform_original_set(right_handed_to_left_handed(localTransform));
  //internalNode->local_transform_current_set(right_handed_to_left_handed(localTransform));
  //  localTransform = translate_matrix * rotation_matrix * scale_matrix;
  internalNode->local_transform_original_set(localTransform);
  internalNode->local_transform_current_set(localTransform);

  for (size_t i = 0; i < node.mNumChildren; i++) {
    node_map_create(*node.mChildren[i], internalNode.get(), level + 1);
  }

  parent->child_add(std::move(internalNode), level + 1);

  return nodePtr;
}




void Model::materials_parse(Assets &assets)
{
  if (!scene->HasMaterials()) {
    std::cout << "Fragmic warning: model '" << this << "'has no materials" << std::endl;
    return;
  }

  //std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;

  for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
    aiMaterial &assimpMaterial = *scene->mMaterials[i];
    std::unique_ptr<Material> materialPtr(new Material());
    Material &material = *materialPtr;

    std::cout << "\tProperties: " << assimpMaterial.mNumProperties <<std::endl;
    std::cout << "\tNone textures: " << assimpMaterial.GetTextureCount(aiTextureType_NONE) <<std::endl;
    std::cout << "\tAmbient textures: " << assimpMaterial.GetTextureCount(aiTextureType_AMBIENT) <<std::endl;
    std::cout << "\tDiffuse textures: " << assimpMaterial.GetTextureCount(aiTextureType_DIFFUSE) <<std::endl;
    std::cout << "\tSpecular textures: " << assimpMaterial.GetTextureCount(aiTextureType_SPECULAR) <<std::endl;
    std::cout << "\tEmissive textures: " << assimpMaterial.GetTextureCount(aiTextureType_EMISSIVE) <<std::endl;
    std::cout << "\tNormals textures: " << assimpMaterial.GetTextureCount(aiTextureType_NORMALS) <<std::endl;
    std::cout << "\tLightmap textures: " << assimpMaterial.GetTextureCount(aiTextureType_LIGHTMAP) <<std::endl;
    {
      aiColor4D color;
      aiReturn ret;

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_AMBIENT, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tAmbient (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tDiffuse (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" << std::endl;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tSpecular (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" << std::endl;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_EMISSIVE, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tEmissive (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_COLOR_REFLECTIVE, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tReflective (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_SHININESS, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tShininess (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
      }

      ret = aiGetMaterialColor(&assimpMaterial, AI_MATKEY_SHININESS_STRENGTH, &color);
      if (ret == AI_SUCCESS) {
        std::cout << "\tShininess strength (r,g,b,a) = (" << color.r << ","
          << color.g << "," << color.b
          << "," << color.a << ")" <<std::endl;
      }
    }

    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_DIFFUSE);
    assimp_material_add_texture(material, assimpMaterial, MODEL_TEXTURE_NORMAL);

    materials.push_back(materialPtr.get());
    assets.material_add(std::move(materialPtr));
  }
}


void Model::mesh_create_all(Assets &assets, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone)
{
  mesh_create(assets, node, boneForAssimpBone);
  for (unsigned int i = 0; i < node.mNumChildren; i++) {
    mesh_create_all(assets, *node.mChildren[i], boneForAssimpBone);
  }
}


void Model::mesh_create(Assets &assets, const aiNode &node, const BoneForAssimpBone &boneForAssimpBone)
{
  if (!node.mNumMeshes) {
    return;
  }

  std::string nodeName = std::string(node.mName.data);
  Node *meshNode = lookupIn(nodeName, nodes);
  assert(meshNode);

  for (unsigned int i = 0; i < node.mNumMeshes; i++) {
    unsigned int meshIndex = node.mMeshes[i];
    aiMesh *assimpMesh = scene->mMeshes[meshIndex];
    std::unique_ptr <Mesh> meshPtr(new Mesh());
    Mesh &m = *meshPtr;

    // Point the mesh to the existing material, corresponding to the index
    meshNode->material = materials[assimpMesh->mMaterialIndex];

    for (unsigned int ii = 0; ii < assimpMesh->mNumFaces; ii++) {
      const aiFace & face = assimpMesh->mFaces[ii];

      if (face.mNumIndices < 3) {
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

      Bone *bone = lookupIn(assimpBone, boneForAssimpBone);
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
      m.model = bone->skinning_matrix_update();
      //m.model = bone->joint_node->transform_global * bone->offset_matrix;
    }

    if (!assimpMesh->mNumBones) {
      //      m.model = glm::rotate(meshNode->transform_global, -90.f, glm::vec3(1.f, 0.f, 0.f));
      m.model = meshNode->transform_global;
      //m.model = right_handed_to_left_handed(meshNode->transform_global);
    }

    for (unsigned int iv = 0; iv < assimpMesh->mNumVertices; iv++) {
      Vertex v;
      v.position.x = assimpMesh->mVertices[iv].x;
      v.position.y = assimpMesh->mVertices[iv].y;
      v.position.z = assimpMesh->mVertices[iv].z;
      if (assimpMesh->HasNormals()) {
        v.normal.x = assimpMesh->mNormals[iv].x;
        v.normal.y = assimpMesh->mNormals[iv].y;
        v.normal.z = assimpMesh->mNormals[iv].z;
      }

      if (assimpMesh->HasTangentsAndBitangents()) {
        v.tangent.x = assimpMesh->mTangents[iv].x;
        v.tangent.y = assimpMesh->mTangents[iv].y;
        v.tangent.z = assimpMesh->mTangents[iv].z;

        v.bitangent.x = assimpMesh->mBitangents[iv].x;
        v.bitangent.y = assimpMesh->mBitangents[iv].y;
        v.bitangent.z = assimpMesh->mBitangents[iv].z;

        /*
        std::cout << "tanget x,y,z = " << v.tangent.x  << ", " << v.tanget.y << ", " << v.tangent.z << std::endl;
        std::cout << "bitanget x,y,z = " << v.bitangent.x  << ", " << v.bitangent.y << ", " << v.bitangent.z << std::endl;
        */
      }


      unsigned int p = 0;
      while (assimpMesh->HasTextureCoords(p)) {
        v.uv.x = assimpMesh->mTextureCoords[p][iv].x;
        v.uv.y = assimpMesh->mTextureCoords[p][iv].y;
        if (p > 0) {
          std::cout << "Fragmic warning: more than one set of texture coordinates for one mesh" << std::endl;
          std::cout << "NOT CURRENTLY SUPPORTED!" << std::endl;
        }
        p++;
      }

      m.vertices.push_back(v);
      assert(weightsPerVertex[iv].size() <= 4);

      for (unsigned int a = 0; a < weightsPerVertex[iv].size(); a++) {
        int id = weightsPerVertex[iv][a].mVertexId;
        m.vertices[iv].bones[a] = id;
        m.vertices[iv].weights[a] = weightsPerVertex[iv][a].mWeight;
      }
    }

    meshPtr->scale_matrix = glm::scale(glm::mat4(1.0f), meshNode->original_scaling);
    meshNode->mesh = meshPtr.get();
    meshNode->armature = armaturePtr;
    assets.mesh_add(std::move(meshPtr));
  }

}


void Model::key_frames_parse(void)
{
  if (!scene->HasAnimations()) {
    //std::cout << "Fragmic WARNING: model '" << this << "'has no keyframes" << std::endl;
    return;
  }

  for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
    aiAnimation *animation = scene->mAnimations[i];

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

        node.addKeyFrame(s, q, t, animNode->mPositionKeys[npk].mTime);
      }
    }
  }
}
