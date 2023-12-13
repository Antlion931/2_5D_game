#include "Renderer/Shader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <GL/glew.h>

namespace {
    struct ShaderProgramSource
    {
        std::string VertexSource;
        std::string FragmentSource;
    };

    static uint CompileShader(const std::string& source, uint type)
    {
        uint id = glCreateShader(type);
        const char* src = source.c_str();

        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        // Error handling
        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = static_cast<char*>(alloca(length * sizeof(char)));

            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader:\n";
            std::cout << message << std::endl;

            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    ShaderProgramSource ParseShader(const std::filesystem::path fpath)
    {
        std::ifstream stream(fpath);

        if (!stream)
            throw std::runtime_error("Failed to open shader file");

        enum class ShaderType
        {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;

        while (std::getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;
            }
            else if (type != ShaderType::NONE)
            {
                ss[static_cast<int>(type)] << line << '\n';
            }
        }

        return { ss[0].str(), ss[1].str() };
    }

    uint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
    {
        uint program = glCreateProgram();
        uint vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
        uint fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

        // Link shaders
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);

        // Delete shaders
        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }
}

Shader::Shader(const std::filesystem::path& fpath): m_FilePath(fpath) 
{
    ShaderProgramSource source = ParseShader(fpath);
    m_id = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader() {
    glDeleteProgram(m_id);
}

void Shader::Bind() const {
    glUseProgram(m_id);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

uint Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    uint location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

    m_UniformLocationCache[name] = location;
    return location;
}

template<typename... Args>
void Shader::SetUniform(const std::string& name, Args... args)
{
    const uint location = GetUniformLocation(name);

    if constexpr(std::conjunction_v<std::is_same<Args,bool>...>){    // Handling bools
        if constexpr(sizeof...(args) == 1) glUniform1i(location, args...);
        else if constexpr(sizeof...(args) == 2) glUniform2i(location, args...);
        else if constexpr(sizeof...(args) == 3) glUniform3i(location, args...);
        else if constexpr(sizeof...(args) == 4) glUniform4i(location, args...);
        else static_assert(false, "Invalid number of arguments");
    }

    else if constexpr(std::conjunction_v<std::is_same<Args,int>...>){    // ints
        if constexpr(sizeof...(args) == 1) glUniform1i(location, args...);
        else if constexpr(sizeof...(args) == 2) glUniform2i(location, args...);
        else if constexpr(sizeof...(args) == 3) glUniform3i(location, args...);
        else if constexpr(sizeof...(args) == 4) glUniform4i(location, args...);
        else static_assert(false, "Invalid number of arguments");
    }

    else if constexpr(std::conjunction_v<std::is_same<Args,uint>...>){    // uints
        if constexpr(sizeof...(args) == 1) glUniform1ui(location, args...);
        else if constexpr(sizeof...(args) == 2) glUniform2ui(location, args...);
        else if constexpr(sizeof...(args) == 3) glUniform3ui(location, args...);
        else if constexpr(sizeof...(args) == 4) glUniform4ui(location, args...);
        else static_assert(false, "Invalid number of arguments");
    }

    else if constexpr(std::conjunction_v<std::is_same<Args,float>...>){    // floats
        if constexpr(sizeof...(args) == 1) glUniform1f(location, args...);
        else if constexpr(sizeof...(args) == 2) glUniform2f(location, args...);
        else if constexpr(sizeof...(args) == 3) glUniform3f(location, args...);
        else if constexpr(sizeof...(args) == 4) glUniform4f(location, args...);
        else static_assert(false, "Invalid number of arguments");
    }

    else if constexpr(std::conjunction_v<std::is_same<Args,double>...>){    // doubles
        if constexpr(sizeof...(args) == 1) glUniform1d(location, args...);
        else if constexpr(sizeof...(args) == 2) glUniform2d(location, args...);
        else if constexpr(sizeof...(args) == 3) glUniform3d(location, args...);
        else if constexpr(sizeof...(args) == 4) glUniform4d(location, args...);
        else static_assert(false, "Invalid number of arguments");
    }

    else static_assert(false, "Invalid argument type");
}

#define INSTANTIATE_SET_UNIFORM(type) \
    template void Shader::SetUniform(const std::string& name, type); \
    template void Shader::SetUniform(const std::string& name, type, type); \
    template void Shader::SetUniform(const std::string& name, type, type, type); \
    template void Shader::SetUniform(const std::string& name, type, type, type, type);

INSTANTIATE_SET_UNIFORM(bool)
INSTANTIATE_SET_UNIFORM(int)
INSTANTIATE_SET_UNIFORM(uint)
INSTANTIATE_SET_UNIFORM(float)
INSTANTIATE_SET_UNIFORM(double)

#undef INSTANTIATE_SET_UNIFORM

template<typename Matrix>
void Shader::SetUniformM(const std::string& name, const Matrix& matrix)
{
    const uint location = GetUniformLocation(name);

    if constexpr(std::is_same_v<Matrix, glm::mat2>)
        glUniformMatrix2fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat3>)
        glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat4>)
        glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat2x3>)
        glUniformMatrix2x3fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat2x4>)
        glUniformMatrix2x4fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat3x2>)
        glUniformMatrix3x2fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat3x4>)
        glUniformMatrix3x4fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat4x2>)
        glUniformMatrix4x2fv(location, 1, GL_FALSE, &matrix[0][0]);
    else if constexpr(std::is_same_v<Matrix, glm::mat4x3>)
        glUniformMatrix4x3fv(location, 1, GL_FALSE, &matrix[0][0]);
    else
        static_assert(false, "Invalid matrix type");
}

#define INSTANTIATE_SET_UNIFORM_MATRIX(type) \
    template void Shader::SetUniformM(const std::string& name, const type&);

INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat2)
INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat3)
INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat4)

INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat2x3)
INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat2x4)

INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat3x2)
INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat3x4)

INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat4x2)
INSTANTIATE_SET_UNIFORM_MATRIX(glm::mat4x3)

#undef INSTANTIATE_SET_UNIFORM_MATRIX