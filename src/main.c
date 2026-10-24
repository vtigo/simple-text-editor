#include <GLFW/glfw3.h>
#include <gl/gl.h>
#include <stdio.h>

void error_callback(int error, const char *description)
{
    fprintf(stderr, "error(%d): %s\n", error, description);
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(640, 480, "simple text editor", NULL, NULL);

    glfwMakeContextCurrent(window);
    
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
