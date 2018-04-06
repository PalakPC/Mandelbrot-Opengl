#include "shader.h"
#include "stb_image.h"

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static void error_callback(int error, const char *description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

double cx = 0.0, cy = 0.0, zoom = 0.4;
int itr = 100;
int fps = 0;

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

int main()
{
    unsigned int VBO[2];
    unsigned int VAO[1];

    if(!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return 1;
    }

    atexit(glfwTerminate);
    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Palak Choudhary Mandelbrot 8", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
// set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load("name2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    float vertices[] = {
            // positions          // colors           // texture coords
            -0.5f,  1.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
            -0.5f, 0.8f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
            -1.0f,  1.0f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,    // top left

            // positions          // colors           // texture coords
            -0.5f, 0.8f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
            -1.0f, 0.8f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
            -1.0f,  1.0f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // top left
    };

    float mandyPoints[] = {
            -1.0f,  1.0f,  0.0f,
            -1.0f,  -1.0f,  0.0f,
            1.0f,  -1.0f,  0.0f,

            -1.0f,  1.0f,  0.0f,
            1.0f,  -1.0f,  0.0f,
            1.0f,  1.0f,  0.0f,
    };

    Shader ourShader("vertex.glsl", "fragment.glsl"); // you can name your shader files however you like
    Shader mandyShader("mandyVertex.glsl", "mandyFragment.glsl");

    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mandyPoints), mandyPoints, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        glUniform2d(glGetUniformLocation(mandyShader.ID, "screen_size"), (double)SCR_WIDTH, (double)SCR_HEIGHT);
        glUniform1d(glGetUniformLocation(mandyShader.ID, "screen_ratio"), (double)SCR_WIDTH / (double)SCR_HEIGHT);
        glUniform2d(glGetUniformLocation(mandyShader.ID, "center"), cx, cy);
        glUniform1d(glGetUniformLocation(mandyShader.ID, "zoom"), zoom);
        glUniform1i(glGetUniformLocation(mandyShader.ID, "itr"), itr);

        glClearColor(0.0745f, 0.098f, 0.149f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        mandyShader.use();
        glBindVertexArray(VAO[1]);
        glDrawArrays (GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO[0]);
    glDeleteBuffers(1, &VBO[0]);

    glDeleteVertexArrays(1, &VAO[1]);
    glDeleteBuffers(1, &VBO[1]);

    glfwTerminate();
    return 0;
}