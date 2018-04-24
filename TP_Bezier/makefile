#Makefile pour les courbes de Bézier
#R. RAFFIN, Dept Info, Arles

compilateur = g++
preproc_options = -c -Wall
link_options = -Wall
libs = -lGLEW -lGL -lGLU -lglut
main = Projet_Gimenez

all: $(main)
	@echo "\ncompilation terminee"

$(main): point3.o Projet_Gimenez.o
	@echo "--- compilation de $@ ..."
	$(compilateur) $(link_options) $^ $(libs) -o $@
	@echo "--- compilation de $@ terminée"

point3.o: point3.cpp point3.h
	$(compilateur) $(preproc_options) $<

Projet_Gimenez.o: Projet_Gimenez.cpp
	$(compilateur) $(preproc_options) $<

clean:
	rm -f *.o $(main) *~

archive:
	tar czvf $(main).tgz *.cpp *.h makefile


run: all
	./$(main)

