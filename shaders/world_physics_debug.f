flat in vec3 color;

out vec4 out_color;

void main()
{
 // gl_FragColor = vec4(1, 0, 0, 1);
  out_color  = vec4(color, 1);
}

