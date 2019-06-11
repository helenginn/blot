project('vagabond', 'cpp', 'c')
qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'], required : true)
dep_gl = dependency('gl', required : true)
png_dep = dependency('libpng')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

# Run the version and hash generator
hash_script = find_program('get_hash.sh')

# This will produce the dependency file.
run_command('get_hash.sh')

gen_src = custom_target('commit-output',
                        input : ['commit.h.pre'],
                        output : ['commit.h'],
                        command : [hash_script, '@INPUT@', '@OUTPUT@'],
                        build_by_default: true,
                        build_always_stale: true)

# Meat of the vagabond library, minus command line-specific or GUI-specific source files.

# Graphical user interface
# (including all the Qt and OpenGL stuff)

# Pre-processing by Qt5

moc_files = qt5.preprocess(moc_headers : [],
		moc_extra_arguments: ['-DMAKES_MY_MOC_HEADER_COMPILE'])

# Non-Qt5 GUI elements (mostly OpenGL stuff)
# Uses Qt5-supplied OpenGL

executable('vgui',
'main.cpp', 
cpp_args: ['-w', '-std=c++11'], dependencies : [ qt5_dep, dep_gl, png_dep ], install: true)

