#include <cstdint>
#include <vector>
#include <fstream>
#include <filesystem>

#include <arete/options.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/detail/type_quat.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"

namespace
{

struct State
{
  bool _shouldRun = true;
};

//! Display.
struct Display
{
  //! A GLFW window.
  ::GLFWwindow* _window = nullptr;
};

//! Initializes GLFW display
//!
//! @param display Display.
//! @param width Window width.
//! @param height Window height.
void InitializeGlfw(Display& display, int32_t width, int32_t height)
{
  glfwSetErrorCallback([](int error_code, const char* description){
    printf("[GLFW error] %d : %s\n", error_code, description);
  });

  if (!glfwInit())
    throw std::runtime_error("Couldn't initialize GLFW.");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  display._window = glfwCreateWindow(width, height, "A Game", nullptr, nullptr);
  glfwMakeContextCurrent(display._window);

  const int32_t version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    throw std::runtime_error("Couldn't initialize GLAD.");
  }

  glViewport(0, 0, width, height);

  printf("OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

void TerminateGlfw(Display& display)
{
  glfwTerminate();
}

void ProcessGlfwFrame(Display& display)
{
  glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glfwSwapBuffers(display._window);
  glfwPollEvents();
}

void ProcessInputFrame(Display& display, State& state)
{
  state._shouldRun = !glfwWindowShouldClose(display._window);
}

struct Mesh
{
  uint32_t _vbo { 0 };
  uint32_t _vao { 0 };
  uint32_t _ibo { 0 };

  std::vector<glm::vec3> _vertices;
  std::vector<glm::ivec3> _indices;
};

void CreateRendererMesh(Mesh& mesh)
{
  glGenBuffers(1, &mesh._vbo);
  glGenBuffers(1, &mesh._ibo);

  // Create vertex array and bind it.
  glGenVertexArrays(1, &mesh._vao);
  glBindVertexArray(mesh._vao);

  // Generate and fill the vertex buffer.
  glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    mesh._vertices.size() * sizeof(glm::vec3),
    mesh._vertices.data(),
    GL_STATIC_DRAW);

  // Initialize vertex attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
  glEnableVertexAttribArray(0);

  // Generate and fill the index buffer.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._ibo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      mesh._indices.size() * sizeof(glm::ivec3),
      mesh._indices.data(),
      GL_STATIC_DRAW);

  glBindVertexArray(0);
}


struct Material {
  uint32_t _vertexShaderHandle{ 0 };
  uint32_t _fragmentShaderHandle{ 0 };
  uint32_t _programHandle { 0 };

  std::vector<std::byte> _vertexShaderSource;
  std::vector<std::byte> _fragmentShaderSource;
};

void CreateRendererMaterial(Material& material)
{
  material._vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
  material._fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

  auto createSpirvShader = [](uint32_t& shaderHandle, const std::vector<std::byte>& shaderSource){
    glShaderBinary(
        1,
        &shaderHandle,
        GL_SHADER_BINARY_FORMAT_SPIR_V,
        shaderSource.data(),
        shaderSource.size());
    glSpecializeShaderARB(shaderHandle, "main", 0, nullptr, nullptr);
    GLint isShaderCompiled = false;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isShaderCompiled);
    if (isShaderCompiled == GL_FALSE)
    {
      GLint maxLength = 0;
      glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(shaderHandle, maxLength, &maxLength, &infoLog[0]);
      printf("Shader Create Error: %s\n", infoLog.data());
    }
  };

  createSpirvShader(material._vertexShaderHandle, material._vertexShaderSource);
  createSpirvShader(material._fragmentShaderHandle, material._fragmentShaderSource);

  material._programHandle = glCreateProgram();
  glAttachShader(material._programHandle, material._vertexShaderHandle);
  glAttachShader(material._programHandle, material._fragmentShaderHandle);
  glLinkProgram(material._programHandle);

  GLint isProgramLinked = 0;
  glGetProgramiv(material._programHandle, GL_LINK_STATUS, &isProgramLinked);
  if (isProgramLinked == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(material._programHandle, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(material._programHandle, maxLength, &maxLength, &infoLog[0]);
    printf("Program Link Error: %s\n", infoLog.data());
  }
}

void SetupRender()
{

}

void ProcessRenderFrame()
{

}

} // anonymous namespace

struct ShaderMatrices
{
  glm::mat4 clip;
  glm::mat4 proj;
  glm::mat4 view;
  glm::mat4 model;
} _shaderMatrices;

