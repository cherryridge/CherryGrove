#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int compile(const std::string& code, unsigned int type){
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
}

static unsigned int createShader(const std::string& vertex, const std::string& fragment){
    unsigned int id = glCreateProgram();
    unsigned int vs = compile(vertex, GL_VERTEX_SHADER);
    unsigned int fs = compile(fragment, GL_FRAGMENT_SHADER);
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    glValidateProgram(id);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return id;
}

int main(){
    GLFWwindow* window;
    if(!glfwInit()) return -1;
    window = glfwCreateWindow(640, 480, "Cherry Grove", NULL, NULL);
    if(!window){
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK) std::cout << "Error on initializing GLEW";
    std::cout << glGetString(GL_VERSION) << "\n";
    float positions[6] = {-0.4f, -0.3f, 0.0f, 0.9f, 0.6f, -0.3f};
    unsigned int one;

    glGenBuffers(1, &one);
    glBindBuffer(GL_ARRAY_BUFFER, one);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    const std::string
        vertex =
            "#version 330 core\n"
            "layout(location = 0) in vec4 position;\n"
            "void main(){\n"
            "    gl_Position = position;\n"
            "}",
        fragment =
            "#version 330 core\n"
            "layout(location = 0) out vec4 color;\n"
            "void main(){\n"
            "    color = vec4(1.0, 1.0, 0.0, 0.5);\n"
            "}";
    unsigned int shader = createShader(vertex, fragment);
    glUseProgram(shader);
    
    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}