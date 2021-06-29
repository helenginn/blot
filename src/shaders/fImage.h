#ifndef __Blot__Image_fsh__
#define __Blot__Image_fsh__

std::string fImage =
"#version 330\n"\
"in vec4 vColor;\n"\
"in vec2 vTex;\n"\
"in vec4 vPos;\n"\
"in float vTime;\n"\
"\n"\
"uniform sampler2D pic_tex;\n"\
"\n"\
"out vec4 FragColor;\n"\
"\n"\
"void main()\n"\
"{\n"\
"	vec2 tex = vec2(vTex[0], vTex[1]);\n"\
"	vec4 temp = texture2D(pic_tex, tex);\n"\
"	FragColor = vec4(temp[2], temp[1], temp[0], temp[3]);\n"\
"	FragColor += vColor;\n"\
"	FragColor[3] *= vTime;\n"\
"\n"\
"\n"\
"\n"\
"}\n";


#endif
