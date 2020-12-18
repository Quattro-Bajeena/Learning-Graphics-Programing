#include "stdafx.h"
#include "TurtleGuy3DEngine.h"

#include "TestCubeScript.h"




namespace tge {


TurtleGuy3DEngine::TurtleGuy3DEngine() {

}

void TurtleGuy3DEngine::Initialize() {


    Mesh teapot_mesh;
    teapot_mesh.LoadFromObjectFile("teapot.obj", true);
    

    std::shared_ptr<GameObject> teapot = std::make_shared<GameObject>(teapot_mesh);
    teapot->AddComponent<TestCubeScript>();

    gameObjects.push_back(teapot);

    testTexture.loadFromFile("teapot texture.png");
        

    float fFov = 60.f;
    float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
    float fNear = 0.1f;
    float fFar = 1000.f;

    camera = Camera(fFov, fAspectRatio, fNear, fFar);

    matProj = camera.GetProjMatrix();

    lightDirection = Vector3D(0, 1, -1).Normalized();

    depthBuffer = new float[ScreenWidth() * ScreenHeight()];

}

void TurtleGuy3DEngine::Update(float deltaTime) {

    //std::cout << 1 / deltaTime << "\n";
    /*
    Create Cameara View Matrix
    For all meshes in scene:

        Create transforamtion matrix

        for all triangles in mesh:

            apply translation/roation/scale from local to World Space
            Calculate Normal
            Chceck if camera ray is aligned with normal (is triangle visible)

            if triangle visible:
                calculat light intensity
                convert from world sapce to view space (camera)
                clip triangle again near plane -> potentali two new triangles:

                for new triagnles:
                    project triangle from 3D to 2D
                    scale according to w
                    offset verices into visible normalzied space (?)
                    push triangles to Raster


        Sort triangles to Raster
        make depth buffer

        for all triagnles to raster:
            clip against every screen edge (we are in 2d now btw) -> may prodcue more triangles
            continue untill all are clipped
    */



    UpdateInput(deltaTime);
    UpdateObjects(deltaTime);
    RenderObjects();

}

void TurtleGuy3DEngine::InitializeObjects()
{
}

void TurtleGuy3DEngine::UpdateObjects(float deltaTime)
{
    for (std::shared_ptr<GameObject>& gameObject : gameObjects) {
        gameObject->Update(deltaTime);
    }
}

void TurtleGuy3DEngine::RenderObjects() {

    Matrix3D view_matrix = CreateViewMatrix();

    for (const std::shared_ptr<GameObject>& object : gameObjects) {


        Matrix3D transform_matrix = object->transform.TransformMatrix();

        std::vector<Triangle> triangles_to_raster = PrepMeshToRaster(object->mesh, transform_matrix, view_matrix);

        std::sort(triangles_to_raster.begin(), triangles_to_raster.end(),
            [](Triangle& t1, Triangle& t2)
            {
                float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.f;
                float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.f;
                return z1 > z2;
            });

        for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
            depthBuffer[i] = 0.f;


        RasterTriangles(triangles_to_raster);
    }


}

void TurtleGuy3DEngine::UpdateInput(float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        camera.transform.position.x += 8 * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        camera.transform.position.x -= 8 * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        camera.transform.position.y -= 8 * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        camera.transform.position.y += 8 * deltaTime;
    }

    Vector3D forward = camera.LookDir() * (8 * deltaTime);

    Vector3D look_right = MatRotY(-0.5 * M_PI) * forward;
    Vector3D right = look_right * (32 * deltaTime);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        camera.transform.position +=  right;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        camera.transform.position =- right;


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        camera.transform.position += forward;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        camera.transform.position -= forward;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        camera.transform.rotation.y -= deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        camera.transform.rotation.y += deltaTime;
    }
}

Matrix3D TurtleGuy3DEngine::CreateViewMatrix() {

    Vector3D target = camera.transform.position + camera.LookDir();
    Matrix3D matCamera = MatLookAt(camera.transform.position, target, Vector3D(0, 1, 0));
    Matrix3D mat_view = MatQuickInverse(matCamera);
    return mat_view;
}

std::vector<Triangle> TurtleGuy3DEngine::PrepMeshToRaster(const Mesh& mesh, Matrix3D transform, Matrix3D view_matrix) {
    std::vector<Triangle> triangles_to_raster;

    for (const Triangle tri : mesh.GetTris()) {

        Triangle tri_transformed = transform * tri;

        Vector3D line1 = tri_transformed.points[1] - tri_transformed.points[0];
        Vector3D line2 = tri_transformed.points[2] - tri_transformed.points[0];
        Vector3D tri_normal = CrossProduct(line1, line2).Normalized();

        Vector3D camera_ray = tri_transformed.points[0] - camera.transform.position;

        if (DotProduct(tri_normal, camera_ray) < 0.f) {



            float light_intensity = std::max(0.1f, DotProduct(lightDirection, tri_normal)) * 255;
            tri_transformed.color = sf::Color(light_intensity, light_intensity, light_intensity);

            Triangle tri_viewed = view_matrix * tri_transformed;

            Triangle tri_clipped[2];

            int clipped_triangles = ClipAgainstPlane(Vector3D(0.f, 0.f, 0.1f), Vector3D(0.f, 0.f, 1.f), tri_viewed, tri_clipped[0], tri_clipped[1]);

            for (int n = 0; n < clipped_triangles; n++) {

                Triangle tri_projected = matProj * tri_clipped[n];

                for (int i = 0; i < 3; i++) {
                    tri_projected.uvCords[i].u /= tri_projected.points[i].w;
                    tri_projected.uvCords[i].v /= tri_projected.points[i].w;
                    tri_projected.uvCords[i].w = 1.f / tri_projected.points[i].w;
                }

                tri_projected.ScaleUvByW();
                tri_projected.ScalePointsByW();

                for (int i = 0; i < 3; i++) {
                    tri_projected.points[i].x *= -1;
                    tri_projected.points[i].y *= -1;
                }


                Vector3D offset_view = Vector3D(1, 1, 0);
                tri_projected = tri_projected + offset_view;

                for (int i = 0; i < 3; i++) {
                    tri_projected.points[i].x *= 0.5f * ScreenWidth();
                    tri_projected.points[i].y *= 0.5f * ScreenHeight();
                }


                triangles_to_raster.push_back(tri_projected);

            }
        }
    }

    return triangles_to_raster;
}

