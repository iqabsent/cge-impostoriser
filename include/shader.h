class ShaderProg {
  
  GLuint program_;
  GLuint modelToProjection_index;
  GLuint diffuse_color_index;
  GLuint light_direction_index;

public:
  ShaderProg() {}

  void init() {

    char * vertex_shader_code =
      "varying vec4 color_;"
      "attribute vec4 pos;"
      "attribute vec3 normal;"
      "uniform vec4 diffuse_color;"
      "uniform vec3 light_direction;"
      "uniform mat4 modelToProjection;"
      ""
      "void main() {"
      "  float light_factor = dot(light_direction, normal);"
      "  color_ = max(light_factor, 0.2) * diffuse_color;"
      "  gl_Position = modelToProjection * pos;"
      "}";

    char * fragment_shader_code =
      "varying vec4 color_;"
      "void main() {gl_FragColor = color_;}";

    GLuint vertex_shader = makeShader(GL_VERTEX_SHADER, vertex_shader_code);
    GLuint fragment_shader = makeShader(GL_FRAGMENT_SHADER, fragment_shader_code);
    
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);

    glBindAttribLocation(program_, 0, "pos");
    glBindAttribLocation(program_, 1, "normal");

    glLinkProgram(program_);

    modelToProjection_index = glGetUniformLocation(program_, "modelToProjection");
    diffuse_color_index = glGetUniformLocation(program_, "diffuse_color");
    light_direction_index = glGetUniformLocation(program_, "light_direction");

  }

  void setSomeStuff(const vec4 &diffuse_color, const vec4 &light_direction) {
    
    glUseProgram(program_);

    glUniform4fv(diffuse_color_index, 1, diffuse_color.get());
    glUniform3fv(light_direction_index, 1, light_direction.get());
  }

  void setMatrixStuff(const mat4 &modelToProjection) {
    
    glUseProgram(program_);
    
    glUniformMatrix4fv(modelToProjection_index, 1, GL_FALSE, modelToProjection.get());
  }

  GLuint makeShader (int type, char * code) {
    GLsizei length;
    GLchar buf[256];
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char **)&code, NULL);
    glCompileShader(shader);
    glGetShaderInfoLog(shader, sizeof(buf), &length, buf);
    puts(buf);

    modelToProjection_index = glGetUniformLocation(program_, "modelToProjection");
    diffuse_color_index = glGetUniformLocation(program_, "diffuse_color");
    light_direction_index = glGetUniformLocation(program_, "light_direction");

    return shader;
  }

  GLuint program() {
    return program_;
  }

};