#include "model.h"


static void copyaiMat(const aiMatrix4x4 * from, glm::mat4 & to)
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

  template < typename Key, typename Value >
static const Value & lookupIn(const Key & key, const std::map < Key,
    Value > &map)
{
  typename std::map < Key, Value >::const_iterator iter = map.find(key);
  assert(iter != map.end());
  return iter->second;
}

Model::Model():
  armaturePtr(nullptr)
{
}


Model::~Model()
{
  std::cout << "Deleting model object" << std::endl;
}


void Model::createBoneMap(Assets & assets, BoneForAssimpBone & boneForAssimpBone)
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
      copyaiMat(&assimpBone->mOffsetMatrix, offsetMatrix);

      std::unique_ptr<Bone> internalBone(new Bone(key, boneIndex, offsetMatrix, node));

      assert(!boneForAssimpBone[assimpBone]);
      assert(internalBone->jointNode);

      boneForAssimpBone[assimpBone] = internalBone.get();
      armature->bones_add(std::move(internalBone));
      ++boneIndex;
    }
  }

  if (armature->bones_num_get() <= 0) {
    return;
  }

  Node *armatureRoot = armature->find_toplevel_node();
  if (armatureRoot) {
    armatureRoot->armature = armature.get();
  }
  armaturePtr = armature.get();
  assets.armature_add(std::move(armature));
}

Node *Model::createNodeMap(const aiNode & node, Node * parent, int level)
{
  glm::mat4 localTransform;
  std::string key(node.mName.data);
  auto internalNode = std::unique_ptr<Node>(new Node(key));
  Node *nodePtr = internalNode.get();

  nodes[key] = internalNode.get();
  copyaiMat(&node.mTransformation, localTransform);
  internalNode->local_transform_original_set(localTransform);
  internalNode->local_transform_current_set(localTransform);

  for (size_t i = 0; i < node.mNumChildren; i++) {
    createNodeMap(*node.mChildren[i], internalNode.get(), level + 1);
  }

  parent->child_add(std::move(internalNode), level + 1);

  return nodePtr;
}

void Model::createMeshes(Assets & assets, const aiNode & node,
    const BoneForAssimpBone & boneForAssimpBone)
{
  createMesh(assets, node, boneForAssimpBone);
  for (unsigned int i = 0; i < node.mNumChildren; i++) {
    createMeshes(assets, *node.mChildren[i], boneForAssimpBone);
  }

}

void Model::createMesh(Assets &assets, const aiNode & node,
    const BoneForAssimpBone & boneForAssimpBone)
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
        vertexWeight = aiVertexWeight(bone->get_index(), assimpBone->mWeights[iw].mWeight);
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
      m.model = bone->jointNode->transform_global * bone->offsetMatrix;
    }

    if (!assimpMesh->mNumBones) {
      std::cout << "Fragmic Info: no bones for mesh, setting model transform:" << std::endl;
      printMatrix(std::cout, m.model, 0);
      m.model = meshNode->transform_global;
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

    meshNode->mesh = meshPtr.get();
    meshNode->armature = armaturePtr;
    assets.mesh_add(std::move(meshPtr));
  }

}


void Model::parseKeyFrames(void)
{
  if (!scene->HasAnimations()) {
    std::cout << "Fragmic WARNING: model '" << this << "'has no keyframes" << std::endl;
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


void Model::parseMaterials(Assets &assets)
{
  if (!scene->HasMaterials()) {
    std::cout << "Fragmic warning: model '" << this << "'has no materials" << std::endl;
    return;
  }

  std::cout << "Number of materials: " << scene->mNumMaterials << std::endl;

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

    }

    {
      int texIndex = 0;
      aiString path;
      aiReturn texFound = assimpMaterial.GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
      if (texFound == AI_SUCCESS) {
        if (texIndex > 0) {
          std::cout << "Fragmic warning: more then one diffuse texture for material" << std::endl;
          std::cout << "NOT CURRENTLY SUPPORTED!" << std::endl;
        }

        std::cout << "\tDiffuse texture file: " << path.data << std::endl;
        std::unique_ptr<Texture> texturePtr(new Texture());
        Texture &texture = *texturePtr;
        texture.loadImage(prefix + std::string(path.data));
        material.texture = std::move(texturePtr);

        texIndex++;
        texFound = assimpMaterial.GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
      }
    }

    materials.push_back(materialPtr.get());
    assets.material_add(std::move(materialPtr));
  }
}



Node *Model::load(Assets &assets, Node &root,
    const std::string &_prefix, const std::string &filename)
{
  Transform t;
  std::string fullName = _prefix + "/" + filename;
  prefix = _prefix;

  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 3);
  scene = importer.ReadFile(fullName.c_str(), aiProcess_Triangulate |
      aiProcess_GenSmoothNormals |
      aiProcess_FlipUVs | aiProcess_LimitBoneWeights);

  Node *rootPtr = createNodeMap(*scene->mRootNode, &root, root.tree_level);

  t.calculateGlobalTransformTopDown(*rootPtr);

  BoneForAssimpBone boneForAssimpBone;
  createBoneMap(assets, boneForAssimpBone);

  parseMaterials(assets);
  //parseTextures();

  createMeshes(assets, *scene->mRootNode, boneForAssimpBone);

  parseKeyFrames();


  return rootPtr;
}
