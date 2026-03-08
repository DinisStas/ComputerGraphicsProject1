#pragma once
#include <vector>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Responsible for importing the model and seperating it into meshes so they can render with correctly with materials
class Model {
private:
    Assimp::Importer importer;

    //Vector containing all of the meshes from the file, (since a file can contain multiple models and materials inside of it itself)
    std::vector<Mesh> meshes;

    //Adds all of the meshes inside the file containing the model to the meshes list and process the meshes inside the mesh class
    void processModel(aiNode* node, const aiScene* scene);

public:
    //Load each mesh contained in the file
    void LoadModel(const std::string& filename);

    //Draw each mesh
    void draw(Shader& shader);


    //───────────────────────────────────────────────────────────Variables for transformations───────────────────────────────────────────────────────────
    glm::mat4 transform = glm::mat4(1.0f);

    //Allows manipulation of the models
    void setPosition(glm::vec3 pos) { transform = glm::translate(glm::mat4(1.0f), pos); }
    void setScale(glm::vec3 scale) { transform = glm::scale(transform, scale); }
    void setRotation(float degrees, glm::vec3 axis) { transform = glm::rotate(transform, glm::radians(degrees), axis); }
};
