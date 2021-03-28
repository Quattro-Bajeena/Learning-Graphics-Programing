#include "stdafx.h"
#include "TurtleGuy3DEngine.h"


int main()
{
    
    tge::TurtleGuy3DEngine engine;
    int pixel_size = 2;
    engine.CreateWindow(1920 / pixel_size, 1080 / pixel_size, pixel_size);
    engine.Play();

    return 0;
}