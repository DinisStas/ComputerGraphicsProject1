#include "Model.h"


void Model::LoadModel(const std::string& filename) {

    // Load the FBX file
    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_Triangulate |           // Convert polygons to triangles
        aiProcess_FlipUVs |                // Flip UV coordinates
        aiProcess_CalcTangentSpace |       // Calculate tangent space
        aiProcess_GenNormals);             // Generate normals if missing

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
       // std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "Mesh Loaded: " << filename << std::endl;
    std::cout << "Number meshes in file: " << scene->mNumMeshes << std::endl;

    //Process all meshes in the scene
    processModel(scene->mRootNode, scene);

    //std::cout << "Loaded " << meshes.size() << " meshes" << std::endl;
}

void Model::processModel(aiNode* node, const aiScene* scene) {

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {

        //std::cout << "Loaded model " << std::endl;
        //Get the mesh
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        //Create mesh, processMesh as Mesh class and add it to the mesh List
        Mesh newMesh;
        newMesh.ProcessMesh(mesh, scene);
        meshes.push_back(newMesh);

        //std::cout << "Loaded mesh " << i << ": " << mesh->mName.C_Str() << std::endl;
    }

    //If the mesh contains any child meshes inside, process them too
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processModel(node->mChildren[i], scene);
    }
}


void Model::draw(Shader& shader) {
    //Applly tranforms
    shader.setMat4("model", transform);

    //Draw each mesh
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        meshes[i].draw(shader);
    }
}