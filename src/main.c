#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>


#define WINDOW_NAME   "simple text editor"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define VS_SOURCE_FILEPATH "src/shaders/main.vert"
#define FS_SOURCE_FILEPATH "src/shaders/main.frag"


void callback_error(int error, const char *description);
void callback_framebuffersize(GLFWwindow *window, int width, int heigth);

void shutdown_fatal(const char *fmt, ...);
void input_process(GLFWwindow *window);

const char* shader_source_read(const char *filepath);
GLuint shader_compile(GLenum shader_type, const char *src);
GLuint shader_program_link(GLuint vertex_shader, GLuint fragment_shader);
// GLuint shader_program_build(const char *vs_src, const char *fs_src);


int main(void)
{   
    glfwSetErrorCallback(callback_error);

    // window & context setup

    if (!glfwInit())
        shutdown_fatal("failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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

    const char *vs_source = shader_source_read(VS_SOURCE_FILEPATH);
    if (!vs_source)
        shutdown_fatal("failed to read vertex shader source");

    const char *fs_source = shader_source_read(FS_SOURCE_FILEPATH);
    if (!fs_source)
        shutdown_fatal("failed to read fragment shader source");

    GLuint vertex_shader   = shader_compile(GL_VERTEX_SHADER, vs_source);
    GLuint fragment_shader = shader_compile(GL_FRAGMENT_SHADER, fs_source);
    GLuint shader_program  = shader_program_link(vertex_shader, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);


    // load vertex data

    const float vertices[] = {
        // triangulo
       -0.5f, -0.5f, 0.0f, // inferior esquerdo
        0.5f, -0.5f, 0.0f, // inferior direito
        0.0f,  0.5f, 0.0f  // superior central
    };
    
    unsigned int vao, vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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
        input_process(window);
    }
    
    // cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

const char* shader_source_read(const char *filepath)
{
    FILE *f = fopen(filepath, "rb");
    if (!f) return NULL;

    size_t cap = 4096, len = 0, n = 0, available;

    char *buf = malloc(cap);
    if (!buf) {
        fclose(f);
        return NULL;
    };

    for(;;) {
        available = cap - len - 1; // - 1 for the terminator
        n = fread(buf + len, 1, available, f);
        len += n;

        // short read
        if (n < available) break;
        
        // buffer capacity wasnt enough
        // double it and keep reading
        
        // max buffer size guard
        if (cap > SIZE_MAX / 2) {
            free(buf);
            fclose(f);
            return NULL;
        }
        
        // double the capacity
        char *tmp = realloc(buf, cap * 2);
        if (!tmp) {
            free(buf);
            fclose(f);
            return NULL;
        }

        buf = tmp;
        cap *= 2;
    }

    if (ferror(f)) {
        free(buf);
        fclose(f);
        return NULL;
    }

    buf[len] = '\0';
    return (const char *) buf;
}


GLuint shader_compile(GLenum shader_type, const char *src)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        shutdown_fatal("failed to compile shader: %s", info_log);
    }

    return shader;
}

GLuint shader_program_link(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        shutdown_fatal("failed to link shader program: %s", info_log);
    }
    
    return shader_program;
}

// GLuint shader_program_build(const char *vs_src, const char *fs_src);


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

void input_process(GLFWwindow *window)
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
