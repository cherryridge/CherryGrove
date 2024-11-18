/*#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include "shader.h"

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader){
    id = glCreateProgram();
    unsigned int vs = Shader::compile(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = Shader::compile(fragmentShader, GL_FRAGMENT_SHADER);
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    glValidateProgram(id);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::Shader(FILE vertexShaderFile, FILE fragmentShaderFile){

}

bool Shader::attach(){
    glUseProgram(id);
    return true;
}

bool Shader::detach(){
    glUseProgram(0);
    return true;
}

unsigned int Shader::compile(const std::string& code, unsigned int type){
    unsigned int id = glCreateShader(type);
    const char* src = code.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* error = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, error);
        std::cout << "Error in" << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << "Shader:" << error << "\n";
        glDeleteShader(id);
        return 0;
    }
    return id;
}*/