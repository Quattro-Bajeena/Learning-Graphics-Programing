
#include "olcConsoleGameEngine.h"
#include <iostream>
#include <cmath>


class FakeMode7 : public olcConsoleGameEngine {
public:
    FakeMode7() {
        m_sAppName = L"Psaudo 3D planes";
    }

private:
    olcSprite* sprGround;
    olcSprite* sprSky;
    int nMapSize = 1024;


    float fWorldX = 1000;
    float fWorldY = 1000;
    float fWorldA = 0;

    float fNear = 0.005;
    float fFar = 0.03;

    float fFoVHalf = 3.14159 / 4.;

    virtual bool OnUserCreate() override {
        sprGround = new olcSprite(L"mariokart.spr");
        sprSky = new olcSprite(L"sky1.spr");

        /*for (int x = 0; x < nMapSize; x+= 32) {
            for (int y = 0; y < nMapSize; y++) {
                sprGround->SetColour(x, y, FG_MAGENTA);
                sprGround->SetGlyph(x, y, PIXEL_SOLID);

                sprGround->SetColour(x + 1, y, FG_MAGENTA);
                sprGround->SetGlyph(x + 1, y, PIXEL_SOLID);

                sprGround->SetColour(x - 1, y, FG_MAGENTA);
                sprGround->SetGlyph(x - 1, y, PIXEL_SOLID);


                sprGround->SetColour(y, x, FG_BLUE);
                sprGround->SetGlyph(y, x, PIXEL_SOLID);

                sprGround->SetColour(y + 1, x, FG_BLUE);
                sprGround->SetGlyph(y + 1, x, PIXEL_SOLID);

                sprGround->SetColour(y - 1, x, FG_BLUE);
                sprGround->SetGlyph(y - 1, x, PIXEL_SOLID);
            }
        }*/


        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override {
        float fFarX1 = fWorldX + std::cosf(fWorldA - fFoVHalf) * fFar;
        float fFarY1 = fWorldY + std::sinf(fWorldA - fFoVHalf) * fFar;

        float fFarX2 = fWorldX + std::cosf(fWorldA + fFoVHalf) * fFar;
        float fFarY2 = fWorldY + std::sinf(fWorldA + fFoVHalf) * fFar;

        float fNearX1 = fWorldX + std::cosf(fWorldA - fFoVHalf) * fNear;
        float fNearY1 = fWorldY + std::sinf(fWorldA - fFoVHalf) * fNear;

        float fNearX2 = fWorldX + std::cosf(fWorldA + fFoVHalf) * fNear; 
        float fNearY2 = fWorldY + std::sinf(fWorldA + fFoVHalf) * fNear;

        for (int y = 0; y < ScreenHeight() / 2; y++) {
            float fSampleDepth = (float)y / (ScreenHeight() / 2.);
            //Normalized value 0 - 1
            //0 top of the screen
            //1 - bottom of screen
            
            //Scanline going across the frsutrum
            float fStartX = (fFarX1 - fNearX1) / ( fSampleDepth)+fNearX1;
            float fStartY = (fFarY1 - fNearY1) / ( fSampleDepth)+fNearY1;

            float fEndX = (fFarX2 - fNearX2) / ( fSampleDepth)+fNearX2;
            float fEndY = (fFarY2 - fNearY2) / ( fSampleDepth)+fNearY2;


            for (int x = 0; x < ScreenWidth(); x++) {
                float fSampleWidth = (float)x / ScreenWidth();
                float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
                float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

                fSampleX = std::fmod(fSampleX, 1.f);
                fSampleY = std::fmod(fSampleY, 1.f);

                

                wchar_t sym = sprGround->SampleGlyph(fSampleX, fSampleY);
                short col = sprGround->SampleColour(fSampleX, fSampleY);
                
                Draw(x, y + (ScreenHeight() / 2), sym, col);

                sym = sprSky->SampleGlyph(fSampleX, fSampleY);
                col = sprSky->SampleColour(fSampleX, fSampleY);

                Draw(x,  (ScreenHeight() / 2) - y, sym, col);
            }

        }

        // Draw a blanking line to fill the gap between sky and ground
        DrawLine(0, ScreenHeight() / 2, ScreenWidth(), ScreenHeight() / 2, PIXEL_SOLID, FG_CYAN);

        if (GetKey(L'Q').bHeld) fNear += 0.1f * fElapsedTime;
        if (GetKey(L'A').bHeld) fNear -= 0.1f * fElapsedTime;

        if (GetKey(L'W').bHeld) fFar += 0.1f * fElapsedTime;
        if (GetKey(L'S').bHeld) fFar -= 0.1f * fElapsedTime;

        if (GetKey(L'Z').bHeld) fFoVHalf += 0.1f * fElapsedTime;
        if (GetKey(L'X').bHeld) fFoVHalf -= 0.1f * fElapsedTime;

        if (GetKey(VK_LEFT).bHeld)
            fWorldA -= 1.f * fElapsedTime;

        if (GetKey(VK_RIGHT).bHeld)
            fWorldA += 1.f * fElapsedTime;

        if (GetKey(VK_UP).bHeld) {
            fWorldX += cosf(fWorldA) * 0.2f * fElapsedTime;
            fWorldY += sinf(fWorldA) * 0.2f * fElapsedTime;
        }

        if (GetKey(VK_DOWN).bHeld) {
            fWorldX -= cosf(fWorldA) * 0.2f * fElapsedTime;
            fWorldY -= sinf(fWorldA) * 0.2f * fElapsedTime;
        }

        return true;
    }
};










int main()
{
    FakeMode7 game;
    game.ConstructConsole(320, 240, 4, 4);
    game.Start();
    return 0;
}

