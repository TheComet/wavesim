#include "frontend/models/SceneLoader.hpp"
#include "assimp/Importer.hpp"
#include "assimp/importerdesc.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "wavesim/mesh/mesh.h"
#include <QStringList>

using namespace frontend;

// ----------------------------------------------------------------------------
SceneLoader::SceneLoader() :
    importer_(new Assimp::Importer),
    rootEntity_(new Qt3DCore::QEntity)
{
}

// ----------------------------------------------------------------------------
SceneLoader::~SceneLoader()
{
    delete rootEntity_;
    delete importer_;
}

// ----------------------------------------------------------------------------
QString SceneLoader::getSupportedFormatsFilter() const
{
    QStringList filterList;
    for (int i = 0; i != importer_->GetImporterCount(); ++i)
    {
        const aiImporterDesc* desc = importer_->GetImporterInfo(i);
        if (desc == nullptr)
            continue;

        QStringList extensionList;
        for (const auto& ext : QString(desc->mFileExtensions).split(" "))
            extensionList.append("*." + ext);
        filterList.append(QString(desc->mName) + " (" + extensionList.join(" ") + ")");
    }
    filterList.append("All Files (*)");

    return filterList.join(";;");
}

// ----------------------------------------------------------------------------
mesh_t* SceneLoader::assToQt(const aiMesh* mesh, const aiMaterial* material)
{
    // 3 floats for position
    // 3 floats for normals
    // 3 floats for colors
    std::size_t vertexSize = (3 + 3 + 3) * sizeof(float);

    QByteArray vertexBuffer;
    vertexBuffer.resize(mesh->mNumVertices * vertexSize);
    std::size_t idx = 0;
    for (unsigned int i = 0; i != mesh->mNumVertices; ++i)
    {
        float* ptr = reinterpret_cast<float*>(vertexBuffer.data());
        ptr[idx++] = mesh->mVertices[i].x;
        ptr[idx++] = mesh->mVertices[i].y;
        ptr[idx++] = mesh->mVertices[i].z;
        ptr[idx++] = mesh->mNormals[i].x;
        ptr[idx++] = mesh->mNormals[i].y;
        ptr[idx++] = mesh->mNormals[i].z;
        ptr[idx++] = 0.0f;
        ptr[idx++] = 0.2f;
        ptr[idx++] = 0.5f;
    }

    // Indices
    QByteArray indexBuffer;
    indexBuffer.resize(mesh->mNumFaces * 3 * sizeof(uint16_t));
    idx = 0;
    for (unsigned int i = 0; i != mesh->mNumFaces; ++i)
    {
        uint16_t* ptr = reinterpret_cast<uint16_t*>(indexBuffer.data());
        assert(mesh->mFaces[i].mNumIndices == 3);
        ptr[idx++] = mesh->mFaces[i].mIndices[0];
        ptr[idx++] = mesh->mFaces[i].mIndices[1];
        ptr[idx++] = mesh->mFaces[i].mIndices[2];
    }

    return nullptr;
}

// ----------------------------------------------------------------------------
static void processAssNode(QVector<mesh_t*>* ws_meshes,
                           const aiNode* node,
                           const aiMesh*const* meshes,
                           const aiMaterial*const* materials,
                           aiMatrix4x4 accTransform)
{
    for (unsigned int i = 0; i != node->mNumMeshes; ++i)
    {
        unsigned int meshIdx = node->mMeshes[i];
        const aiMesh* mesh = meshes[meshIdx];
        unsigned int materialIdx = mesh->mMaterialIndex;
        const aiMaterial* material = materials[materialIdx];

        // Only support meshes that are composed of triangles
        if ((mesh->mPrimitiveTypes & ~aiPrimitiveType_TRIANGLE))
            continue;

        mesh_t* ws_mesh = SceneLoader::assToQt(mesh, material);
        ws_meshes->push_back(ws_mesh);
    }

    for (unsigned int i = 0; i != node->mNumChildren; ++i)
    {
        const aiNode* child = node->mChildren[i];
        processAssNode(ws_meshes, child, meshes, materials, node->mTransformation * accTransform);
    }
}

// ----------------------------------------------------------------------------
bool SceneLoader::loadFile(const QString& fileName, QVector<mesh_t*>* ws_meshes, QString* errorMsg)
{
    const aiScene* assScene = importer_->ReadFile(fileName.toUtf8().constData(),
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_SplitLargeMeshes |
        aiProcess_ImproveCacheLocality |
        aiProcess_GenNormals |
        aiProcess_FindDegenerates |
        aiProcess_FindInvalidData);
    if (assScene == nullptr)
    {
        if (errorMsg)
            *errorMsg = importer_->GetErrorString();
        return false;
    }

    if (assScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        *errorMsg = "AI_SCENE_FLAGS_INCOMPLETE is set, aborting";
        return false;
    }

    processAssNode(ws_meshes, assScene->mRootNode, assScene->mMeshes, assScene->mMaterials, aiMatrix4x4());

    return true;
}
