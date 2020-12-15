#pragma once
#include "ParaonSimpleRenderingEngine.h"

#include "Mesh.h"
#include "GameObject.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "Triangle.h"

namespace tge {

class TurtleGuy3DEngine : public ParaonSimpleRenderingEngine {

public:
    TurtleGuy3DEngine();

private:
    Matrix3D matProj; //matrix that converts from view sapce to screen sapce

    Vector3D cameraPos; //location of cameara in world sapce
    Vector3D lookDir; //direction vector along he direction camera pints

    Vector3D lightDirection;

    float Yaw; //fps camara rotation in XZ plane
    float Theta; //spins the world transform

    float* DepthBuffer = nullptr;


    std::list<GameObject> gameObjects;

    virtual void Initialize() override;

    virtual void Update(float deltaTime) override;

    void InitializeObjects();

    void UpdateObjects(float deltaTime);
    void RenderObjects();

    void UpdateInput(float deltaTime);
    Matrix3D CreateViewMatrix();
    std::vector<Triangle> PrepMeshToRaster(const Mesh& mesh, Matrix3D transform, Matrix3D view_matrix);


    void RasterTriangles(const std::vector<Triangle>& triangles_to_raster);
    void FillTriangle(const Triangle& tri);
    void DrawTriangle(const Triangle& tri, sf::Color color);
};

}
