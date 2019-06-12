#ifndef __Blot__Image_fsh__
#define __Blot__Image_fsh__

std::string fImage =
"varying vec4 vColor;\n"\
"varying vec2 vTex;\n"\
"varying vec4 vPos;\n"\
"\n"\
"uniform sampler2D pic_tex;\n"\
"\n"\
"void main()\n"\
"{\n"\
"	vec2 tex = vec2(vTex[0], vTex[1]);\n"\
"	vec4 temp = texture2D(pic_tex, tex);\n"\
"	gl_FragColor = vec4(temp[2], temp[1], temp[0], temp[3]);\n"\
"\n"\
"\n"\
"\n"\
"}\n";


#endif
