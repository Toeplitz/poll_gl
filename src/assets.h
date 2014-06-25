#ifndef ASSETS_H
#define ASSETS_H

#include <memory>
#include <vector>
#include <thread>
#include <mutex>

#include "armature.h"
#include "material.h"
#include "mesh.h"


class Assets {
  private:
    std::vector <std::unique_ptr <Armature>> armatures;
    std::vector <std::unique_ptr <Material>> materials;
    std::vector <std::unique_ptr <Mesh>> meshes;

  public:

    Assets() {
    } 


    ~Assets() {
    }

    void addMaterial(std::unique_ptr<Material> &&material) {
      materials.push_back(std::move(material));
    }


    void addMesh(std::unique_ptr<Mesh> &&mesh) {
      meshes.push_back(std::move(mesh));
    }


    void addArmature(std::unique_ptr<Armature> &&armature) {
      armatures.push_back(std::move(armature));
    }


    std::vector<std::unique_ptr<Armature>> const &armatures_get() const 
    {
      return armatures;
    } 


};

#endif
