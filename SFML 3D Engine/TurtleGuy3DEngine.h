#pragma once
#include "ParaonSimpleRenderingEngine.h"

#include "Mesh.h"
#include "GameObject.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "Triangle.h"
#include "Camera.h"

namespace tge {

class TurtleGuy3DEngine : public ParaonSimpleRenderingEngine {

public:
    TurtleGuy3DEngine();

private:

    Matrix3D matProj; //matrix that converts from view sapce to screen sapce

    Vector3D lightDirection;


    Camera camera;
    float Theta; //spins the world transform

    float* depthBuffer = nullptr;

    sf::Image testTexture;


    std::list<std::shared_ptr<GameObject>> gameObjects;

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
    void DrawTexturedTriangle(const Triangle& tri, const sf::Image& texture);

    
};

}
