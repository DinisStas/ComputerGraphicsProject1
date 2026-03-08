#pragma once
#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh {
public:
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    GLuint diffuseTexture;

    //Generates vao, vbo, ebo and binds vertices
    void setupMesh();
    GLuint loadTextureFromMaterial(aiMaterial* material, const aiScene* scene);
    GLuint loadTextureFromFile(const char* path);

public:
    Mesh();
    //Cleaner causes issues with textures so not active
    //~Mesh();

    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void draw(Shader& shader);

    GLuint getVAO() const { return VAO; }
    size_t getIndexCount() const { return indices.size(); }
};