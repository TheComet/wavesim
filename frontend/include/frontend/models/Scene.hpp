#pragma once

#include <QString>
#include <Qt3DCore/QEntity>

struct aiScene;

namespace Assimp {
    class Importer;
}

namespace frontend {

class Scene
{
public:
    Scene();
    ~Scene();

    QString getSupportedFormatsFilter() const;
    Qt3DCore::QEntity* loadFile(const QString& fileName, QString* errorMsg);


private:
    bool createQtMeshesFromAssimpScene(const aiScene* scene);

private:
    Assimp::Importer* importer_;
    Qt3DCore::QEntity* rootEntity_;
};

}
