#pragma once

#include <QString>
#include <Qt3DCore/QEntity>

namespace Assimp {
    class Importer;
}

struct aiMesh;
struct aiMaterial;

namespace frontend {

class SceneLoader
{
public:
    SceneLoader();
    ~SceneLoader();

    QString getSupportedFormatsFilter() const;
    bool loadFile(const QString& fileName, QVector<Qt3DCore::QEntity*>* entities, QString* errorMsg);

    static Qt3DCore::QEntity* assToQt(const aiMesh* mesh, const aiMaterial* material);

private:
    Assimp::Importer* importer_;
    Qt3DCore::QEntity* rootEntity_;
};

}
