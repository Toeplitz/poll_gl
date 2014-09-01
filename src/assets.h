#pragma once

#include <vector>

#include "armature.h"
#include "camera.h"
#include "glshader.h"
#include "light.h"
#include "physics_rigidbody.h"
#include "model.h"
#include "manipulator.h"
#include "material.h"
#include "mesh.h"


typedef std::vector<std::unique_ptr<Armature>>          Armature_List;
typedef std::vector<std::unique_ptr<Camera>>            Camera_List;
typedef std::vector<std::unique_ptr<Light>>             Light_List;
typedef std::vector<std::unique_ptr<Manipulator>>       Manipulator_List;
typedef std::vector<std::unique_ptr<Material>>          Material_List;
typedef std::vector<std::unique_ptr<Mesh>>              Mesh_List;
typedef std::vector<std::unique_ptr<Physics_Rigidbody>> Physics_Rigidbody_List;
typedef std::vector<std::unique_ptr<Text>>              Text_List;


class Config;
class GLcontext;
class Scene;


class Stock_Nodes 
{
  private:


    Node *node_light_sphere;
    Node *node_symbol_cone;
    Node *node_symbol_diamond;
    Node *node_symbol_disk;
    Node *node_symbol_pyramid;
    Node *node_screen_quad;

  public:

    void init(Scene &scene);
    Node *cone_get();
    Mesh *disk_get();
    Mesh *diamond_get();
    Mesh *pyramid_get();
    Mesh *screen_quad_get();
    Mesh *sphere_get();
};


class Stock_Shaders
{
  public:
    GLshader screen_light;
    GLshader screen_post_proc;
    GLshader text;
    GLshader world_basic_color;
    GLshader world_geometry;
    GLshader world_light;
    GLshader world_stencil;
    GLshader world_physics_debug;

    void init(Config &config, Scene &scene);
    void term();
};


class Assets 
{

  private:

    Armature_List armatures;
    Camera_List cameras;
    Light_List active_lights;
    Light_List inactive_lights;
    Physics_Rigidbody_List rigidbodies;
    Manipulator_List manipulators;
    Material_List materials;
    Mesh_List meshes;
    Text_List texts;

    Stock_Nodes stock_nodes;
    Stock_Shaders stock_shaders;


  public:

    void                  armature_add(std::unique_ptr<Armature> &&armature);
    Armature_List  const &armature_get_all() const;
    void                  armature_print_all() const;
    void                  camera_add(std::unique_ptr<Camera> &&camera);
    void                  camera_print_all(const Node &node) const;
    void                  init(Config &config, Scene &scene);
    void                  light_activate(Light *light);
    void                  light_active_add(std::unique_ptr<Light> &&light); 
    Light_List     const &light_active_get() const;
    Light                *light_ith_get(const int i);
    void                  light_deactivate(Light *light);
    void                  light_inactive_add(std::unique_ptr<Light> &&light);
    bool                  light_is_active(Light *light);
    void                  light_print_all(const Node &node) const;
    void                  manipulator_add(std::unique_ptr<Manipulator> &&manipulator);
    void                  manipulator_print_all(const Node &node) const;
    Material_List  const &material_get_all() const;
    void                  material_add(std::unique_ptr<Material> &&material);
    unsigned int          material_node_lookup(const Material *material, Node &node);
    void                  material_print_all(Node &node);
    void                  mesh_add(std::unique_ptr<Mesh> &&mesh);
    Mesh_List      const &mesh_get_all() const;
    unsigned int          mesh_node_lookup(const Mesh *mesh, Node &node);
    void                  mesh_print_all(Node &node);
    void                  physics_rigidbody_add(std::unique_ptr<Physics_Rigidbody> &&rigidbody);
    void                  physics_rigidbody_print_all(Node &node);
    void                  print_all(Node &node);
    Stock_Nodes          &stock_nodes_get();
    Stock_Shaders        &stock_shaders_get();
    void                  text_add(std::unique_ptr<Text> &&text);
    void                  text_print_all(const Node &node) const;
    void                  term(Scene &scene);

};

