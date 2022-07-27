#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <vector>
#include "Shader.hpp"

using namespace std;

struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;

    Vertex(glm::vec3 P, glm::vec3 N, glm::vec2 texCoords, glm::vec3 T, glm::vec3 B) :
            Position(P), Normal(N), TexCoords(texCoords), Tangent(T), Bitangent(B)
    {}
};

struct Texture
{
    unsigned int id;
    string type;
    aiString path;

    Texture()
    {}

};

class Mesh
{
private:
    vector<Vertex> _vertices;
    vector<unsigned int> _indices;
//    vector<Texture> _textures;
    unsigned int VAO;
public:
    Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices) : _vertices(vertices), _indices(indices)
    {
        setup();
    }

    void draw(Shader &shader)
    {
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    //创建 VAO
    void setup()
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));
    }
};

class Model
{
private:
//    vector<Texture> textures_loaded;
    vector<Mesh> _meshes;
public:
    Model(string path)
    {
        loadModel("../Resources/" + path);
    }

    void draw(Shader &shader)
    {
        for (auto &mesh: _meshes)
            mesh.draw(shader);
    }

private:
    void loadModel(string path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            _meshes.push_back(processMesh(mesh));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 pos{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            glm::vec3 normal{0.0f};
            glm::vec3 tangent{0.0f};
            glm::vec3 bitangents{0.0f};
            glm::vec2 texCoords{0.0f};
            if (mesh->HasNormals())
            {
                normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            }
            if (mesh->HasTextureCoords(i))
                texCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            if (mesh->HasTangentsAndBitangents())
            {
                tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                bitangents = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
            }
            vertices.emplace_back(pos, normal, texCoords, tangent, bitangents);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        return Mesh(vertices, indices);
    }
};
