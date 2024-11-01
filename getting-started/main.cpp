#include "../common/camera.hpp"
#include "../common/shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/detail/func_trigonometric.inl>
#include <glm/ext/matrix_clip_space.inl>
#include <glm/ext/matrix_transform.inl>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
const unsigned int WINDOW_HEIGHT = 600;
const unsigned int WINDOW_WIDTH = 800;
const auto NEAR_PLANE = .1f;
const auto FAR_PLANE = 100.0f;
const auto VIEW_ANGLE = 45.0f;
const auto MOUSE_SENSITIVITY = 0.1f;
auto deltaTime = .0f;
auto mouseFirstEnter = true;
auto xPosLast = (float)WINDOW_WIDTH / 2;
auto yPosLast = (float)WINDOW_HEIGHT / 2;
Camera camera(glm::vec3(.0f, .0f, 3.0f));
static void CreateTexture(const char*);
static void FramebufferSizeCallback(GLFWwindow*, int, int);
static void MouseCallback(GLFWwindow*, double, double);
static void ProcessInput(GLFWwindow*);
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Getting Started", NULL, NULL);
  if (window == NULL)
    std::cout << "Error: Failed to create GLFW window." << std::endl;
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    std::cout << "Error: Failed to initialize GLAD." << std::endl;
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  Shader shader("vertShader.glsl", "fragShader.glsl");
  CreateTexture("container.jpg");
  //
  //              0-----1
  //              |     |
  //              |     |
  //  1-----0-----4-----5-----1
  //  |     |     |     |     |
  //  |     |     |     |     |
  //  2-----3-----7-----6-----2
  //              |     |
  //              |     |
  //              3-----2
  //
  float vertices[] = {-.5f, -.5f, -.5f, .0f, .0f, .5f, -.5f, -.5f, 1.0f, .0f, .5f, .5f, -.5f, 1.0f, 1.0f, .5f, .5f, -.5f, 1.0f, 1.0f, -.5f, .5f, -.5f, .0f, 1.0f, -.5f, -.5f, -.5f, .0f, .0f, -.5f, -.5f, .5f, .0f, .0f, .5f, -.5f, .5f, 1.0f, .0f, .5f, .5f, .5f, 1.0f, 1.0f, .5f, .5f, .5f, 1.0f, 1.0f, -.5f, .5f, .5f, .0f, 1.0f, -.5f, -.5f, .5f, .0f, .0f, -.5f, .5f, .5f, 1.0f, .0f, -.5f, .5f, -.5f, 1.0f, 1.0f, -.5f, -.5f, -.5f, .0f, 1.0f, -.5f, -.5f, -.5f, .0f, 1.0f, -.5f, -.5f, .5f, .0f, .0f, -.5f, .5f, .5f, 1.0f, .0f, .5f, .5f, .5f, 1.0f, .0f, .5f, .5f, -.5f, 1.0f, 1.0f, .5f, -.5f, -.5f, .0f, 1.0f, .5f, -.5f, -.5f, .0f, 1.0f, .5f, -.5f, .5f, .0f, .0f, .5f, .5f, .5f, 1.0f, .0f, -.5f, -.5f, -.5f, .0f, 1.0f, .5f, -.5f, -.5f, 1.0f, 1.0f, .5f, -.5f, .5f, 1.0f, .0f, .5f, -.5f, .5f, 1.0f, .0f, -.5f, -.5f, .5f, .0f, .0f, -.5f, -.5f, -.5f, .0f, 1.0f, -.5f, .5f, -.5f, .0f, 1.0f, .5f, .5f, -.5f, 1.0f, 1.0f, .5f, .5f, .5f, 1.0f, .0f, .5f, .5f, .5f, 1.0f, .0f, -.5f, .5f, .5f, .0f, .0f, -.5f, .5f, -.5f, .0f, 1.0f};
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  shader.Use();
  auto posAttrib = glGetAttribLocation(shader.ID, "posIn");
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(posAttrib);
  auto texCoordAttrib = glGetAttribLocation(shader.ID, "texCoordIn");
  glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(texCoordAttrib);
  glEnable(GL_DEPTH_TEST);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, MouseCallback);
  auto model = glm::mat4(1.0f);
  auto view = camera.GetViewMatrix();
  auto projection = glm::perspective(glm::radians(VIEW_ANGLE), (float)WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_PLANE, FAR_PLANE);
  shader.SetMat4("model", model);
  shader.SetMat4("view", view);
  shader.SetMat4("projection", projection);
  glClearColor(.2f, .3f, .3f, 1.0f);
  auto timeLast = .0f;
  while (!glfwWindowShouldClose(window)) {
    auto timeNow = glfwGetTime();
    deltaTime = timeNow - timeLast;
    timeLast = timeNow;
    ProcessInput(window);
    view = camera.GetViewMatrix();
    shader.SetMat4("view", view);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader.ID);
  glfwTerminate();
  return 0;
}
static void CreateTexture(const char* filename) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height, nrChannels;
  auto data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else
    std::cout << "Error: Failed to load texture." << std::endl;
  stbi_image_free(data);
}
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
static void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
  if (mouseFirstEnter) {
    xPosLast = xPos;
    yPosLast = yPos;
    mouseFirstEnter = false;
  }
  auto xOffset = xPos - xPosLast;
  auto yOffset = yPosLast - yPos;
  xPosLast = xPos;
  yPosLast = yPos;
  xOffset *= MOUSE_SENSITIVITY;
  yOffset *= MOUSE_SENSITIVITY;
  camera.ProcessMouse(xOffset, yOffset);
}
static void ProcessInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  uint8_t wasd = 0;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    wasd |= 1;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    wasd |= 2;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    wasd |= 4;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    wasd |= 8;
  camera.ProcessKeyboard(wasd, deltaTime);
}