struct PushConstants
{
  glm::mat4 mvp;
  glm::mat4 model;
//  float time;
} _uniform;

int main(int argc, const char** argv) {
  // Program options.
  struct
  {
    int32_t _width = 1024;
    int32_t _height = 512;
  } options;

  // Parse program options.
  arete::opt::ParseOptions(
    argc,
    argv,
    {
      {"width", [&options](auto& ss)
       {
         ss >> options._width;
       }},
      {"height", [&options](auto& ss)
       {
         ss >> options._height;
       }}
    });

  Display display;
  printf("Display extent: %dx%d\n", options._width, options._height);
  InitializeGlfw(display, options._width, options._height);

  const auto readShaderSource = [](std::filesystem::path path){
    std::ifstream source(path);
    if (source.fail())
      throw std::runtime_error("file not found");

    source.seekg(0, std::ios::end);
    auto size = source.tellg();
    source.seekg(0, std::ios::beg);

    std::vector<std::byte> data;
    data.resize(size);

    source.readsome(reinterpret_cast<char*>(data.data()), size);

    return data;
  };

  uint32_t ubo = 0;
  glGenBuffers(1, &ubo);

  // Setup uniform buffer.
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  // Uniform buffer allocation.
  glBufferData(GL_UNIFORM_BUFFER, sizeof(PushConstants), &_uniform, GL_STATIC_DRAW);
  // Uniform buffer binding.
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  Mesh _cubeMesh;
  _cubeMesh._vertices = {
      {-0.5f, -0.5f, +0.5f},
      {+0.5f, -0.5f, +0.5f},
      {-0.5f, +0.5f, +0.5f},
      {+0.5f, +0.5f, +0.5f},

      // BACK
      {-0.5f, -0.5f, -0.5f},
      {+0.5f, -0.5f, -0.5f},
      {-0.5f, +0.5f, -0.5f},
      {+0.5f, +0.5f, -0.5f}
  };
  _cubeMesh._indices = {
      // FRONT
      // FRONT bottom-left
      {2, 1, 0},
      // FRONT upper-right
      {1, 2, 3},

      // BACK
      {6, 4, 5},
      // BACK
      {5, 7, 6},

      // RIGHT
      {3, 5, 1},
      // RIGHT
      {5, 3, 7},

      // LEFT
      {6, 0, 4},
      // LEFT
      {0, 6, 2},

      // TOP
      {6, 3, 2},
      // TOP
      {3, 6, 7},

      // BOTTOM
      {0, 5, 4},
      // BOTTOM
      {5, 0, 1},
  };
  CreateRendererMesh(_cubeMesh);

  struct {
    glm::vec3 _position{0, 0, 3};
    glm::quat _rotation{1.0f, 0, 0, 0};
    glm::vec3 _scale{1.0f};
  } camera;
  camera._rotation = camera._rotation * glm::angleAxis(glm::pi<float>(), glm::vec3(0, 1, 0));

  Material _cubeMaterial;
  _cubeMaterial._vertexShaderSource = readShaderSource("resources/shaders/cube-vertex.spv");
  _cubeMaterial._fragmentShaderSource = readShaderSource("resources/shaders/cube-fragment.spv");
  CreateRendererMaterial(_cubeMaterial);

  _shaderMatrices.proj = glm::perspective(
      glm::radians<float>(45.0f),
      static_cast<float>(options._width) / static_cast<float>(options._height),
      0.1f,
      100.0f
  );

  _shaderMatrices.model = glm::mat4x4( 1.0f );

  // Selfie / Look from front of the cube at 0,0,0
  _shaderMatrices.view = glm::lookAt(
      camera._position,
      camera._position + glm::vec3(0, 0, 1) * camera._rotation,
      glm::vec3(0, 1, 0) * camera._rotation
  );

  _uniform.model = _shaderMatrices.model;
  _uniform.mvp =
      _shaderMatrices.proj *
      _shaderMatrices.view *
      _shaderMatrices.model;

  State state;
  while(state._shouldRun)
  {
    // Update uniform.
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PushConstants), &_uniform, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Render cube.
    glUseProgram(_cubeMaterial._programHandle);
    glBindVertexArray(_cubeMesh._vao);
    glDrawElements(GL_TRIANGLES, _cubeMesh._indices.size() * 3, GL_INT, 0);
    glBindVertexArray(0);

    ProcessGlfwFrame(display);
    ProcessInputFrame(display, state);
  }

  TerminateGlfw(display);

  return 0;
}
