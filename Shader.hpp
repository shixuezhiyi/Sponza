#pragma once


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Shader
{
private:
    GLuint _ShaderID;
    bool _isUsed;
public:
    //认为 shader 放在 Shaders 文件夹下 .vert与.frag
    Shader(string shaderName)
    {
        string vertPath = "../Shaders/" + shaderName + ".vert";
        string fragPath = "../Shaders/" + shaderName + ".frag";
        new(this)Shader(vertPath, fragPath);
    }

    Shader(string vertPath, string fragPath)
    {
        string vertexCode, fragmentCode;
        ifstream vShaderFile, fShaderFile;
        int success;
        char infoLog[512];
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShaderFile.open(vertPath);
            stringstream vShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();
            vertexCode = vShaderStream.str();
        } catch (std::ifstream::failure &e)
        {
            std::cerr << "Can not find: " << vertPath << endl;
        }
        try
        {
            fShaderFile.open(fragPath);
            stringstream fShaderStream;
            fShaderStream << fShaderFile.rdbuf();
            fShaderFile.close();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cerr << "Can not find: " << fragPath << endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << vertPath << "  :存在错误\n";
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << fragPath << "  :存在错误\n";
            std::cerr << infoLog << std::endl;
        }
        _ShaderID = glCreateProgram();
        glAttachShader(_ShaderID, vertexShader);
        glAttachShader(_ShaderID, fragmentShader);
        glLinkProgram(_ShaderID);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        _isUsed = false;
    }

    void setUniform(string name, glm::mat4 value)
    {
        if (!_isUsed)
            use();
        glUniformMatrix4fv(glGetUniformLocation(_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(const std::string &name, bool value)
    {
        if (!_isUsed)
            use();
        glUniform1i(glGetUniformLocation(_ShaderID, name.c_str()), (int) value);
    }

    void setUniform(const std::string &name, int value)
    {
        if (!_isUsed)
            use();
        glUniform1i(glGetUniformLocation(_ShaderID, name.c_str()), value);
    }

    void setUniform(const std::string &name, float value)
    {
        if (!_isUsed)
            use();
        glUniform1f(glGetUniformLocation(_ShaderID, name.c_str()), value);
    }
    void setUniform(const std::string &name, glm::vec3 value)
    {
        if (!_isUsed)
            use();
        glUniform3f(glGetUniformLocation(_ShaderID, name.c_str()), value.x, value.y, value.z);
    }

    void setUniform(const std::string &name, float x, float y, float z)
    {
        if (!_isUsed)
            use();
        glUniform3f(glGetUniformLocation(_ShaderID, name.c_str()), x, y, z);
    }
    void setUniformBlock(string name, int index)
    {
        if (!_isUsed)
            use();
        glUniformBlockBinding(_ShaderID, glGetUniformBlockIndex(_ShaderID, name.data()), index);
    }

    void use()
    {
        _isUsed = true;
        glUseProgram(_ShaderID);
    }


};