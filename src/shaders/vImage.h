#ifndef __Blot__Image_vsh__
#define __Blot__Image_vsh__

std::string vImage =
"#version 330\n"\
"in vec3 normal;\n"\
"in vec3 position;\n"\
"in vec4 color;\n"\
"in vec4 extra;\n"\
"in vec2 tex;\n"\
"\n"\
"uniform mat4 projection;\n"\
"uniform mat4 model;\n"\
"uniform float time;\n"\
"\n"\
"out vec4 vColor;\n"\
"out vec4 vPos;\n"\
"out vec2 vTex;\n"\
"out float vTime;\n"\
"\n"\
"void main()\n"\
"{\n"\
"    vec4 pos = model * vec4(position, 1.0);\n"\
"    vPos = pos;\n"\
"    gl_Position = pos;\n"\
"	 vColor = color;\n"\
"	 vTex = tex;\n"\
"    vTime = time;\n"\
"}";


#endif
