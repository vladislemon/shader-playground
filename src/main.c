#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef NDEBUG
#define VERTEX_SHADER_PATH "../shaders/shader.vert"
#define FRAGMENT_SHADER_PATH "../shaders/shader.frag"
#else
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"
#endif

static int WIDTH = 800;
static int HEIGHT = 600;
static char TITLE[64];
static int POS_X;
static int POS_Y;
static int FULLSCREEN = 0;
static int WINDOWED_WIDTH = 800;
static int WINDOWED_HEIGHT = 600;
static GLFWwindow *WINDOW;
static int SHADER_PROGRAM = 0;
static int WINDOW_SIZE_UNIFORM;
static int TIME_UNIFORM;
static int MOUSE_POS_UNIFORM;
static int OFFSET_UNIFORM;
static int ZOOM_UNIFORM;
static double mouseX;
static double mouseY;
//static double offsetX = 0.883249953520462316447492412407882511615753173828125000;
static double offsetX = 0.33f;
//static double offsetY = -0.020885167973717505551789486162306275218725204467773438;
static double offsetY = 0.0f;
static double zoom = 0.66f;
static int pause = 1;
static double pauseTime;
static double prevTime;

char *loadFile(const char *path, size_t *size_out) {
    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = malloc(size);
    fread(data, 1, size, file);
    fclose(file);
    *size_out = size;
    return data;
}

void reloadShaders() {
    if (SHADER_PROGRAM != 0) {
        glUseProgram(0);
        glDeleteProgram(SHADER_PROGRAM);
    }
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    size_t vertexSourceSize;
    size_t fragmentSourceSize;

    char *vertexSource = loadFile(VERTEX_SHADER_PATH, &vertexSourceSize);
    char *fragmentSource = loadFile(FRAGMENT_SHADER_PATH, &fragmentSourceSize);
    glShaderSource(vertexShader, 1, (const GLchar *const *) &vertexSource, (const GLint *) &vertexSourceSize);
    glShaderSource(fragmentShader, 1, (const GLchar *const *) &fragmentSource, (const GLint *) &fragmentSourceSize);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    SHADER_PROGRAM = glCreateProgram();
    glAttachShader(SHADER_PROGRAM, vertexShader);
    glAttachShader(SHADER_PROGRAM, fragmentShader);
    glLinkProgram(SHADER_PROGRAM);
    int success;
    glGetProgramiv(SHADER_PROGRAM, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(SHADER_PROGRAM, sizeof(log), NULL, log);
        printf("%s\n", log);
    }
    glUseProgram(SHADER_PROGRAM);
    WINDOW_SIZE_UNIFORM = glGetUniformLocation(SHADER_PROGRAM, "windowSize");
    TIME_UNIFORM = glGetUniformLocation(SHADER_PROGRAM, "time");
    MOUSE_POS_UNIFORM = glGetUniformLocation(SHADER_PROGRAM, "mousePos");
    OFFSET_UNIFORM = glGetUniformLocation(SHADER_PROGRAM, "offset");
    ZOOM_UNIFORM = glGetUniformLocation(SHADER_PROGRAM, "zoom");
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexSource);
    free(fragmentSource);
}

void setFullscreen() {
    FULLSCREEN = 1;
    const GLFWvidmode *videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowMonitor(WINDOW, glfwGetPrimaryMonitor(), 0, 0, videoMode->width, videoMode->height,
                         videoMode->refreshRate);
}

void setWindowed() {
    FULLSCREEN = 0;
    glfwSetWindowMonitor(WINDOW, NULL, POS_X, POS_Y, WINDOWED_WIDTH, WINDOWED_HEIGHT, GLFW_DONT_CARE);
}

void toggleFullscreen() {
    if (FULLSCREEN) {
        setWindowed();
    } else {
        setFullscreen();
    }
}

void onKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
        reloadShaders();
    }
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        toggleFullscreen();
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (pause) {
            pause = 0;
            glfwSetTime(pauseTime);
            prevTime = pauseTime;
        } else {
            pause = 1;
            pauseTime = glfwGetTime();
        }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        glfwSetTime(0);
        pauseTime = 0;
        prevTime = 0;
    }
}

void onMousePosEvent(GLFWwindow *window, double xpos, double ypos) {
    double oldMouseX = mouseX;
    double oldMouseY = mouseY;
    mouseX = xpos;
    mouseY = (double) HEIGHT - ypos;
    double deltaX = mouseX - oldMouseX;
    double deltaY = mouseY - oldMouseY;
    if (deltaX > 100.0 || deltaY > 100.0) {
        return;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        double length = WIDTH > HEIGHT ? HEIGHT : WIDTH;
        offsetX += deltaX / length / zoom;
        offsetY += deltaY / length / zoom;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        double zoomDelta = deltaY / (double) HEIGHT;
        zoom += zoomDelta * zoom * 4.0;
    }
}

void onMouseButtonEvent(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        printf("x = %.54f; y = %.54f\n", offsetX, offsetY);
    }
}

void onFramebufferSizeEvent(GLFWwindow *window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
    if (!FULLSCREEN) {
        WINDOWED_WIDTH = width;
        WINDOWED_HEIGHT = height;
    }
}

void onWindowPosEvent(GLFWwindow *window, int xpos, int ypos) {
    if (!FULLSCREEN) {
        POS_X = xpos;
        POS_Y = ypos;
    }
}

void setupVAO() {
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertices[] = {
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
}

void render() {
    glUniform2f(WINDOW_SIZE_UNIFORM, (float) WIDTH, (float) HEIGHT);
    glUniform1f(TIME_UNIFORM, (float) glfwGetTime());
    glUniform2d(MOUSE_POS_UNIFORM, mouseX, mouseY);
    glUniform2d(OFFSET_UNIFORM, offsetX, offsetY);
    glUniform1d(ZOOM_UNIFORM, zoom);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    sprintf(TITLE, "Zoom: %.2f", zoom);
    glfwSetWindowTitle(WINDOW, TITLE);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    WINDOW = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    glfwGetWindowPos(WINDOW, &POS_X, &POS_Y);
    glfwSetKeyCallback(WINDOW, onKeyEvent);
    glfwSetCursorPosCallback(WINDOW, onMousePosEvent);
    glfwSetMouseButtonCallback(WINDOW, onMouseButtonEvent);
    glfwSetFramebufferSizeCallback(WINDOW, onFramebufferSizeEvent);
    glfwSetWindowPosCallback(WINDOW, onWindowPosEvent);
    glfwMakeContextCurrent(WINDOW);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    setupVAO();
    reloadShaders();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(WINDOW)) {
        glfwPollEvents();
        if (pause) {
            render();
            glfwWaitEvents();
        } else {
            zoom += zoom * (glfwGetTime() - prevTime); // zoom constantly
            prevTime = glfwGetTime();
            render();
        }
        glfwSwapBuffers(WINDOW);
    }

    glfwDestroyWindow(WINDOW);
    glfwTerminate();
    return 0;
}
