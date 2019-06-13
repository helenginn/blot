#ifndef __Blot__Image_vsh__
#define __Blot__Image_vsh__

std::string vImage =
"attribute vec3 normal;\n"\
"attribute vec3 position;\n"\
"attribute vec4 color;\n"\
"attribute vec4 extra;\n"\
"attribute vec2 tex;\n"\
"\n"\
"uniform mat3 aspect;\n"\
"uniform float time;\n"\
"\n"\
"varying vec4 vColor;\n"\
"varying vec4 vPos;\n"\
"varying vec2 vTex;\n"\
"varying float vTime;\n"\
"\n"\
"void main()\n"\
"{\n"\
"    vec4 pos = vec4(aspect * position, 1.0);\n"\
"    vPos = pos;\n"\
"    gl_Position = pos;\n"\
"	 vColor = color;\n"\
"	 vTex = tex;\n"\
"    vTime = time;\n"\
"}";


#endif
