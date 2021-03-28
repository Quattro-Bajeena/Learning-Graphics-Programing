#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <algorithm>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct sEdge {
    float sx, sy;
    float ex, ey;
};

struct sCell {
    int edge_id[4];
    bool edge_exist[4];
    bool exist = false;
};

enum DIR {
    NORTH = 0,
    SOUTH = 1,
    EAST = 2,
    WEST = 3
};



class ShadowCasting2D : public olc::PixelGameEngine {
public:
    ShadowCasting2D() {
        sAppName = "ShadowCasting2D";

    }

private:
    sCell* world;
    int nWorldWidth = 40;
    int nWorldHeight = 30;

    std::vector<sEdge> vecEdges;
    std::vector<std::tuple<float, float, float>> vecVisibilityPolygonPoints;

    olc::Sprite* sprLightCast;
    olc::Sprite* buffLightRay;
    olc::Sprite* buffLightTex;


    void ConverTileMapToPolyMap(int sx, int sy, int w, int h, float fBlockWidth, int pitch) {
        //Clear polymap
        vecEdges.clear();

        for(int x = 0; x < w; x++)
            for (int y = 0; y < h; y++) 
                for(int j = 0; j < 4; j++){
                    world[(y + sy) * pitch + (x + sx)].edge_exist[j] = false;
                    world[(y + sy) * pitch + (x + sx)].edge_id[j] = 0;
                }

        //Iterate through the region from top left to bottom right
        for(int x = 1; x < w - 1; x++)
            for (int y = 1; y < h - 1; y++) {

                //create some convenete indices
                int i = (y + sy) * pitch + (x + sx); //this
                int n = (y + sy - 1) * pitch + (x + sx); //northen neighbour
                int s = (y + sy + 1) * pitch + (x + sx); //southern neighbour
                int w = (y + sy) * pitch + (x + sx - 1); //wester neighbour
                int e = (y + sy) * pitch + (x + sx + 1); //eastern neighbour


                int neighbour_sides[4];
                neighbour_sides[DIR::WEST] = w;
                neighbour_sides[DIR::EAST] = e;
                neighbour_sides[DIR::NORTH] = n;
                neighbour_sides[DIR::SOUTH] = s;

                int neighbour_check_edge[4];
                neighbour_check_edge[DIR::WEST] = n;
                neighbour_check_edge[DIR::EAST] = n;
                neighbour_check_edge[DIR::SOUTH] = w;
                neighbour_check_edge[DIR::NORTH] = w;



                //If this cell exists, check it needs edges
                if (world[i].exist) {

                    for (int direction = 0; direction < 4; direction++) {

                        //if this cell has no western neghbopur, it needs a western edge
                        if (world[neighbour_sides[direction]].exist == false) {

                            //it can either extend it from its norther neighbour if they have western edge
                            //one, ar it can start a new one
                            if (world[neighbour_check_edge[direction]].edge_exist[direction]) {

                                //Northern neighbour has a wetsern edge, so grow it downwards
                                if(direction == DIR::EAST || direction == DIR::WEST)
                                    vecEdges[world[neighbour_check_edge[direction]].edge_id[direction]].ey += fBlockWidth;
                                else
                                    vecEdges[world[neighbour_check_edge[direction]].edge_id[direction]].ex += fBlockWidth;

                                world[i].edge_id[direction] = world[neighbour_check_edge[direction]].edge_id[direction];
                                world[i].edge_exist[direction] = true;
                            }
                            else {
                                //Norther neighbour doewsnt gave one, so create new
                                sEdge edge;

                                if (direction == DIR::SOUTH) {
                                    edge.sx = (sx + x) * fBlockWidth;
                                    edge.sy = (sy + y + 1) * fBlockWidth;
                                }
                                else if(direction == DIR::EAST){
                                    edge.sx = (sx + x + 1) * fBlockWidth;
                                    edge.sy = (sy + y) * fBlockWidth;
                                }
                                else {
                                    edge.sx = (sx + x) * fBlockWidth;
                                    edge.sy = (sy + y) * fBlockWidth;
                                }

                                if (direction == DIR::WEST || direction == DIR::EAST) {
                                    edge.ex = edge.sx;
                                    edge.ey = edge.sy + fBlockWidth;
                                }
                                else {
                                    edge.ex = edge.sx + fBlockWidth;
                                    edge.ey = edge.sy;
                                }
                                

                                //Add edge to Polygon Pool
                                int edge_id = vecEdges.size();
                                vecEdges.push_back(edge);

                                // Update tile information with edge infoamtion
                                world[i].edge_id[direction] = edge_id;
                                world[i].edge_exist[direction] = true;
                            }
                        }
                    } 
                }
            }
    }