void TurtleGuy3DEngine::RasterTriangles(const std::vector<Triangle>& triangles_to_raster) {

    for (const Triangle& tri_to_raster : triangles_to_raster) {

        Triangle tri_clipped[2];
        std::list<Triangle> triangles_list;

        triangles_list.push_back(tri_to_raster);
        int new_triangles = 1;


        for (int plane = 0; plane < 4; plane++) {


            int tris_to_add = 0;
            while (new_triangles > 0) {
                // Take triangle from front of queue

                Triangle test = triangles_list.front();
                triangles_list.pop_front();
                new_triangles--;

                // Clip it against a plane. We only need to test each 
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified

                //0 - top
                //1- bottom
                //2 - left
                //3 - right

                switch (plane) {
                case 0: tris_to_add = ClipAgainstPlane(Vector3D(0.f, 2.f, 0.f), Vector3D(0.f, 1.f, 0.f), test, tri_clipped[0], tri_clipped[1]); break;
                case 1: tris_to_add = ClipAgainstPlane(Vector3D(0.f, (float)ScreenHeight() - 2, 0.f), Vector3D(0.f, -1.f, 0.f), test, tri_clipped[0], tri_clipped[1]); break;
                case 2: tris_to_add = ClipAgainstPlane(Vector3D(2.f, 0.f, 0.f), Vector3D(1.f, 0.f, 0.f), test, tri_clipped[0], tri_clipped[1]); break;
                case 3: tris_to_add = ClipAgainstPlane(Vector3D((float)ScreenWidth() - 2, 0.f, 0.f), Vector3D(-1.f, 0.f, 0.f), test, tri_clipped[0], tri_clipped[1]); break;
                }


                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int i = 0; i < tris_to_add; i++) {
                    triangles_list.push_back(tri_clipped[i]);
                }
            }

            new_triangles = triangles_list.size();

        }

        for (const Triangle& tri : triangles_list) {
            //FillTriangle(tri);
            //DrawTriangle(tri, sf::Color::Black);
            DrawTexturedTriangle(tri, testTexture);
        }

    }
}


void TurtleGuy3DEngine::FillTriangle(const Triangle& tri) {


    //void FillTriangle(sf::Vector2i point1, sf::Vector2i point2, sf::Vector2i point3, sf::Color color)
    sf::Vector2i point1 = sf::Vector2i(tri.points[0].x, tri.points[0].y);
    sf::Vector2i point2 = sf::Vector2i(tri.points[1].x, tri.points[1].y);
    sf::Vector2i point3 = sf::Vector2i(tri.points[2].x, tri.points[2].y);

    ParaonSimpleRenderingEngine::FillTriangle(point1, point2, point3, tri.color);

}

void TurtleGuy3DEngine::DrawTriangle(const Triangle& tri, sf::Color color) {
    sf::Vector2i point1 = sf::Vector2i(tri.points[0].x, tri.points[0].y);
    sf::Vector2i point2 = sf::Vector2i(tri.points[1].x, tri.points[1].y);
    sf::Vector2i point3 = sf::Vector2i(tri.points[2].x, tri.points[2].y);

    ParaonSimpleRenderingEngine::DrawTriangle(point1, point2, point3, color);
}

void TurtleGuy3DEngine::DrawTexturedTriangle(const Triangle& tri, const sf::Image& texture)
{

    int x1 = tri.points[0].x; int y1 = tri.points[0].y; float w1 = tri.points[0].w;
    int x2 = tri.points[1].x; int y2 = tri.points[1].y; float w2 = tri.points[1].w;
    int x3 = tri.points[2].x; int y3 = tri.points[2].y; float w3 = tri.points[2].w;

    float u1 = tri.uvCords[0].u; float v1 = tri.uvCords[0].v;
    float u2 = tri.uvCords[1].u; float v2 = tri.uvCords[1].v;
    float u3 = tri.uvCords[2].u; float v3 = tri.uvCords[2].v;

    //std::cout << u1 << ";" << v1 << ";" << w1 << "\n";

    ParaonSimpleRenderingEngine::TexturedTriangle(
        x1, y1, u1, v1, w1,
        x2, y2, u2, v2, w2,
        x3, y3, u3, v3, w3,
        depthBuffer, texture
    );
}





}




