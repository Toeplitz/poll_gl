#pragma once

#include <memory>
#include <vector>

#include "armature.h"
#include "light.h"
#include "material.h"
#include "mesh.h"

typedef std::vector<std::unique_ptr<Armature>> Armature_Unique_Ptr_List;
typedef std::vector<std::unique_ptr<Light>> Light_Unique_Ptr_List;

class Assets {
  private:
    Armature_Unique_Ptr_List armatures;
    std::vector<std::unique_ptr<Material>> materials;
    std::vector<std::unique_ptr<Light>> lights;
    std::vector<std::unique_ptr<Mesh>> meshes;

  public:
    Assets();
    ~Assets();

    void                                  armature_add(std::unique_ptr<Armature> &&armature);
    Armature_Unique_Ptr_List       const &armature_get_all() const;
    void                                  armature_print_all() const;
    void                                  light_add(std::unique_ptr<Light> &&light); 
    Light_Unique_Ptr_List          const &light_get_all() const;
    void                                  light_print_all(const Node &node) const;
    void                                  print_all(const Node &node) const;
    void                                  material_add(std::unique_ptr<Material> &&material);
    unsigned int                          material_node_lookup(const Material *material, const Node &node) const;
    void                                  material_print_all(const Node &node) const;
    void                                  mesh_add(std::unique_ptr<Mesh> &&mesh);
    unsigned int                          mesh_node_lookup(const Mesh *mesh, const Node &node) const;
    void                                  mesh_print_all(const Node &node) const;
};

