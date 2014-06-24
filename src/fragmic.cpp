#include "fragmic.h"

/*
 * - Fix naming convention to lowercase/underscore and class_t
 * - Simplify GL code in Engine class? Remove all wrappers.
 * - Simplify and clean up.
 * - End goal: small and simple codebase with animated skeleton with bullet collision
 *    - Calculate all bounding boxes for all animations on initial loading.
 * - Particle animation on character when hittig something..
 */


static void toggleMouseCursor(void)
{
  static bool flagMouseCursor;
  if (flagMouseCursor)
    SDL_ShowCursor(1);
  else
    SDL_ShowCursor(0);
  flagMouseCursor = !flagMouseCursor;
}


void Fragmic::toggleDebug()
{
  /*
  static int toggle = 0;

  toggle = !toggle;

  GLuniformBuffer *buffer = buffers.getDebugBuffer();

  std::cout << "TOGGLE: " << toggle << std::endl;

  glm::vec4 d(0.5, 0.5, 0.5, toggle);
  buffer->update(d, 0);
  */
}



/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/

void Fragmic::gl_draw(Node &node)
{
  Mesh *mesh = node.mesh;
  if (!mesh) return;

  GLsizei count = (GLsizei) mesh->getSize();
  GLvoid *indices = (GLvoid *) (mesh->aabb.getNumIndices() * sizeof(GLushort));
  GLint baseVertex = (GLint) mesh->aabb.getNumVertices();
  glBindVertexArray(gl_vao);
  glDrawElementsBaseVertex(GL_TRIANGLES, count, 
      GL_UNSIGNED_SHORT, 
      indices, 
      baseVertex);

}


void Fragmic::gl_uniform_buffers_init(GLshader &shader, Camera &camera, Scene &scene)
{
  GLuint program = shader.program;
  GLint uniform_block_index;
  GLuint buffer;
  GLenum target;
  GLuint bindPointIndex;

  uniform_block_index = shader.get_block_index("GlobalMatrices");
  glUniformBlockBinding(program, uniform_block_index, UB_GLOBALMATRICES);
  uniform_block_index = shader.get_block_index("Matrices");
  glUniformBlockBinding(program, uniform_block_index, UB_MATRICES);
  uniform_block_index = shader.get_block_index("Armature");
  glUniformBlockBinding(program, uniform_block_index, UB_ARMATURE);
  uniform_block_index = shader.get_block_index("Debug");
  glUniformBlockBinding(program, uniform_block_index, UB_DEBUG);

  target = GL_UNIFORM_BUFFER;
  {
    glm::mat4 matrix[2];
    bindPointIndex = UB_GLOBALMATRICES;
    glGenBuffers(1, &gl_buffer_globalmatrices);
    glBindBuffer(target, gl_buffer_globalmatrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_globalmatrices, 0, sizeof(matrix));
    glBindBufferBase(target, bindPointIndex, gl_buffer_globalmatrices);
  }

  {
    glm::mat4 matrix;
    bindPointIndex = UB_MATRICES;
    glGenBuffers(1, &gl_buffer_matrices);
    glBindBuffer(target, gl_buffer_matrices);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_matrices, 0, sizeof(matrix));
    glBindBufferBase(target, bindPointIndex, gl_buffer_matrices);
  }

  {
    glm::mat4 matrix[64];
    bindPointIndex = UB_ARMATURE;
    glGenBuffers(1, &gl_buffer_armature);
    glBindBuffer(target, gl_buffer_armature);
    glBufferData(target, sizeof(matrix), &matrix, GL_STREAM_DRAW);
    glBindBufferRange(target, bindPointIndex, gl_buffer_armature, 0, sizeof(matrix));
    glBindBufferBase(target, bindPointIndex, gl_buffer_armature);
  }

  {
    glm::vec4 debug(0.5, 0.5, 0.5, 0);
    bindPointIndex = UB_DEBUG;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBindBufferRange(target, bindPointIndex, buffer, 0, sizeof(debug));
    glBindBufferBase(target, bindPointIndex, buffer);
  }
}

void Fragmic::gl_uniform_buffers_update(GLshader &shader, Camera &camera)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  glm::mat4 data[2];
  data[0] = camera.perspective;
  data[1] = camera.view;
  glBindBuffer(target, gl_buffer_globalmatrices);
  glBufferSubData(target, offset, sizeof(data), &data);
}


void Fragmic::gl_uniform_buffers_update_node(GLshader &shader, Node &node)
{
  GLenum target = GL_UNIFORM_BUFFER;
  GLintptr offset = 0;
  Armature *armature = node.armature;
  Material *material = node.material;
  Mesh *mesh = node.mesh;

  if (armature) {
    glBindBuffer(target, gl_buffer_armature);
    glBufferSubData(target, offset, armature->skinning_matrices.size() * sizeof(armature->skinning_matrices[0]), 
        armature->skinning_matrices.data());
  }

  if (material) {
    auto &texture = material->texture;
    if (texture) {
      glBindTexture(GL_TEXTURE_2D, texture->gl_texture);
    }
  }

  if (mesh) {
    glBindBuffer(target, gl_buffer_matrices);
    glBufferSubData(target, offset, sizeof(mesh->model), &mesh->model);
  }

}


