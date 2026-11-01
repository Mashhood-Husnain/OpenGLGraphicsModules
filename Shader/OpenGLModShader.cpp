#include "OpenGLModShader.hpp"

OpenGLModShader::OpenGLModShader(const std::string &vs_path, const std::string &fs_path)
{
    std::string vs_src, fs_src;
    if (!readFile(vs_path, vs_src) || !readFile(fs_path, fs_src)) return;

    unsigned int vs = compile(GL_VERTEX_SHADER, vs_src, "VERTEX");
    if (!vs) return;

    unsigned int fs = compile(GL_FRAGMENT_SHADER, fs_src, "FRAGMENT");
    if (!fs)
    {
        glDeleteShader(vs);
        return;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    if(!checkError(program, "program", "SHADERPROGRAM"))
    {
        glDeleteProgram(program);
        return;
    }

    shader_program = program;
}

OpenGLModShader OpenGLModShader::fromSource(const std::string &vs_src, const std::string &fs_src)
{
    OpenGLModShader shader;
    shader.build(vs_src, fs_src);
    return shader;
}

bool OpenGLModShader::build(const std::string &vs_src, const std::string &fs_src)
{
    unsigned int vs = compile(GL_VERTEX_SHADER, vs_src, "VERTEX");
    if (!vs) return false;

    unsigned int fs = compile(GL_FRAGMENT_SHADER, fs_src, "FRAGMENT");
    if (!fs)
    {
        glDeleteShader(vs);
        return false;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    if (!checkError(program, "program", "SHADERPROGRAM"))
    {
        glDeleteProgram(program);
        return false;
    }

    shader_program = program;
    return true;
}

OpenGLModShader::~OpenGLModShader()
{
    if (shader_program) glDeleteProgram(shader_program);
}

OpenGLModShader::OpenGLModShader(OpenGLModShader&& other) noexcept :
    shader_program(std::exchange(other.shader_program, 0)),
    uniform_cache(std::move(other.uniform_cache)) {}

OpenGLModShader &OpenGLModShader::operator=(OpenGLModShader &&other) noexcept
{
    if (this != &other)
    {
        if (shader_program) glDeleteProgram(shader_program);
        shader_program = std::exchange(other.shader_program, 0);
        uniform_cache = std::move(other.uniform_cache);
    }

    return *this;
}

bool OpenGLModShader::checkError(unsigned int shader, std::string error_type, std::string type)
{
    int ok;
    char log[512];

    if (error_type == "shader")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            glGetShaderInfoLog(shader, 512, NULL, log);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << log << std::endl;
            return false;
        }
        return true;
    }

    glGetProgramiv(shader, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        glGetProgramInfoLog(shader, 512, NULL, log);
        std::cerr << "ERROR::SHADER::" << type << "::LINK_FAILED\n" << log << std::endl;
        return false;
    }
    return true;
}

bool OpenGLModShader::readFile(const std::string &path, std::string &out)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error opening: " << path << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();

    return true;
}

unsigned int OpenGLModShader::compile(unsigned int type, const std::string &source, std::string label)
{
    unsigned int shader = glCreateShader(type);
    const char *code = source.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    if (!checkError(shader, "shader", label))
    {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int OpenGLModShader::location(const std::string &name)
{
    auto it = uniform_cache.find(name);
    if (it != uniform_cache.end()) return it->second;

    int loc = glGetUniformLocation(shader_program, name.c_str());
    if (loc == -1)
    {
        std::cerr << "Error: uniform " << name << " not found!" << std::endl;
    }

    uniform_cache[name] = loc;
    return loc;
}

bool OpenGLModShader::isValid() const { return shader_program != 0; }
unsigned int OpenGLModShader::id() const { return shader_program; }
void OpenGLModShader::use() const { glUseProgram(shader_program); }

void OpenGLModShader::setBool(const std::string &name, bool value) { glUniform1i(location(name), (int)value); }
void OpenGLModShader::setInt(const std::string &name, int value) { glUniform1i(location(name), value); }
void OpenGLModShader::setFloat(const std::string &name, float value) { glUniform1f(location(name), value); }
void OpenGLModShader::setVec2(const std::string &name, const glm::vec2 &v) { glUniform2fv(location(name), 1, glm::value_ptr(v)); }
void OpenGLModShader::setVec3(const std::string &name, const glm::vec3 &v) { glUniform3fv(location(name), 1, glm::value_ptr(v)); }
void OpenGLModShader::setVec4(const std::string &name, const glm::vec4 &v) { glUniform4fv(location(name), 1, glm::value_ptr(v)); }
void OpenGLModShader::setMat3(const std::string &name, const glm::mat3 &m) { glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(m)); }
void OpenGLModShader::setMat4(const std::string &name, const glm::mat4 &m) { glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(m)); }