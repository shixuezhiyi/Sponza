#pragma once

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include <tiny_gltf.h>
#include <stb_image.h>
#include <vector>
#include "Shader.hpp"

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

struct MyMaterial
{
//    bool isDoubleSized;
    bool hasNormal;
    bool hasBaseColor;
    bool hasMetallicRoughness;
    unsigned int NormalTextID;
    unsigned int BaseColorID;
    unsigned int MetallicRoughnessTextureID;


    void bind(Shader &shader)
    {
//        glCheckError();
////        shader.setUniform("isDoubleSized",isDoubleSized);
        shader.setUniform("hasNormal", hasNormal);
        shader.setUniform("hasBaseColor", hasBaseColor);
        shader.setUniform("hasMetallicRoughness", hasMetallicRoughness);
        glActiveTexture(GL_TEXTURE0);
        shader.setUniform("BaseColorTex", 0);
        glBindTexture(GL_TEXTURE_2D, BaseColorID);
        glActiveTexture(GL_TEXTURE1);
        shader.setUniform("NormalTex", 1);
        glBindTexture(GL_TEXTURE_2D, NormalTextID);
        glActiveTexture(GL_TEXTURE2);
        shader.setUniform("MetallicRoughnessTex", 2);
        glBindTexture(GL_TEXTURE_2D, MetallicRoughnessTextureID);
        glCheckError();
    }
};

struct MyPrimitive
{
    unsigned int VAO;
    MyMaterial material;
    int mode;
    unsigned long count;
    int componentType;
    unsigned long offset;


    MyPrimitive(const tinygltf::Model &model, const int meshIndex, const int pIdx, const vector<unsigned int> &VBOs,
                const vector<unsigned int> &TextureIDs,
                const unsigned int defaultTexture)
    {
        glCheckError();
        auto &primitive = model.meshes[meshIndex].primitives[pIdx];
        mode = primitive.mode;
        vector<pair<string, int>> preDefinedAttributes =
                {
                        {"POSITION",   0},
                        {"NORMAL",     1},
                        {"TEXCOORD_0", 2},
                        {"TANGENT",    3}
                };
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        for (auto &preDefinedAttribute: preDefinedAttributes)
        {
            auto attributeName = preDefinedAttribute.first;
            auto attributeLocation = preDefinedAttribute.second;
            auto it = primitive.attributes.find(attributeName);
            if (it == primitive.attributes.end())
                continue;
            auto &accessor = model.accessors[it->second];
            auto &bufferView = model.bufferViews[accessor.bufferView];
            auto bufferIdx = bufferView.buffer;

            glBindBuffer(GL_ARRAY_BUFFER, VBOs[bufferIdx]);
            auto byteOffset = accessor.byteOffset + bufferView.byteOffset;
            glEnableVertexAttribArray(attributeLocation);
            glVertexAttribPointer(attributeLocation, accessor.type, accessor.componentType, GL_FALSE,
                                  bufferView.byteStride,
                                  (void *) byteOffset);
        }
        if (primitive.indices >= 0)
        {
            const auto &indicesAccessor = model.accessors[primitive.indices];
            const auto &indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
            const auto bufferIndex = indicesBufferView.buffer;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[bufferIndex]);
            count = indicesAccessor.count;
            componentType = indicesAccessor.componentType;
            offset = indicesAccessor.byteOffset + indicesBufferView.byteOffset;
        } else
            cerr << "No indices" << endl;


