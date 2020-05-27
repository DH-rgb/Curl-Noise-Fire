#include <stdio.h>
#include <stdlib.h>
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>

#include "shadersource.h"

int readShaderSource(GLuint shader, const char *file)
{
  FILE *fp;
  const GLchar *source;
  GLsizei length;
  int ret;
  
  fp = fopen(file, "rb");
  if (fp == NULL) {
    perror(file);
    return -1;
  }
  
  fseek(fp, 0L, SEEK_END);
  length = ftell(fp);
  
  source = (GLchar *)malloc(length);
  if (source == NULL) {
    fprintf(stderr, "Could not allocate read buffer.\n");
    return -1;
  }
  
  fseek(fp, 0L, SEEK_SET);
  ret = fread((void *)source, 1, length, fp) != (size_t)length;
  fclose(fp);
  
  if (ret)
    fprintf(stderr, "Could not read file: %s.\n", file);
  else
    glShaderSource(shader, 1, &source, &length);
  
  free((void *)source);
  
  return ret;
}

/*
ログの確認*/
void printShaderInfoLog(GLuint shader)
{
  GLsizei bufSize;
  
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
  
  if (bufSize > 1) {
    GLchar *infoLog = (GLchar *)malloc(bufSize);
    
    if (infoLog != NULL) {
      GLsizei length;
      
      glGetShaderInfoLog(shader, bufSize, &length, infoLog);
      fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
      free(infoLog);
    }
    else
      fprintf(stderr, "Could not allocate InfoLog buffer.\n");
  }
}

/*
ログの確認*/
void printProgramInfoLog(GLuint program)
{
  GLsizei bufSize;
  
  glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);
  
  if (bufSize > 1) {
    GLchar *infoLog = (GLchar *)malloc(bufSize);
    
    if (infoLog != NULL) {
      GLsizei length;
      
      glGetProgramInfoLog(program, bufSize, &length, infoLog);
      fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
      free(infoLog);
    }
    else
      fprintf(stderr, "Could not allocate InfoLog buffer.\n");
  }
}
