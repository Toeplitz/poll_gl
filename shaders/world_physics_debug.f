flat in vec3 color;

void main()
{
 // gl_FragColor = vec4(1, 0, 0, 1);
  gl_FragColor  = vec4(color, 1);
}

