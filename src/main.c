
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define GL_CALL(x) do { \
    x; \
    GLenum error = glGetError(); \
    while (error != GL_NO_ERROR) { \
        fprintf(stderr, "[OpenGL Error]: (%u) at %s:%d in function %s", error, __FILE__, __LINE__, __FUNCTION__); \
        error = glGetError(); \
        glfwTerminate(); \
        exit(1); \
        } \
} while(0)

#define GL_CALL_RET(ret, x) do { \
    ret = x; \
    GLenum error = glGetError(); \
    while (error != GL_NO_ERROR) { \
        fprintf(stderr, "[OpenGL Error]: (%u) at %s:%d in function %s", error, __FILE__, __LINE__, __FUNCTION__); \
        error = glGetError(); \
        glfwTerminate(); \
        exit(1); \
    } \
} while(0)

#ifdef _WIN32
#include <windows.h>
double get_time() {
    static LARGE_INTEGER frequency, start_time;
    static BOOL initialized = FALSE;
    LARGE_INTEGER now;
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start_time);
        initialized = TRUE;
    }
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - start_time.QuadPart) / frequency.QuadPart;
}
#else
#include <time.h>
double get_time() {
    static struct timespec start_time;
    static int initialized = 0;
    struct timespec now;
    if (!initialized) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        initialized = 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (double)(now.tv_sec - start_time.tv_sec) + (double)(now.tv_nsec - start_time.tv_nsec) / 1e9;
}
#endif

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(x)
#else
#include <unistd.h>
#include <stdarg.h>
#define sleep(x) usleep((x) * 1000)
#endif

void error_callback(const int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s", error, description);
}

void print(const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("[%f]: ", get_time());
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

GLFWwindow *glfw_init() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW");
        return NULL;
    }
    print("GLFW initialized successfully");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window_ptr = glfwCreateWindow(640, 480, "playground", NULL, NULL);
    if (!window_ptr) {
        fprintf(stderr, "Failed to create GLFW window");
        glfwTerminate();
        return NULL;
    }
    print("GLFW window created");

    glfwMakeContextCurrent(window_ptr);
    glfwSwapInterval(-1);
    glfwSwapInterval(1);
    return window_ptr;
}

int glad_init() {
    if (gladLoadGL() == 0) {
        fprintf(stderr, "Failed to initialize GLAD");
        glfwTerminate();
        return 1;
    }
    print("GLAD initialized successfully");
    return 0;
}

char *read_text_file(char *file_path) {
    FILE *file_ptr = fopen(file_path, "r");
    if (!file_ptr) {
        fprintf(stderr, "Error opening file: %s", file_path);
        return NULL;
    }

    fseek(file_ptr, 0L, SEEK_END);
    const int size = (int) ftell(file_ptr);
    char *buffer = malloc(size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation error for file: %s", file_path);
        fclose(file_ptr);
        return NULL;
    }
    rewind(file_ptr);

    fread(buffer, sizeof(char), size, file_ptr);
    buffer[size] = '\0';
    print("Successfully read file: %s", file_path);

    fclose(file_ptr);
    return buffer;
}

int compile_shader(char *path, const int type, GLuint *shader) {
    GLchar *source = read_text_file(path);
    const GLchar *source_array[] = {source};

    GL_CALL_RET(*shader, glCreateShader(type));
    GL_CALL(glShaderSource(*shader, 1, source_array, NULL));
    GL_CALL(glCompileShader(*shader));

    free(source);

    GLint vertex_status;
    GL_CALL(glGetShaderiv(*shader, GL_COMPILE_STATUS, &vertex_status));
    if (vertex_status == 0) {
        fprintf(stderr, "Failed to compile shader: %s", path);
    }

    print("Shader compiled successfully: %s", path);
    return vertex_status == 0;
}

int create_program(char *vertex_path, char *fragment_path, GLuint *program) {
    GLuint vertex_shader, fragment_shader;
    if (compile_shader(vertex_path, GL_VERTEX_SHADER, &vertex_shader) ||
        compile_shader(fragment_path, GL_FRAGMENT_SHADER, &fragment_shader)) {
        return -1;
    }

    GL_CALL_RET(*program, glCreateProgram());
    print("Shader program created with ID: %i", *program);

    GL_CALL(glAttachShader(*program, vertex_shader));
    GL_CALL(glAttachShader(*program, fragment_shader));

    GL_CALL(glLinkProgram(*program));
    glBindFragDataLocation(*program, 0, "outColor");

    GL_CALL(glUseProgram(*program));
    print("Using shader program: %i", *program);

    GL_CALL(glDetachShader(*program, vertex_shader));
    GL_CALL(glDetachShader(*program, fragment_shader));

    GL_CALL(glDeleteShader(vertex_shader));
    GL_CALL(glDeleteShader(fragment_shader));

    GLint position_attribute;
    GL_CALL_RET(position_attribute, glGetAttribLocation(*program, "position"));
    GL_CALL(glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0));
    GL_CALL(glEnableVertexAttribArray(position_attribute));

    GLint tri_color_loc;
    GL_CALL_RET(tri_color_loc, glGetUniformLocation(*program, "triColor"));
    print("tri_color_loc = %i", tri_color_loc);

    glUniform3f(tri_color_loc, 1.0f, 0.0f, 1.0f);

    return 0;
}

int opengl_init(GLFWwindow *window_ptr) {
    int width, height;
    glfwGetFramebufferSize(window_ptr, &width, &height);
    GL_CALL(glViewport(0, 0, width, height));

    GLuint vertex_array_object;
    GL_CALL(glGenVertexArrays(1, &vertex_array_object));
    GL_CALL(glBindVertexArray(vertex_array_object));
    print("Vertex Array Object created with ID: %u", vertex_array_object);

    GLuint vertex_buffer_object;
    GL_CALL(glGenBuffers(1, &vertex_buffer_object));
    print("Vertex Buffer Object created with ID: %u", vertex_buffer_object);

    const float vertices[] = {
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    };

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLuint program;
    if (create_program("shaders/vertex.glsl", "shaders/fragment.glsl", &program)) {
        fprintf(stderr, "Error creating shader program");
        glfwTerminate();
        return 1;
    }

    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    return 0;
}

void draw();

int main() {
    print("### Setup ###");
    GLFWwindow *window_ptr = glfw_init();
    if (!window_ptr || glad_init()) {
        glfwTerminate();
        return 1;
    }

    if (opengl_init(window_ptr)) {
        glfwTerminate();
        return 1;
    }

    print("### Main Loop ###");

    const double target_framerate = 240.0f;
    const double target_delta = 1.0f / target_framerate;
    int frame_count = 0;

    print("Initial time: %f", glfwGetTime());

    while (!glfwWindowShouldClose(window_ptr)) {
        const double current_time = get_time();
        frame_count++;

        if (frame_count % (int)target_framerate == 0) {
            print("current time: %f | frame: %i", current_time, frame_count);
        }

        draw();

        glfwSwapBuffers(window_ptr);
        glfwPollEvents();

        while (get_time() - current_time < target_delta);
        //while (get_time() - current_time < target_delta) {
        //    while (get_time() - current_time < target_delta);
        //}
    }

    print("### Exiting Application ###");

    glfwTerminate();
    return 0;
}

void draw() {
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
}