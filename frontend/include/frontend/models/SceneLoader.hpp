#pragma once

#include <QString>
#include <Qt3DCore/QEntity>

namespace Assimp {
    class Importer;
}

struct aiMesh;
struct aiMaterial;
typedef struct mesh_t mesh_t;

namespace frontend {

class SceneLoader
{
public:
    SceneLoader();
    ~SceneLoader();

    QString getSupportedFormatsFilter() const;
    bool loadFile(const QString& fileName, QVector<mesh_t*>* ws_meshes, QString* errorMsg);

    static mesh_t* assToQt(const aiMesh* mesh, const aiMaterial* material);

private:
    Assimp::Importer* importer_;
    Qt3DCore::QEntity* rootEntity_;
};

}
