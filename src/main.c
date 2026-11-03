#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


#define WINDOW_NAME   "simple text editor"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600


void callback_error(int error, const char *description);
void callback_framebuffersize(GLFWwindow *window, int width, int heigth);

void shutdown_fatal(const char *fmt, ...);
void process_input(GLFWwindow *window);
const char* get_vertex_shader_source(void);
const char* get_fragment_shader_source(void);
GLuint shader_compile(GLenum shader_type, const char *src);
GLuint shader_program_link(GLuint vertex_shader, GLuint fragment_shader);


int main(void)
{   
    glfwSetErrorCallback(callback_error);

    // window & context setup

    if (!glfwInit())
        shutdown_fatal("failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          WINDOW_NAME,
                                          NULL, NULL);

    if (!window)
        shutdown_fatal("failed to create window");

    glfwMakeContextCurrent(window);

    // caps the loop to the monitor refresh rate (requests for vsync).
    glfwSwapInterval(1);

    // load open function pointers
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        shutdown_fatal("failed to load OpenGL functions");
    
    // set viewport
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);
    glfwSetFramebufferSizeCallback(window, callback_framebuffersize);


    // compile shaders and link program

    GLuint vertex_shader, fragment_shader, shader_program;

    vertex_shader   = shader_compile(GL_VERTEX_SHADER, get_vertex_shader_source());
    fragment_shader = shader_compile(GL_FRAGMENT_SHADER, get_fragment_shader_source());

    shader_program  = shader_program_link(vertex_shader, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    // load vertex data

    const float vertices[] = {
        // triangulo
       -0.5f, -0.5f, 0.0f, // inferior esquerdo
        0.5f, -0.5f, 0.0f, // inferior direito
        0.0f,  0.5f, 0.0f  // superior central
    };
    
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shader_program);


    // event loop
    while (!glfwWindowShouldClose(window))
    {
        // rendering commands
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // swap the buffers and react to events
        glfwSwapBuffers(window);
        glfwPollEvents();
        process_input(window);
    }
    
    // cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


GLuint shader_compile(GLenum shader_type, const char *src)
{
    int success;
    char info_log[512];
    GLuint shader;

    shader = glCreateShader(shader_type);

    glShaderSource(shader, 1, &src, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        shutdown_fatal("failed to compile shader: %s", info_log);
    }

    return shader;
}

GLuint shader_program_link(GLuint vertex_shader, GLuint fragment_shader)
{
    int success;
    char info_log[512];
    GLuint shader_program;

    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        shutdown_fatal("failed to link shader program: %s", info_log);
    }
    
    return shader_program;
}

const char* get_vertex_shader_source(void)
{
    return "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
}

const char* get_fragment_shader_source(void)
{
    return "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";
}

// no cleanup on purpose: the OS reclaims the window, GL context and objects
void shutdown_fatal(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);
}

void callback_error(int error, const char *description)
{
    (void)error;
    fprintf(stderr, "error: %s\n", description);
}

void callback_framebuffersize(GLFWwindow *window, int width, int heigth)
{
    (void)window;
    glViewport(0, 0, width, heigth);
}
