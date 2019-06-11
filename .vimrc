set path+=src/**

command! Tags !ctags -R libgui/* libsrc/*
command! Ninja :wa|!ninja -C build/current

command! Doxy !doxygen Doxyfile



