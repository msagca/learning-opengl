#include <component_types.hpp>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera_controller.hpp>
#include <utility.hpp>
#include <imgui_impl_opengl3.h>
#include <mesh.hpp>
#include <imgui_impl_glfw.h>
#include <entity_manager.hpp>
#include <system.hpp>
#include <iostream>
#include <gui.hpp>
#include <input_manager.hpp>
static const auto WINDOW_HEIGHT = 600u;
static const auto WINDOW_WIDTH = 800u;
static const auto CAMERA_POSITION = glm::vec3(.0f, .0f, 3.0f);
static const auto CLEAR_COLOR = glm::vec4(.1f, .1f, .1f, 1.0f);
static const auto INACTIVITY_TIMEOUT = 3.0f;
double deltaTime = .0;
bool showCreateMenu = false;
bool showHierarchyWindow = true;
static CameraController* cameraControllerPtr;
static GLFWwindow* InitializeGLFW();
static void InitializeImGui(GLFWwindow*);
static void WindowCloseCallback(GLFWwindow*);
static void FramebufferSizeCallback(GLFWwindow*, int, int);
int main() {
  auto window = InitializeGLFW();
  if (!window)
    return 1;
  auto& inputManager = InputManager::GetInstance();
  inputManager.Initialize(window);
  InitializeImGui(window);
  // OpenGL configuration
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  // initialize ECS
  EntityManager entityManager;
  RenderSystem renderSystem(entityManager);
  // populate the scene
  auto cameraID = entityManager.CreateEntity("MainCamera");
  auto& camera = entityManager.AddComponent<Camera>(cameraID);
  auto cameraController = CameraController(camera, inputManager);
  cameraController.SetPosition(CAMERA_POSITION);
  cameraControllerPtr = &cameraController;
  renderSystem.SetCamera(&camera);
  auto lightID = entityManager.CreateEntity("MainLight");
  entityManager.AddComponent<Light>(lightID);
  lightID = entityManager.CreateEntity("PointLight");
  auto& light = entityManager.AddComponent<Light>(lightID);
  light.type = LightType::Point;
  light.vector = glm::vec3(-.2f, 1.0f, -.3f);
  glfwMaximizeWindow(window);
  // register callbacks
  inputManager.RegisterCallback(GLFW_KEY_H, GLFW_PRESS, []() { showHierarchyWindow = !showHierarchyWindow; });
  inputManager.RegisterCallback(GLFW_KEY_SPACE, GLFW_PRESS, []() { showCreateMenu = !showCreateMenu; });
  inputManager.RegisterCallback(GLFW_KEY_V, GLFW_PRESS, [&renderSystem]() { renderSystem.ToggleWireframeMode(); });
  inputManager.RegisterCallback(GLFW_MOUSE_BUTTON_2, GLFW_PRESS, [&window]() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); });
  inputManager.RegisterCallback(GLFW_MOUSE_BUTTON_2, GLFW_RELEASE, [&window]() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); });
  auto timeLast = .0;
  while (!glfwWindowShouldClose(window)) {
    auto timeNow = glfwGetTime();
    deltaTime = timeNow - timeLast;
    timeLast = timeNow;
    glClearColor(CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cameraController.Update();
    renderSystem.Update();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (showHierarchyWindow)
      ShowHierarchyWindow(entityManager, inputManager);
    if (showCreateMenu)
      ShowCreateMenu(entityManager);
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    if (inputManager.GetInactivityTime() > INACTIVITY_TIMEOUT)
      ShowHints(windowWidth, windowHeight);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  entityManager.CleanUp();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
static GLFWwindow* InitializeGLFW() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
  if (!window)
    std::cerr << "Error: Failed to create GLFW window." << std::endl;
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    std::cerr << "Error: Failed to initialize GLAD." << std::endl;
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetWindowCloseCallback(window, WindowCloseCallback);
  SetWindowIcon(window, "logo.png");
  return window;
}
static void InitializeImGui(GLFWwindow* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto& imguiIO = ImGui::GetIO();
  imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
}
static void WindowCloseCallback(GLFWwindow* window) {
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  auto ratio = static_cast<float>(width) / height;
  if (cameraControllerPtr)
    cameraControllerPtr->SetAspect(ratio);
}
