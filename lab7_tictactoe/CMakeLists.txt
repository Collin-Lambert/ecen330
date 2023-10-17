add_executable(lab7.elf main_m1.c minimax.c testBoards.c ticTacToeDisplay.c)
target_link_libraries(lab7.elf ${330_LIBS} interrupts intervalTimer touchscreen buttons_switches)
set_target_properties(lab7.elf PROPERTIES LINKER_LANGUAGE CXX)