#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "gl/gl_core_4_0.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    bool addVertex(std::string fileName);
    bool addGeometry(std::string fileName);
    bool addFragment(std::string fileName);
    bool addTesselationControl(std::string fileName);
    bool addTesselationEvaluation(std::string fileName);
    bool link(void);
    bool bind(void);
    bool isLinked(void);
    GLuint programId(void);
    void release(void);
    GLint attributeLocation(std::string attribName);
    GLint uniformLocation(std::string uniformName);
    void deleteProgram(void);
private:
    struct {
        GLuint vertex;
        GLuint fragment;
        GLuint geometry;
        GLuint tesselation_control;
        GLuint tesselation_evaluation;
    } _shaders;
    bool _contextInitialized;
    GLuint _shaderHandle;

    GLuint compileShader(GLenum kind,std::string fileName);

    std::map<std::string, GLint> _attribMap;
    std::map<std::string, GLint> _uniformMap;

    bool oglInit();
};

#endif // SHADERPROGRAM_H
