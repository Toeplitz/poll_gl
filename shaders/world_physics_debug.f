in vec3 color;
out vec4 out_color;

void main()
{
  out_color.rgb = vec3(1, 0, 0);
  //out_color.rgb = color;
  out_color.a = 1.0;
}

