#include <iostream>
#include "olcConsoleGameEngine.h"
#include <string>


class PanAndZoom : public olcConsoleGameEngine {
public:
    PanAndZoom() {
        m_sAppName = L"Pan And Zoom";
    }

private:
    float fOffsetX = 0;
    float fOffsetY = 0;


    float fStartPanX = 0;
    float fStartPanY = 0;

    float fScaleX = 1;
    float fScaleY = 1;

    float fSelectedCellX = 0;
    float fSelectedCellY = 0;


    //Convert coordinates from World Space -> Screen space
    void WorldToScreen(float fWorldX, float fWorldY, int& nScreenX, int& nScreenY) {

        nScreenX = (int)((fWorldX - fOffsetX) * fScaleX);
        nScreenY = (int)((fWorldY - fOffsetY) * fScaleY);
    }




    //Convbert coordinates from Screen Space -> World space
    void ScreenToWorld(int nScreenX, int nScreenY, float& fWorldX, float& fWorldY) {
        fWorldX = (float)nScreenX / fScaleX + fOffsetX;
        fWorldY = (float)nScreenY / fScaleY + fOffsetY;
    }


protected:
    virtual bool OnUserCreate() override {
        /*fOffsetX = -ScreenWidth() / 2;
        fOffsetY = -ScreenHeight() / 2;*/
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override {
        
        float fMouseX = (float)GetMouseX();
        float fMouseY = (float)GetMouseY();


        // For paning, we need to capture the screen location when the user starts
        // to pan
        if (GetMouse(0).bPressed) {
            fStartPanX = fMouseX;
            fStartPanY = fMouseY;
        }


        // as the mose moves, the screen location changes. Convert this screen coordinate change into world change
        //to implement the pan. Simple
        if (GetMouse(0).bHeld) {
            fOffsetX -= (fMouseX- fStartPanX) / fScaleX;
            fOffsetY -= (fMouseY- fStartPanY) / fScaleY;

            // start "new" pan for next epoch
            fStartPanX = fMouseX;
            fStartPanY = fMouseY;

        }

        float fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom;
        ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom);

        if (GetKey(L'Q').bHeld) {
            fScaleX *= 1.001;
            fScaleY *= 1.001;
        }
        if (GetKey(L'A').bHeld) {
            fScaleX *= 0.999;
            fScaleY *= 0.999;
        }


        float fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom;
        ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom);

        fOffsetX += (fMouseWorldX_BeforeZoom - fMouseWorldX_AfterZoom);
        fOffsetY += (fMouseWorldY_BeforeZoom - fMouseWorldY_AfterZoom);

        if (GetMouse(1).bReleased) {
            fSelectedCellX = (int)fMouseWorldX_AfterZoom;
            fSelectedCellY = (int)fMouseWorldY_AfterZoom;
        }



        // DRAWING
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
        

        //Clip
        float fWorldLeft, fWorldTop, fWorldRight, fWorldBottom;
        ScreenToWorld(0, 0, fWorldLeft, fWorldTop);
        ScreenToWorld(ScreenWidth(), ScreenHeight(), fWorldRight, fWorldBottom);

        int nLinesDrawn = 0;

        //Draw 10 Horizontal lines
        for (float y = 0; y <= 10; y++) {

            if (y >= fWorldTop && y <= fWorldBottom) {

                float sx = 0, sy = y;
                float ex = 10, ey = y;

                int pixel_sx, pixel_sy, pixel_ex, pixel_ey;
                WorldToScreen(sx, sy, pixel_sx, pixel_sy);
                WorldToScreen(ex, ey, pixel_ex, pixel_ey);


                DrawLine(pixel_sx, pixel_sy, pixel_ex, pixel_ey, PIXEL_SOLID, FG_WHITE);
                nLinesDrawn += 1;
            }
            
        }

        //Draw 10 Vertical lines
        for (float x = 0; x <= 10; x++) {

            if (x >= fWorldLeft && x <= fWorldRight) {
                float sx = x, sy = 0;
                float ex = x, ey = 10;

                int pixel_sx, pixel_sy, pixel_ex, pixel_ey;
                WorldToScreen(sx, sy, pixel_sx, pixel_sy);
                WorldToScreen(ex, ey, pixel_ex, pixel_ey);


                DrawLine(pixel_sx, pixel_sy, pixel_ex, pixel_ey, PIXEL_SOLID, FG_WHITE);
                nLinesDrawn += 1;
            }
            
        }

        int cx, cy, cr;
        WorldToScreen(fSelectedCellX + 0.5, fSelectedCellY + 0.5, cx, cy);
        cr = 0.3f * fScaleX;
        FillCircle(cx, cy, cr, PIXEL_SOLID, FG_RED);


        auto function = [](float x) {
            return std::sinf(x);
        };

        //Draw Chart
        float fWorldPerScreenWidthPixel = (fWorldRight - fWorldLeft) / ScreenWidth();
        float fWorldPerScreenHeightPixel = (fWorldBottom - fWorldTop) / ScreenWidth();

        int px, py, opx = 0, opy = 0;
        WorldToScreen(fWorldLeft - fWorldPerScreenWidthPixel, -function((fWorldLeft - fWorldPerScreenWidthPixel)-5.f) + 5.f, opx, opy);

        for (float x = fWorldLeft; x < fWorldRight; x += fWorldPerScreenWidthPixel) {

            float y = -function(x - 5) + 5;
            

            WorldToScreen(x, y, px, py);
            DrawLine(opx, opy, px, py, PIXEL_SOLID, FG_GREEN);
            opx = px;
            opy = py;
        }

        DrawString(1, 2, std::to_wstring(fOffsetX) + L"/" + std::to_wstring(fOffsetY));
        DrawString(1, 5, L"Lines Drawin: " + std::to_wstring(nLinesDrawn));

        return true;

    }
};


int main()
{
    PanAndZoom demo;
    demo.ConstructConsole(160, 100, 8, 8);
    demo.Start();
    return 0;
}

