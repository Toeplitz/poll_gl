#pragma once

#include <vector>

#include "armature.h"
#include "camera.h"
#include "light.h"
#include "physics_rigidbody.h"
#include "material.h"
#include "mesh.h"

typedef std::vector<std::unique_ptr<Armature>> Armature_Unique_Ptr_List;
typedef std::vector<std::unique_ptr<Light>> Light_Unique_Ptr_List;

class Assets {

  private:
    Armature_Unique_Ptr_List armatures;
    std::vector<std::unique_ptr<Camera>> cameras;
    std::vector<std::unique_ptr<Light>> active_lights;
    std::vector<std::unique_ptr<Light>> inactive_lights;
    std::vector<std::unique_ptr<Physics_Rigidbody>> rigidbodies;
    std::vector<std::unique_ptr<Material>> materials;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<std::unique_ptr<Text>> texts;

  public:
    Assets();
    ~Assets();

    void                                  armature_add(std::unique_ptr<Armature> &&armature);
    Armature_Unique_Ptr_List       const &armature_get_all() const;
    void                                  armature_print_all() const;
    void                                  camera_add(std::unique_ptr<Camera> &&camera);
    void                                  camera_print_all(const Node &node) const;
    void                                  light_activate(Light *light);
    void                                  light_active_add(std::unique_ptr<Light> &&light); 
    Light_Unique_Ptr_List          const &light_active_get() const;
    Light                                *light_ith_get(const int i);
    void                                  light_deactivate(Light *light);
    void                                  light_inactive_add(std::unique_ptr<Light> &&light);
    bool                                  light_is_active(Light *light);
    void                                  light_print_all(const Node &node) const;
    void                                  print_all(const Node &node) const;
    void                                  physics_rigidbody_add(std::unique_ptr<Physics_Rigidbody> &&rigidbody);
    void                                  material_add(std::unique_ptr<Material> &&material);
    unsigned int                          material_node_lookup(const Material *material, const Node &node) const;
    void                                  material_print_all(const Node &node) const;
    void                                  mesh_add(std::unique_ptr<Mesh> &&mesh);
    unsigned int                          mesh_node_lookup(const Mesh *mesh, const Node &node) const;
    void                                  mesh_print_all(const Node &node) const;
    void                                  text_add(std::unique_ptr<Text> &&text);
    void                                  text_print_all(const Node &node) const;

};

