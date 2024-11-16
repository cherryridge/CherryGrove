#ifndef SHADER_H
#define SHADER_H

class Shader{
    private:
        unsigned int id;
        static unsigned int compile(const std::string& code, unsigned int type);
    public:
        Shader(const std::string& vertexShader, const std::string& fragmentShader);
        Shader(FILE vertexShaderFile, FILE fragmentShaderFile);
        bool attach();
        bool detach();
        
};

#endif