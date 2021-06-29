#ifndef __Blot__Image_proc_fsh__
#define __Blot__Image_proc_fsh__

std::string fpImage =
"#version 330\n"\
"in vec4 vColor;\n"\
"in vec2 vTex;\n"\
"in vec4 vPos;\n"\
"in float vTime;\n"\
"\n"\
"uniform sampler2D pic_tex;\n"\
"uniform sampler2D proc_tex;\n"\
"\n"\
"out vec4 FragColor;\n"\
"\n"\
"void main()\n"\
"{\n"\
"	vec4 value = texture(pic_tex, vTex);\n"\
"	vec4 stage = texture(proc_tex, vTex);\n"\
"	FragColor = vec4(value[2], value[1], value[0], value[3]);\n"\
"	FragColor += vColor;\n"\
"	FragColor[3] = value[3] * vTime;\n"\
"	if (stage[0] > vTime)\n"\
"	{\n"\
"		FragColor[3] = 0.;\n"\
"	}\n"\
"	if (FragColor[3] < 0.01)\n"\
"	{\n"\
"		discard;\n"\
"	}\n"\
"\n"\
"\n"\
"\n"\
"\n"\
"}\n";


#endif

