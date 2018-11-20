#include "frontend/models/SceneLoader.hpp"
#include "assimp/Importer.hpp"
#include "assimp/importerdesc.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <QStringList>
#include <QMatrix4x4>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>

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
Qt3DCore::QEntity* SceneLoader::assToQt(const aiMesh* mesh, const aiMaterial* material)
{
    Qt3DCore::QEntity* customMeshEntity = new Qt3DCore::QEntity;

    // Custom Mesh (TetraHedron)
    Qt3DRender::QGeometryRenderer* customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

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

    Qt3DRender::QBuffer* vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
    Qt3DRender::QBuffer* indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);
    vertexDataBuffer->setData(vertexBuffer);
    indexDataBuffer->setData(indexBuffer);

    // Attributes
    Qt3DRender::QAttribute* positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(9 * sizeof(float));
    positionAttribute->setCount(mesh->mNumVertices);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexDataBuffer);
    normalAttribute->setDataType(Qt3DRender::QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(3 * sizeof(float));
    normalAttribute->setByteStride(9 * sizeof(float));
    normalAttribute->setCount(mesh->mNumVertices);
    normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(6 * sizeof(float));
    colorAttribute->setByteStride(9 * sizeof(float));
    colorAttribute->setCount(mesh->mNumVertices);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(mesh->mNumFaces * 3);

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(normalAttribute);
    customGeometry->addAttribute(colorAttribute);
    customGeometry->addAttribute(indexAttribute);

    customMeshRenderer->setInstanceCount(1);
    customMeshRenderer->setFirstVertex(0);
    customMeshRenderer->setFirstInstance(0);
    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    customMeshRenderer->setGeometry(customGeometry);
    customMeshRenderer->setVertexCount(mesh->mNumFaces * 3); // ?? This is the number of primitives * 3 somehow

    customMeshEntity->addComponent(customMeshRenderer);

    return customMeshEntity;
}

// ----------------------------------------------------------------------------
static void processAssNode(QVector<Qt3DCore::QEntity*>* qtEntities,
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

        Qt3DCore::QEntity* entity = SceneLoader::assToQt(mesh, material);
        Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;
        transform->setMatrix(QMatrix4x4(
            accTransform.a1, accTransform.a2, accTransform.a3, accTransform.a4,
            accTransform.b1, accTransform.b2, accTransform.b3, accTransform.b4,
            accTransform.c1, accTransform.c2, accTransform.c3, accTransform.c4,
            accTransform.d1, accTransform.d2, accTransform.d3, accTransform.d4
        ));
        entity->addComponent(transform);
        qtEntities->push_back(entity);
    }

    for (unsigned int i = 0; i != node->mNumChildren; ++i)
    {
        const aiNode* child = node->mChildren[i];
        processAssNode(qtEntities, child, meshes, materials, node->mTransformation * accTransform);
    }
}

// ----------------------------------------------------------------------------
bool SceneLoader::loadFile(const QString& fileName, QVector<Qt3DCore::QEntity*>* entities, QString* errorMsg)
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

    processAssNode(entities, assScene->mRootNode, assScene->mMeshes, assScene->mMaterials, aiMatrix4x4());

    return true;
}
