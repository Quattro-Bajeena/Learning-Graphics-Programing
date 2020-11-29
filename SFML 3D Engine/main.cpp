#include "ParaonSimpleRenderingEngine.h"
#include <random>
class Paraon3DRenderingEngine : public ParaonSimpleRenderingEngine {
public:
    Paraon3DRenderingEngine() {

    }
private:
    virtual void Initialize() override {

    }

    virtual void Update(float deltaTime) override {
        for (int y = 0; y < ScreenSize().y; y++) {
            for (int x = 0; x < ScreenSize().x; x++) {


                DrawPixel(x, y, rand() % 2 ? sf::Color::White : sf::Color::Black);
            }
        }

        //DrawLine(sf::Vector2i(50, 0), sf::Vector2i(0, 50), sf::Color::Blue);
        
    }
};




int main()
{
    
    
    Paraon3DRenderingEngine engine;
    engine.CreateWindow(100, 100, 8);
    engine.Play();

    return 0;
}