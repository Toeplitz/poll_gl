#version 330

in vec2 st;

uniform sampler2D tex;
uniform sampler2D pos;

out vec4 frag_color;

void main()
{
  vec3 out_color = vec3(0.7, 0.7, 0.7);

  if (st.s >= 0.5) {
    out_color = 1 - texture(tex, st).rgb;
  } else {
    out_color = texture(tex, st).rgb;
  }

  out_color = texture(pos, st).rgb;
  frag_color.rgb = out_color;
  frag_color.a = 1.0; 
}



