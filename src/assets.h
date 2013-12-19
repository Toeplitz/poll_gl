#ifndef ASSETS_H
#define ASSETS_H

#include <memory>
#include <vector>
#include <thread>
#include <mutex>

#include "armature.h"
#include "material.h"
#include "mesh.h"

#include "glbuffer.h"


class Assets {
  private:
    std::vector <std::unique_ptr <Armature>> armatures;
    std::vector <std::unique_ptr <Material>> materials;
    std::vector <std::unique_ptr <Mesh>> meshes;

    std::mutex mutex; 

  public:

    Assets() {
    } 


    ~Assets() {
    }

    void addMaterial(std::unique_ptr<Material> &&material) {
      std::lock_guard<std::mutex> lock(mutex);
      materials.push_back(std::move(material));
    }


    void addMesh(std::unique_ptr <Mesh> &&mesh) {
      std::lock_guard<std::mutex> lock(mutex);
      meshes.push_back(std::move(mesh));
    }


    void addArmature(std::unique_ptr < Armature > &&armature) {
      std::lock_guard<std::mutex> lock(mutex);
      armatures.push_back(std::move(armature));
    }


    std::vector<std::unique_ptr<Armature>> const &getArmatures() const 
    {
      return armatures;
    } 


    std::mutex &getMutex()
    {
      return mutex;
    }

};

#endif
