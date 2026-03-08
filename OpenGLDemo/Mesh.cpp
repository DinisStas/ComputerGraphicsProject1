#include "Mesh.h"

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), diffuseTexture(0) {
}
/*
Mesh::~Mesh() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (diffuseTexture != 0) glDeleteTextures(1, &diffuseTexture);
}*/


void Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    //Clear previous meshes
    vertices.clear();
    indices.clear();

    std::cout << "Processing mesh with " << mesh->mNumVertices << " vertices" << std::endl;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        /*
        //Normals go unused for this version
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default up normal
        }*/

        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::cout << "Processed " << vertices.size() << " vertices and " << indices.size() << " indices" << std::endl;
    
    //Should be only 1 since meshes are proccesed individually
    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;

    //Try to load texture from material
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    diffuseTexture = loadTextureFromMaterial(material, scene);

    //If no texture found, load fallback image/ error texture
    if (diffuseTexture == 0) {
        std::cout << "No texture in model, loading fallback texture..." << std::endl;
        diffuseTexture = loadTextureFromFile("awesomeface.png");
    }

    setupMesh();

    std::cout << "Final texture ID: " << diffuseTexture << std::endl;
    std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;

}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    //Positions corresponding to positions in the shader layout (location = 0) in vec3 position;   layout(location = 1) in vec3 normPosition; layout(location = 2) in vec2 texCoord;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    /*
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, Normal));*/

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    std::cout << "VAO: " << VAO << ", VBO: " << VBO << ", EBO: " << EBO << std::endl;
}
//───────────────────────────────────────────────────────────Getting texture and drawing───────────────────────────────────────────────────────────

GLuint Mesh::loadTextureFromMaterial(aiMaterial* mat, const aiScene* scene) {
    GLuint textureID = 0;

    std::cout << "Checking material for textures" << std::endl;
    std::cout << "Diffuse texture amount: " << mat->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

        aiString texPath;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);

        std::cout << "Texture path: " << texPath.C_Str() << std::endl;

       //Load texture file
       textureID = loadTextureFromFile(texPath.C_Str());
       if (textureID != 0) {
          std::cout << "Loaded external texture: " << texPath.C_Str() << std::endl;
       }
    }
    return textureID;
}

GLuint Mesh::loadTextureFromFile(const char* path) {
    GLuint textureID = 0;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        glGenTextures(1, &textureID);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,  format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Successfully loaded texture from file: " << path << " ID: " << textureID << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << path << " - " << stbi_failure_reason() << std::endl;
    }

    return textureID;
}

void Mesh::draw(Shader& shader) {

    static bool firstDraw = true;

    if (firstDraw) {
        std::cout << "  Mesh draw - VAO: " << VAO << ", Texture: " << diffuseTexture << ", Indices: " << indices.size() << std::endl;

        // Verify texture is valid
        GLboolean isValid = glIsTexture(diffuseTexture);
        std::cout << "  Texture is valid: " << (isValid ? "YES" : "NO") << std::endl;

        firstDraw = false;
    }
    if (diffuseTexture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        shader.setInt("ourTexture", 0);
        //std::cout << "Draw mesh texture" << std::endl;
    }


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //std::cout << "Draw mesh end " << std::endl;
}