void Fragmic::gl_vertex_buffers_add(Node &node)
{
  GLuint buffer;
  GLenum target;
  GLint index;
  Mesh *mesh = node.mesh;
  Material *material = node.material;

  if  (mesh) {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> weights;
    std::vector<glm::ivec4> bone_indices;
    std::vector<glm::vec2> uvs;
    std::vector<GLshort> indices;
    mesh->buffer_data_get(&vertices, &normals, &weights, &bone_indices, &uvs, &indices);

    glGenVertexArrays(1, &gl_vao);
    glBindVertexArray(gl_vao);

    target = GL_ARRAY_BUFFER;
    index = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 1;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 2;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, weights.size() * sizeof(weights[0]), weights.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);

    index = 3;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, bone_indices.size() * sizeof(bone_indices[0]), bone_indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 4, GL_INT, GL_FALSE, 0, 0);

    index = 4;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, uvs.size() * sizeof(uvs[0]), uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0);

    target = GL_ELEMENT_ARRAY_BUFFER;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
  }

  if (material) {
    auto &texture = material->texture;

    if (texture) {
      glGenTextures(1, &texture->gl_texture);
      glBindTexture(GL_TEXTURE_2D, texture->gl_texture);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->image->width, texture->image->height,
          0, GL_RGB, GL_UNSIGNED_BYTE, texture->image->data);
    }
  }
}


bool Fragmic::poll_ui_events()
{
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
        break;
      case SDL_KEYUP:
        ui_keyboard_callback_released(&event.key.keysym);
        break;
      case SDL_KEYDOWN:
        return ui_keyboard_callback_pressed(&event.key.keysym);
      case SDL_MOUSEBUTTONDOWN:
        ui_mouse_button_down(&event.button);
        break;
      case SDL_MOUSEBUTTONUP:
        ui_mouse_button_up(&event.button);
        break;
      case SDL_MOUSEMOTION:
        ui_mouse_motion(&event.motion);
        break;
        break;
      default:
        break;
    }
  }

  return true;
}


void Fragmic::profile_fps(Uint32 dt)
{
  static int numFrames = 0;
  static Uint32 t;

  numFrames++;
  t += dt;

  if (t >= 1000) {
    std::cout << numFrames << " frames/sec, " << 1000.0 /
      numFrames << " ms/frame" << std::endl;
    t = 0;
    numFrames = 0;
  }
}

bool Fragmic::ui_keyboard_callback_pressed(SDL_Keysym *keysym)
{
  if (ui_custom_keyboard_pressed_callback)
    ui_custom_keyboard_pressed_callback(keysym);

  switch (keysym->sym) {
    case SDLK_ESCAPE:
      return false;
      break;
    case SDLK_w:
      camera.addMove(FORWARD);
      break;
    case SDLK_q:
      camera.addMove(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      camera.addMove(BACKWARD);
      break;
    case SDLK_e:
      camera.addMove(SIDESTEP_RIGHT);
      break;
    case SDLK_m:
      toggleMouseCursor();
      toggleMouseView = !toggleMouseView;
      break;
    case SDLK_p:
      context.togglePolygonMesh(togglePolygonView);
      togglePolygonView = !togglePolygonView;
      break;
    default:
      break;
  }

  return true;
}


void Fragmic::ui_keyboard_callback_released(SDL_Keysym *keysym)
{

  switch (keysym->sym) {
    case SDLK_w:
      camera.deleteMove(FORWARD);
      break;
    case SDLK_q:
      camera.deleteMove(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      camera.deleteMove(BACKWARD);
      break;
    case SDLK_e:
      camera.deleteMove(SIDESTEP_RIGHT);
      break;
    default:
      break;
  }

}


void Fragmic::ui_mouse_button_down(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !toggleMouseView)
    return;
  camera.addMove(FORWARD);
}


void Fragmic::ui_mouse_button_up(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !toggleMouseView)
    return;
  camera.deleteMove(FORWARD);
}


void Fragmic::ui_mouse_motion(SDL_MouseMotionEvent *ev)
{
  static int last_x, last_y;

  if (!toggleMouseView)
    return;

  if (!last_x)
    last_x = ev->x;
  if (!last_y)
    last_y = ev->y;

  camera.mouseViewUpdate(window, ev->x, ev->y);

  last_x = ev->x;
  last_y = ev->y;
}


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Fragmic::Fragmic(const std::string &title, const int &width, const int &height):
  camera(width, height), 
  context(), 
  shader(), 
  scene(), 
  window(width, height),
  toggleMouseView(false),
  togglePolygonView(false)
{
  window.init(title);
  if (!context.init(window)) {
    exit(-1);
  }
  shader.load("shaders/animation.v", "shaders/animation.f");

  gl_uniform_buffers_init(shader, camera, scene);
  context.setSwapInterval(0);
}


Fragmic::~Fragmic() 
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


Camera &Fragmic::camera_get() 
{
  return camera;
}


void Fragmic::run()
{
  Uint32 lastTime = SDL_GetTicks();

  for (;;) {
    Uint32 timeNow = SDL_GetTicks();
    Uint32 dt = timeNow - lastTime;
    lastTime = timeNow;
    profile_fps(dt);

    if (!poll_ui_events()) {
      std::cout << "Fragmic exiting..." << std::endl;
      return;
    }
    window.checkError();
    context.clear();
    camera.update((double) dt / 1000.0);

    Node *upload_node = scene.upload_queue_pop();
    while (upload_node) {
      gl_vertex_buffers_add(*upload_node);
      upload_node = scene.upload_queue_pop();
    }

    for (auto &node: scene.transform_queue_get()) {
      scene.transform_update(*node, dt);
    }

    Assets &assets = scene.assets_get();
    for (auto &armature: assets.armatures_get()) {
      armature->updateBones();
    }

    /* FIXME: only update if camera changed */
    gl_uniform_buffers_update(shader, camera);
    for (auto &node: scene.render_queue_get()) {
      gl_uniform_buffers_update_node(shader, *node);
      gl_draw(*node);
    }

    context.checkError();
    context.swap(window);

  }
}


Scene &Fragmic::scene_get()
{
  return scene;
}


void Fragmic::term()
{
  context.term();
  window.term();
}


