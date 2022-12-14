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
    GLuint shaderID_;
public:
    //认为 shader 放在 Shaders 文件夹下 .vert与.frag
    Shader(const string &shaderName) : Shader("../Shaders/" + shaderName + ".vert",
                                              "../Shaders/" + shaderName + ".frag")
    {}

    Shader(string vertPath, string fragPath, string geomPath = "")
    {
        shaderID_ = glCreateProgram();
        string geometryCode;
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
        if (!geomPath.empty())
        {
            ifstream gShaderFile;
            gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
                gShaderFile.open(geomPath);
                stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
            catch (std::ifstream::failure &e)
            {
                std::cerr << "Can not find: " << geomPath << endl;
            }
            unsigned int geometryShader;
            const char *gShaderCode = geometryCode.c_str();
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &gShaderCode, NULL);
            glCompileShader(geometryShader);
            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
                std::cerr << geomPath << "  :存在错误\n";
                std::cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            glAttachShader(shaderID_, geometryShader);
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
        }
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
        glAttachShader(shaderID_, vertexShader);
        glAttachShader(shaderID_, fragmentShader);
        glLinkProgram(shaderID_);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glCheckError();
    }

    void setUniform(const std::string &name, const glm::mat4 &value) const
    {
        use();
        glUniformMatrix4fv(glGetUniformLocation(shaderID_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(const std::string &name, const bool &value) const
    {
        use();
        glUniform1i(glGetUniformLocation(shaderID_, name.c_str()), (int) value);
    }

    void setUniform(const std::string &name, const int &value) const
    {
        use();
        glUniform1i(glGetUniformLocation(shaderID_, name.c_str()), value);
    }

    void setUniform(const std::string &name, const float &value) const
    {
        use();
        glUniform1f(glGetUniformLocation(shaderID_, name.c_str()), value);
    }


    void setUniform(const std::string &name, const glm::vec3 &value) const
    {
        use();
        glUniform3f(glGetUniformLocation(shaderID_, name.c_str()), value.x, value.y, value.z);
    }

    void setUniform(const std::string &name, const glm::vec2 &value) const
    {
        use();
        glUniform2f(glGetUniformLocation(shaderID_, name.c_str()), value.x, value.y);
    }


    void setUniform(const std::string &name, const float &x, const float &y, const float &z) const
    {
        use();
        glUniform3f(glGetUniformLocation(shaderID_, name.c_str()), x, y, z);
    }


    void setUniformBlock(const string &name, const int &index) const
    {
        use();
        glUniformBlockBinding(shaderID_, glGetUniformBlockIndex(shaderID_, name.data()), index);
    }

    void setUniform(const std::string &names, const vector<glm::mat4> &value) const
    {
        use();
        for (int i = 0; i < value.size(); i++)
        {
            auto name = names + "[" + to_string(i) + "]";
            setUniform(name, value[i]);
        }
    }


    void use() const
    {
        glUseProgram(shaderID_);
    }
};