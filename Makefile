all:	 scanner

scanner:	lex.l grammar.y scanner.h scanner.c
	bison -d -Wall -t grammar.y  --report=state -ogrammar.c  && \
	flex -omy.lex.c   lex.l    && \
	g++ -g -o $@ grammar.c my.lex.c scanner.c   -I./require/gflags/include -L./require/gflags/lib  -lgflags -lfl -std=c++11 
															
clean:
	rm -f  scanner \
	grammar.c grammar.h my.lex.c
