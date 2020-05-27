#version 120
//
// simple.frag
//
varying float lifeColor;

void main(void)
{
  gl_FragColor = vec4(1, lifeColor*0.8, lifeColor*0.6, 1.0);// (2,2,1.2)--> (1,0.3,0.14)
}
