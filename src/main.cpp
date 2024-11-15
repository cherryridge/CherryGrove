#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>
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
    unsigned int shader1 = createShader(vertex, fragment);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 600, 0);
    glClearColor(0.73f, 0.753f, 0.95f, 1.0f);

    FTGLPixmapFont* font = new FTGLPixmapFont("LXGWWenKai-Regular.ttf");
    if(font->Error()){
        std::cout << "Error loading font!" << std::endl;
        return -1;
    }
    font->FaceSize(24);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(0);
        glPushMatrix();
        glTranslatef(60.0f, 60.0f, 0.1f);
        glScalef(6.0f, 6.0f, 3.0f);
        font->Render("Hello World!");
        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    delete font;
    glfwTerminate();
    glfwDestroyWindow(window);
    return 0;
}