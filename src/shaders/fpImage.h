#ifndef __Blot__Image_proc_fsh__
#define __Blot__Image_proc_fsh__

std::string fpImage =
"varying vec4 vColor;\n"\
"varying vec2 vTex;\n"\
"varying vec4 vPos;\n"\
"varying float vTime;\n"\
"\n"\
"uniform sampler2D pic_tex;\n"\
"uniform sampler2D proc_tex;\n"\
"\n"\
"void main()\n"\
"{\n"\
"	vec2 tex = vec2(vTex[0], vTex[1]);\n"\
"	vec4 value = texture2D(pic_tex, tex);\n"\
"	vec4 stage = texture2D(proc_tex, tex);\n"\
"	gl_FragColor = vec4(value[2], value[1], value[0], value[3]);\n"\
"	gl_FragColor += vColor;\n"\
"	gl_FragColor[3] = value[3] * vTime;\n"\
"	if (stage[0] > vTime)\n"\
"	{\n"\
"		gl_FragColor[3] = 0.;\n"\
"	}\n"\
"\n"\
"\n"\
"\n"\
"}\n";


#endif

