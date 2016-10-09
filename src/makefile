OBJ = test.o basicblock.o binary.o codecache.o counting_bb.o counting_insn.o global.o intercept.o isa.o map.o stack.o top_level.o
CC = gcc
link_flag = -static
#link_flag = 
C_FLAG = -g

a.out : $(OBJ)
	${CC} ${C_FLAG}  ${link_flag} -o a.out $(OBJ)

test.o : test.c
	${CC} ${C_FLAG} -c -o test.o test.c

basicblock.o : basicblock.c
	${CC} ${C_FLAG} -c -o basicblock.o basicblock.c

binary.o :binary.c
	${CC} ${C_FLAG} -c -o binary.o binary.c

codecache.o : codecache.c
	${CC} ${C_FLAG} -c -o codecache.o codecache.c
counting_bb.o : counting_bb.c
	${CC} ${C_FLAG} -c -o counting_bb.o counting_bb.c
counting_insn.o : counting_insn.c
	${CC} ${C_FLAG} -c -o counting_insn.o counting_insn.c
global.o : global.c
	${CC} ${C_FLAG} -c -o global.o global.c
intercept.o : intercept.c
	${CC} ${C_FLAG} -c -o intercept.o intercept.c
isa.o	: isa.c
	${CC} ${C_FLAG} -c -o isa.o isa.c
map.o	: map.c
	${CC} ${C_FLAG} -c -o map.o map.c
stack.o	:stack.c
	${CC} ${C_FLAG} -c -o stack.o stack.c
top_level.o	:top_level.c
	${CC} ${C_FLAG} -c -o top_level.o top_level.c
