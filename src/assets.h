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

    // Pointers for light properties for fast submission to gpu.
    std::vector<Light_Properties *> light_properties;

  public:
    Assets();
    ~Assets();

    void                                  armature_add(std::unique_ptr<Armature> &&armature);
    Armature_Unique_Ptr_List       const &armature_get_all() const;
    void                                  armature_print_all();
    unsigned int                          mesh_node_lookup(const Mesh *mesh, const Node &node);
    void                                  light_add(std::unique_ptr<Light> &&light);
    std::vector<Light_Properties *>  const &light_properties_get_all() const;
    Light_Unique_Ptr_List          const &light_get_all() const;
    void                                  light_print_all(const Node &node);
    void                                  material_add(std::unique_ptr<Material> &&material);
    unsigned int                          material_node_lookup(const Material *material, const Node &node);
    void                                  material_print_all(const Node &node);
    void                                  mesh_add(std::unique_ptr<Mesh> &&mesh);
    void                                  mesh_print_all(const Node &node);
    void                                  print_all(const Node &node);

};

