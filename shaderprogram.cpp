#include "shaderprogram.h"

ShaderProgram::ShaderProgram()
{
    _contextInitialized = false;
    _shaders.fragment = 0;
    _shaders.vertex = 0;
    _shaders.tesselation_control = 0;
    _shaders.geometry = 0;
    _shaders.tesselation_evaluation = 0;
    _shaderHandle = 0;


}

ShaderProgram::~ShaderProgram()
{
    deleteProgram();
}

inline bool ShaderProgram::oglInit(){
    if (!_contextInitialized)
    {
        int result = ogl_LoadFunctions();
        if (result == ogl_LOAD_FAILED)
            return false;
        else
            _contextInitialized = true;
    }
    return true;
}

GLuint ShaderProgram::compileShader(GLenum kind,std::string fileName)
{
    if (!oglInit())
        return 0;
    GLuint handle = 0;

    std::ifstream ifs(fileName.c_str());

    if (!ifs.good())
    {
        std::cout << "Error reading shader " << fileName << "\n";
        return 0;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();

    std::string shaderSource = buffer.str();
    ifs.close();

    if (shaderSource.empty())
    {
        std::cout << "Error reading shader " << fileName << "\n";
        return 0;
    }

    switch(kind)
    {
    case GL_VERTEX_SHADER:
    case GL_FRAGMENT_SHADER:
    case GL_GEOMETRY_SHADER:
    case GL_TESS_CONTROL_SHADER:
    case GL_TESS_EVALUATION_SHADER:
        handle = glCreateShader(kind);
    default:;
    };

    if (handle == 0)
        return 0;
    const char *c_str = shaderSource.c_str();
    glShaderSource(handle,1,&c_str,NULL);
    glCompileShader(handle);
    GLint status = 0;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&status);
    if (status == GL_FALSE)
    {
        std::cout << "Error compiling shader " << fileName <<": ";

        GLint infoLogLength;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(handle, infoLogLength, NULL, strInfoLog);
        std::cout << strInfoLog;
        glDeleteShader(handle);
        delete strInfoLog;
        return 0;
    }
    return handle;
}


bool ShaderProgram::addVertex(std::string fileName){
    if (!oglInit())
        return false;
    glDeleteShader(_shaders.vertex);
    _shaders.vertex = compileShader(GL_VERTEX_SHADER,fileName);
    if (_shaders.vertex != 0)
        return true;
    return false;
}

bool ShaderProgram::addGeometry(std::string fileName){
    if (!oglInit())
        return false;
    glDeleteShader(_shaders.geometry);
    _shaders.geometry = compileShader(GL_GEOMETRY_SHADER,fileName);
    if (_shaders.geometry != 0)
        return true;
    return false;
}

bool ShaderProgram::addFragment(std::string fileName){
    if (!oglInit())
        return false;
    glDeleteShader(_shaders.fragment);
    _shaders.fragment = compileShader(GL_FRAGMENT_SHADER,fileName);
    if (_shaders.fragment != 0)
        return true;
    return false;
}

bool ShaderProgram::addTesselationControl(std::string fileName){
    if (!oglInit())
        return false;
    glDeleteShader(_shaders.tesselation_control);
    _shaders.tesselation_control = compileShader(GL_TESS_CONTROL_SHADER,fileName);
    if (_shaders.tesselation_control != 0)
        return true;
    return false;
}

bool ShaderProgram::addTesselationEvaluation(std::string fileName){
    if (!oglInit())
        return false;
    glDeleteShader(_shaders.tesselation_evaluation);
    _shaders.tesselation_evaluation = compileShader(GL_TESS_EVALUATION_SHADER,fileName);
    if (_shaders.tesselation_evaluation != 0)
        return true;
    return false;
}