    void CalculateVisibilityPolygon(float ox, float oy, float radius) {
        //Get rid of exisitn polygon
        vecVisibilityPolygonPoints.clear();

        //for each edge in PolyMap
        for (auto& e1 : vecEdges) {
            // Take the start point, then the end point (we could use a
            // non duplicate points here, it  would be more optimal

            for (int i = 0; i < 2; i++) {

                //Ray vector
                float rdx, rdy;
                rdx = (i == 0 ? e1.sx : e1.ex) - ox;
                rdy = (i == 0 ? e1.sy : e1.ey) - oy;

                float base_ang = std::atan2f(rdy, rdx);

                float ang;
                //for each point cast 3 rays, 1 directly at point
                // and 1 a little bit either side
                for (int j = 0; j < 3; j++) {
                    if (j == 0) ang = base_ang - 0.0001f;
                    if (j == 1) ang = base_ang;
                    if (j == 2) ang = base_ang + 0.0001f;


                    //create ray along angle for required distance
                    rdx = radius * std::cosf(ang);
                    rdy = radius * std::sinf(ang);
                    
                    float min_t1 = INFINITY;
                    float min_px = 0, min_py = 0, min_ang = 0;
                    bool bValid = false;

                    //check for ray intersection with all edges
                    for (auto& e2 : vecEdges) {

                        //create line segment bector
                        float sdx = e2.ex - e2.sx;
                        float sdy = e2.ey - e2.sy;

                        if (std::fabs(sdx - rdx) > 0 && std::fabs(sdy - rdy) > 0) {

                            //https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
                            //t1, t2 are from 0 to 1
                            // t2 is normalized distance from line segment start to line segment end of intersect point
                            float t2 = (rdx * (e2.sy - oy) - (rdy * (e2.sx - ox))) / (sdx * rdy - sdy * rdx);
                            //t1 is normalised distance from sorce alog ray to ray lenght
                            float t1 = (e2.sx + sdx * t2 - ox) / rdx;


                            // if intersect point exists alog ray, and along line
                            // segment then intersect point is valid
                            if (t1 > 0 && t2 >= 0 && t2 <= 1 && t1 <= 1) {

                                //std::cout << rdx << "/" << rdy << "|" << ox << "/" << oy <<"|"<<t1 <<"\n";
                                //checks if this intersect point is closest to source.
                                //if it is, then store this point and reject others
                                if (t1 < min_t1) {
                                    min_t1 = t1;
                                    min_px = ox + rdx * t1;
                                    min_py = oy + rdy * t1;
                                    min_ang = std::atan2f(min_py - oy, min_px - ox);
                                    bValid = true;
                                }
                            }
                        }
                    }

                    if(bValid)
                        vecVisibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
                }


            }
        }

        //sort permiter points by angle from souirce. this will allow
        // us to draw a trinagle fan.

        std::sort(
            vecVisibilityPolygonPoints.begin(),
            vecVisibilityPolygonPoints.end(),
            [&](const std::tuple<float, float, float>& t1, const std::tuple<float, float, float>& t2) {
                return std::get<0>(t1) < std::get<0>(t2);
            });

    }


public:
    bool OnUserCreate() override {
        world = new sCell[nWorldWidth * nWorldHeight];

        for (int x = 1; x < (nWorldWidth - 1); x++) {
            world[1 * nWorldWidth + x].exist = true;
            world[(nWorldHeight - 2) * nWorldWidth + x].exist = true;
        }

        for (int y = 1; y < (nWorldHeight - 1); y++) {
            world[y * nWorldWidth + 1].exist = true;
            world[y * nWorldWidth   + (nWorldWidth - 2)].exist = true;
        }

        sprLightCast = new olc::Sprite("light_cast.png");
        buffLightRay = new olc::Sprite(ScreenWidth(), ScreenHeight());
        buffLightTex = new olc::Sprite(ScreenWidth(), ScreenHeight());

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {

        float fBlockWidth = 16;
        float fSourceX = GetMouseX();
        float fSourceY = GetMouseY();

        if (GetMouse(0).bReleased) {

            //i = y * width + x
            int i = ((int)fSourceY / (int)fBlockWidth * nWorldWidth + (int)fSourceX / (int)fBlockWidth);
            world[i].exist = !world[i].exist;

            // Take a region of TileMap and conver it to PolyMap - This id done
            //every frame here, nut cpi;d be a pre-procesing stage depeding on
            // how your final appilcation itneracts with tilemaps
            ConverTileMapToPolyMap(0, 0, 40, 30, fBlockWidth, nWorldWidth);
        }

        
        if (GetMouse(1).bHeld) {
            CalculateVisibilityPolygon(fSourceX, fSourceY, 1000);
        }


        //Drawing
        SetDrawTarget(nullptr);


        Clear(olc::BLACK);
        int nRayCast = vecVisibilityPolygonPoints.size();

        auto it = std::unique(
            vecVisibilityPolygonPoints.begin(),
            vecVisibilityPolygonPoints.end(),
            [&](const std::tuple<float, float, float>& t1, const std::tuple<float, float, float>& t2) {

                return std::fabs(std::get<1>(t1) - std::get<1>(t2)) < 0.1f && std::fabs(std::get<2>(t1) - std::get<2>(t2)) < 0.1f;
                
            }
        );
        vecVisibilityPolygonPoints.resize(std::distance(vecVisibilityPolygonPoints.begin(), it));
        

        int nRayCast2 = vecVisibilityPolygonPoints.size();

        DrawString(4, 4, "Ray Cast: " + std::to_string(nRayCast) + " Ray Drawn: " + std::to_string(nRayCast2));

        // if drawing rays, set an offscreen texture as our target buffer
        if (GetMouse(1).bHeld && vecVisibilityPolygonPoints.size() > 1) {

            //Clear offscreen buffer for sprite
            SetDrawTarget(buffLightTex);
            Clear(olc::BLACK);

            // draw "radial light" spirte to offscreen buffer, centered aorund
            // source location (the moouse coordintes, buffer is 512x512

            DrawSprite(fSourceX - 255, fSourceY - 255, sprLightCast);


            // Clear offscreen buffer for rays
            SetDrawTarget(nullptr);
            Clear(olc::BLANK);

            //draw each triangle in fan
            for (int i = 0; i < vecVisibilityPolygonPoints.size() - 1; i++) {
                FillTriangle(
                    fSourceX,
                    fSourceY,

                    std::get<1>(vecVisibilityPolygonPoints[i]),
                    std::get<2>(vecVisibilityPolygonPoints[i]),

                    std::get<1>(vecVisibilityPolygonPoints[i + 1]),
                    std::get<2>(vecVisibilityPolygonPoints[i + 1])
                );
            }

            FillTriangle(
                fSourceX,
                fSourceY,

                std::get<1>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]),
                std::get<2>(vecVisibilityPolygonPoints[vecVisibilityPolygonPoints.size() - 1]),

                std::get<1>(vecVisibilityPolygonPoints[0]),
                std::get<2>(vecVisibilityPolygonPoints[0])
            );

            //Wherever rays exist in ray sprite, copy over radial light spirte
            SetDrawTarget(nullptr);
            for (int x = 0; x < ScreenWidth(); x++)
                for (int y = 0; y < ScreenHeight(); y++)
                    if (buffLightRay->GetPixel(x, y).r > 0)
                        Draw(x, y, buffLightTex->GetPixel(x, y));

        }

        for(int x = 0; x < nWorldWidth; x++)
            for (int y = 0; y < nWorldHeight; y++) {
                if (world[y * nWorldWidth + x].exist)
                    FillRect(x * fBlockWidth, y * fBlockWidth, fBlockWidth, fBlockWidth, olc::BLUE);
            }


        for (auto& e : vecEdges) {
            DrawLine(e.sx, e.sy, e.ex, e.ey);
            FillCircle(e.sx, e.sy, 3, olc::RED);
            FillCircle(e.ex, e.ey, 3, olc::RED);
        }

        return true;
    }

};

int main()
{

    ShadowCasting2D game;
    if(game.Construct(640, 480, 2, 2))
        game.Start();
}


