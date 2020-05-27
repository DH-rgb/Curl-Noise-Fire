LDLIBS = -L/usr/X11R6/lib -lglut -lGLU -lGL -lXmu -lXi -lXext -lX11 -lpthread -lm
TARGETS = main
OBJECTS = src/main.o src/shadersource.o src/Matrix.o src/Trackball.o

$(TARGETS): $(OBJECTS)
	$(CXX) $(CFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

main.o: src/shadersource.h src/Matrix.h src/Trackball.h
shadersource.o: src/shadersource.h
Matrix.o: src/Matrix.h
Trackball.o: src/Trackball.h

clean:
	-rm -f $(TARGETS) *.o *~