bool ShaderProgram::link(void){
    if (!oglInit())
        return false;

    glDeleteProgram(_shaderHandle);

    GLuint programHandle = glCreateProgram();
    if (programHandle == 0)
        return false;

    if (_shaders.vertex != 0)
    {
        glAttachShader(programHandle,_shaders.vertex);
    }
    if (_shaders.fragment != 0)
    {
        glAttachShader(programHandle,_shaders.fragment);
    }
    if (_shaders.geometry != 0)
    {
        glAttachShader(programHandle,_shaders.geometry);
    }
    if (_shaders.tesselation_control != 0)
    {
        glAttachShader(programHandle,_shaders.tesselation_control);
    }
    if (_shaders.tesselation_evaluation != 0)
    {
        glAttachShader(programHandle,_shaders.tesselation_evaluation);
    }

    glLinkProgram(programHandle);

    GLint status = 0;
    glGetProgramiv(programHandle,GL_LINK_STATUS,&status);
    if (status == GL_FALSE)
    {
        std::cout << "Error compiling program: ";

        GLint infoLogLength;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(programHandle, infoLogLength, NULL, strInfoLog);
        std::cout << strInfoLog;
        glDeleteProgram(programHandle);
        delete strInfoLog;
        return 0;
    }
    glValidateProgram(programHandle);
    glGetProgramiv(programHandle,GL_VALIDATE_STATUS,&status);
    if (status == GL_FALSE)
    {
        std::cout << "Error validating program: ";

        GLint infoLogLength;
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(programHandle, infoLogLength, NULL, strInfoLog);
        std::cout << strInfoLog;
        glDeleteProgram(programHandle);
        delete strInfoLog;
        return 0;
    }

    _shaderHandle = programHandle;
    return true;
}

bool ShaderProgram::bind(void){
    if (!oglInit() || _shaderHandle == 0)
        return false;

    glUseProgram(_shaderHandle);
    return true;
}

bool ShaderProgram::isLinked(void){
    if (!oglInit())
        return false;
    if (_shaderHandle != 0)
        return true;
    return false;
}

GLuint ShaderProgram::programId(void){
    return _shaderHandle;
}

void ShaderProgram::release(void){
    if (oglInit())
        glUseProgram(0);
}

GLint ShaderProgram::attributeLocation(std::string attribName){
    if (!oglInit())
        return -1;

    std::map<std::string,GLint>::iterator loc;
    loc = _attribMap.find(attribName);
    GLint aLoc = -1;
    if (loc == _attribMap.end())
    {
        aLoc = glGetAttribLocation(_shaderHandle,attribName.c_str());
        _attribMap[attribName] = aLoc;
    } else {
        aLoc = _attribMap[attribName];
    }
    return aLoc;
}

GLint ShaderProgram::uniformLocation(std::string uniformName){
    if (!oglInit())
        return -1;

    std::map<std::string,GLint>::iterator loc;
    loc = _uniformMap.find(uniformName);
    GLint uLoc = -1;
    if (loc == _uniformMap.end())
    {
        uLoc = glGetUniformLocation(_shaderHandle,uniformName.c_str());
        _uniformMap[uniformName] = uLoc;
    } else {
        uLoc = _uniformMap[uniformName];
    }
    return uLoc;
}

void ShaderProgram::deleteProgram()
{
    if(oglInit())
    {
        if (_shaders.vertex != 0)
        {
            glDeleteShader(_shaders.vertex);
        }
        if (_shaders.fragment != 0)
        {
            glDeleteShader(_shaders.fragment);
        }
        if (_shaders.geometry != 0)
        {
            glDeleteShader(_shaders.geometry);
        }
        if (_shaders.tesselation_control != 0)
        {
            glDeleteShader(_shaders.tesselation_control);
        }
        if (_shaders.tesselation_evaluation != 0)
        {
            glDeleteShader(_shaders.tesselation_evaluation);
        }

        if (_shaderHandle != 0)
        {
            glDeleteProgram(_shaderHandle);
        }
        _attribMap.clear();
        _uniformMap.clear();
    }
}
