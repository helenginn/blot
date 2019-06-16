project('blot', 'cpp', 'c')
qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'], required : true)
dep_gl = dependency('gl', required : true)
png_dep = dependency('libpng')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# Run the version and hash generator
hash_script = find_program('get_hash.sh')

gen_src = custom_target('commit-output',
                        input : ['src/commit.h.pre'],
                        output : ['commit.h'],
                        command : [hash_script, '@INPUT@', '@OUTPUT@'],
                        build_by_default: true,
                        build_always_stale: true)

# Graphical user interface
# (including all the Qt and OpenGL stuff)

# Pre-processing by Qt5

moc_files = qt5.preprocess(moc_headers : [
'src/BlotGL.h', 
'src/Library.h', 
'src/StartScreen.h',
'src/Properties.h', 
],
		moc_extra_arguments: ['-DMAKES_MY_MOC_HEADER_COMPILE'])

executable('blot', gen_src, moc_files,
'src/main.cpp', 
'src/mat3x3.cpp', 
'src/vec3.cpp', 
'src/BaseParser.cpp',
'src/Parser.cpp',
'src/BlotGL.cpp', 
'src/BlotObject.cpp', 
'src/ChangeBackground.cpp', 
'src/Library.cpp', 
'src/ImageAppear.cpp', 
'src/ImageHide.cpp', 
'src/ImageMove.cpp', 
'src/ImageProc.cpp', 
'src/Instruction.cpp', 
'src/Properties.cpp', 
'src/StartScreen.cpp', 
'src/StateValue.cpp', 
'src/WipeSlate.cpp', 
cpp_args: ['-std=c++11'], dependencies : [ qt5_dep, dep_gl, png_dep ], install: true)

