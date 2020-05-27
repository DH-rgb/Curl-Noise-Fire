#ifndef SHADERSOURCE_H
#define SHADERSOURCE_H
#  include <GL/gl.h>

extern int readShaderSource(GLuint shader, const char *file);
extern void printShaderInfoLog(GLuint shader);
extern void printProgramInfoLog(GLuint program);

#endif
