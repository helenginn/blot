set path+=src/**

command! Tags !ctags -R src/* 
command! Ninja :wa|!ninja -C build/current

command! Doxy !doxygen Doxyfile