        auto materialIdx = primitive.material;
        auto curMaterial = model.materials[materialIdx];
        auto baseColorIdx = curMaterial.pbrMetallicRoughness.baseColorTexture.index;
        auto normalTextIdx = curMaterial.normalTexture.index;
        auto mrIdx = curMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
        if (baseColorIdx >= 0)
        {
            material.BaseColorID = TextureIDs[baseColorIdx];
            material.hasBaseColor = true;
        } else
        {
            material.BaseColorID = defaultTexture;
            material.hasBaseColor = false;
        }
        if (normalTextIdx >= 0)
        {
            material.NormalTextID = TextureIDs[normalTextIdx];
            material.hasNormal = true;
        } else
        {
            material.NormalTextID = defaultTexture;
            material.hasNormal = false;
        }
        if (mrIdx >= 0)
        {
            material.MetallicRoughnessTextureID = TextureIDs[mrIdx];
            material.hasMetallicRoughness = true;
        } else
        {
            material.MetallicRoughnessTextureID = defaultTexture;
            material.hasMetallicRoughness = false;
        }
        glCheckError();
    }

    void draw(Shader &shader)
    {
        glCheckError();
        material.bind(shader);
        glBindVertexArray(VAO);
        shader.use();
        if (offset >= 0)
            glDrawElements(mode, count, componentType, (void *) offset);
        else
            cerr << "Can't draw" << endl;
        glCheckError();
    }
};


class MyMesh
{
private:
    vector<MyPrimitive> primitives;
    glm::mat4 originModelMat;

public:
    MyMesh()
    {}

    MyMesh(const tinygltf::Model &model, const int meshIndex, const vector<unsigned int> &VBOs,
           const vector<unsigned int> &TextureIDs, const unsigned int defaultTexture)
    {
        glCheckError();
        for (int i = 0; i < model.meshes[meshIndex].primitives.size(); i++)
        {
            primitives.emplace_back(MyPrimitive(model, meshIndex, i, VBOs, TextureIDs, defaultTexture));
        }
        glCheckError();
    }

    void setModelMat(const glm::mat4 &m)
    {
        originModelMat = m;
    }

    void draw(Shader &shader)
    {
        glCheckError();
        shader.setUniform("originModelMat", originModelMat);
        for (auto &primitive: primitives)
            primitive.draw(shader);
        glCheckError();
    }
};

class MyModel
{
private:
//    vector<Texture> textures_loaded;
    vector<unsigned int> VBOs;
    vector<unsigned int> TextureIDs;
    unsigned int whiteTexture;
    vector<MyMesh> meshes;
public:
    MyModel(string path)
    {
        loadModel("../Resources/" + path);
        glCheckError();
    }

    void draw(Shader &shader)
    {
        glCheckError();
        for (auto &mesh: meshes)
            mesh.draw(shader);
        glCheckError();
    }

private:
    void loadModel(string path)
    {
        whiteTexture = myTextureFromFile("../Resources/white.png");

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        string err;
        string warn;
        bool ret;
        auto fileExtension = path.substr(path.rfind('.'));
        if (fileExtension == ".glb")
            ret = loader.LoadBinaryFromFile(&model, &err, &warn, path); // for binary Box(.glb)
        else if (fileExtension == ".gltf")
            ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        else
        {
            cerr << "not glb and gltf" << endl;
            return;
        }
        if (!warn.empty())
        {
            printf("Warn: %s\n", warn.c_str());
        }

        if (!err.empty())
        {
            printf("Err: %s\n", err.c_str());
        }

        if (!ret)
        {
            cout << "Failed to parse Box:  " << path << endl;
            return;
        }
        buildBuffer(model);
        buildTexture(model);
        buildScene(model);
    }

