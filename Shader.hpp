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
#ifndef MY_GLCHECK
#define MY_GLCHECK
#define glCheckError() glCheckError_(__FILE__, __LINE__)

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#endif

class Shader
{
private:
    GLuint _ShaderID;
public:
    //认为 shader 放在 Shaders 文件夹下 .vert与.frag
    Shader(string shaderName) : Shader("../Shaders/" + shaderName + ".vert", "../Shaders/" + shaderName + ".frag")
    {

    }

    Shader(string vertPath, string fragPath)
    {
        glCheckError();

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
        glCheckError();

    }

    void setUniform(string name, glm::mat4 value)
    {
        glCheckError();
        use();
        glUniformMatrix4fv(glGetUniformLocation(_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        glCheckError();
    }

    void setUniform(const std::string &name, bool value)
    {
        glCheckError();

        use();
        glUniform1i(glGetUniformLocation(_ShaderID, name.c_str()), (int) value);
        glCheckError();

    }

    void setUniform(const std::string &name, int value)
    {
        glCheckError();

        use();
        glUniform1i(glGetUniformLocation(_ShaderID, name.c_str()), value);
        glCheckError();

    }

    void setUniform(const std::string &name, float value)
    {
        glCheckError();

        use();
        glUniform1f(glGetUniformLocation(_ShaderID, name.c_str()), value);
        glCheckError();

    }

    void setUniform(const std::string &name, glm::vec3 value)
    {
        glCheckError();

        use();
        glUniform3f(glGetUniformLocation(_ShaderID, name.c_str()), value.x, value.y, value.z);
        glCheckError();

    }

    void setUniform(const std::string &name, float x, float y, float z)
    {
        glCheckError();

        use();
        glUniform3f(glGetUniformLocation(_ShaderID, name.c_str()), x, y, z);
        glCheckError();

    }

    void setUniformBlock(string name, int index)
    {
        glCheckError();

        use();
        glUniformBlockBinding(_ShaderID, glGetUniformBlockIndex(_ShaderID, name.data()), index);
        glCheckError();

    }


    void use()
    {
        glCheckError();

        glUseProgram(_ShaderID);
        glCheckError();
    }


};