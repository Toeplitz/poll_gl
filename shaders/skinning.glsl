
mat4 func_animation_matrix_get()
{
  mat4 animation = 
    weights[0] * skinning[int(bone_index[0])] +
    weights[1] * skinning[int(bone_index[1])] +
    weights[2] * skinning[int(bone_index[2])];

  //if (weights[0] == 0.0 && weights[1] == 0.0 && weights[2] == 0.0) {
  //  animation = model;
  //}

  return animation;
}