    void buildBuffer(const tinygltf::Model &model)
    {
        for (auto i = 0; i < model.buffers.size(); i++)
        {
            unsigned int VBO;
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, model.buffers[i].data.size(), model.buffers[i].data.data(), GL_STATIC_DRAW);
            VBOs.push_back(VBO);
        }
    }

    void buildTexture(const tinygltf::Model &model)
    {
        for (auto i = 0; i < model.textures.size(); i++)
        {
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            auto texture = model.textures[i];
            auto minFilter =
                    texture.sampler >= 0 && model.samplers[texture.sampler].minFilter != -1
                    ? model.samplers[texture.sampler].minFilter
                    : GL_LINEAR;
            auto magFilter =
                    texture.sampler >= 0 && model.samplers[texture.sampler].magFilter != -1
                    ? model.samplers[texture.sampler].magFilter
                    : GL_LINEAR;
            auto wrapS = texture.sampler >= 0 ? model.samplers[texture.sampler].wrapS
                                              : GL_REPEAT;
            auto wrapT = texture.sampler >= 0 ? model.samplers[texture.sampler].wrapT
                                              : GL_REPEAT;
            const auto& image = model.images[texture.source];
            //TODO:使用 GL_RGB就会有 BUG,我也不知道为啥
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, image.pixel_type,
                         image.image.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
//
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (minFilter == GL_NEAREST_MIPMAP_NEAREST ||
                minFilter == GL_NEAREST_MIPMAP_LINEAR ||
                minFilter == GL_LINEAR_MIPMAP_NEAREST ||
                minFilter == GL_LINEAR_MIPMAP_LINEAR)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            TextureIDs.push_back(textureID);
        }
    }

    unsigned int myTextureFromFile(const char *path, bool gamma = false)
    {
        stbi_set_flip_vertically_on_load(true);
        unsigned int textureID;
        glGenTextures(1, &textureID);
        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    void buildScene(const tinygltf::Model &model)
    {
        for (auto sceneIdx = 0; sceneIdx < model.scenes.size(); sceneIdx++)
        {
            for (int nodeIdx = 0; nodeIdx < model.scenes[sceneIdx].nodes.size(); nodeIdx++)
            {
                buildNode(model, model.scenes[sceneIdx].nodes[nodeIdx]);
            }
        }
    }

    void buildNode(const tinygltf::Model &model, const int nodeIndex)
    {
        for (auto &childNodeIndex: model.nodes[nodeIndex].children)
        {
            buildNode(model, childNodeIndex);
        }


        auto nodeMatrix = model.nodes[nodeIndex].matrix;
        glm::mat4 matrix(1.0f);
        if (nodeMatrix.size() == 16)
        {
            matrix[0].x = nodeMatrix[0], matrix[0].y = nodeMatrix[1],
            matrix[0].z = nodeMatrix[2], matrix[0].w = nodeMatrix[3];
            matrix[1].x = nodeMatrix[4], matrix[1].y = nodeMatrix[5],
            matrix[1].z = nodeMatrix[6], matrix[1].w = nodeMatrix[7];
            matrix[2].x = nodeMatrix[8], matrix[2].y = nodeMatrix[9],
            matrix[2].z = nodeMatrix[10], matrix[2].w = nodeMatrix[11];
            matrix[3].x = nodeMatrix[12], matrix[3].y = nodeMatrix[13],
            matrix[3].z = nodeMatrix[14], matrix[3].w = nodeMatrix[15];
        } else
        {
            if (model.nodes[nodeIndex].translation.size() == 3)
            {
                glm::translate(matrix, glm::vec3(model.nodes[nodeIndex].translation[0],
                                                 model.nodes[nodeIndex].translation[1],
                                                 model.nodes[nodeIndex].translation[2]));
            }
            if (model.nodes[nodeIndex].rotation.size() == 4)
            {
                matrix *= glm::mat4_cast(glm::quat(model.nodes[nodeIndex].rotation[3],
                                                   model.nodes[nodeIndex].rotation[0],
                                                   model.nodes[nodeIndex].rotation[1],
                                                   model.nodes[nodeIndex].rotation[2]));
            }
            if (model.nodes[nodeIndex].scale.size() == 3)
            {
                glm::scale(matrix, glm::vec3(model.nodes[nodeIndex].scale[0],
                                             model.nodes[nodeIndex].scale[1],
                                             model.nodes[nodeIndex].scale[2]));
            }
        }
        if (model.nodes[nodeIndex].mesh >= 0)
        {
            auto mesh = MyMesh(model, model.nodes[nodeIndex].mesh, VBOs, TextureIDs, whiteTexture);
            mesh.setModelMat(matrix);
            meshes.push_back(mesh);
        }
    }
};
