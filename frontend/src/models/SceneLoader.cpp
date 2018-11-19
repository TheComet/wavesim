#include "frontend/models/SceneLoader.hpp"
#include "assimp/Importer.hpp"
#include "assimp/importerdesc.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
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
Qt3DCore::QEntity* SceneLoader::loadFile(const QString& fileName, QString* errorMsg)
{
    const aiScene* assimpScene = importer_->ReadFile(fileName.toUtf8().constData(), aiProcess_Triangulate);
    if (assimpScene == nullptr)
    {
        if (errorMsg)
            *errorMsg = importer_->GetErrorString();
        return nullptr;
    }

    return nullptr;
}
