# ===================================================================
# FLAGS
# ===================================================================
flags = -DNDEBUG -Werror -ggdb3 -std=c++17 -O3 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

speed_flags = -DNDEBUG -std=c++17 -mavx2 -fsized-deallocation -fstrict-overflow -Werror -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -Werror=vla

clang_flags = -O3 -DNDEBUG -flto=thin -ffast-math -mavx2 -mavx -flto

# ===================================================================
# FILES
# ===================================================================
asm_files = SRC/CmpString.asm


# ===================================================================
# BUILDS
# ===================================================================
debug:
	g++ SRC/main.cpp SRC/func.cpp SRC/text_func.cpp SRC/common.cpp SRC/asmo.o $(speed_flags) $(I) -O3 -g  -o mon

debug_cl:
	clang SRC/main.cpp SRC/func.cpp SRC/text_func.cpp SRC/common.cpp SRC/asmo.o $(clang_flags) $(I) -O3 -g  -o mon

prof_gen:
	g++ SRC/main.cpp SRC/func.cpp SRC/text_func.cpp SRC/common.cpp SRC/asmo.o $(speed_flags) $(I)  -fprofile-generate -O3 -g  -o mon

prof_use:
	g++ SRC/main.cpp SRC/func.cpp SRC/text_func.cpp SRC/common.cpp SRC/asmo.o $(spe) $(I) -fprofile-use -O3 -g  -o mon

prof_gen_cl:
	clang SRC/main.cpp SRC/func.cpp SRC/text_func.cpp SRC/common.cpp SRC/asmo.o $(clang_flags) $(I)  -fprofile-generate -O3 -g  -o mon

prof_use_cl:
	clang SRC/main.cpp SRC/func.cpp SRC/text_func.cpp SRC/common.cpp SRC/asmo.o $(clang_flags) $(I) -fprofile-use -O3 -g  -o mon



test:
	g++ $(I) $(speed_flags) -O3 -g -S -masm=intel  -o test

build_asm:
	nasm -f elf64 -g $(asm_files) -o SRC/asmo.o

anls: analys.cpp
	g++ analys.cpp $(flags) -o anls

run:
	taskset -c 14,15 ./mon

run_test:
	./test

run_anls:
	./anls
