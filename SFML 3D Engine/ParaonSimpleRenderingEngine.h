#pragma once
#include <iostream>

#include "SFML/Graphics.hpp"
#include "SFML/Main.hpp"
#include "SFML/Window.hpp"

#include "math_utilities.h"

class ParaonSimpleRenderingEngine {

	int width, height;
	int pixelSize;

	//Rendering
	sf::RenderWindow* window;
	sf::Texture screenTexture;
	sf::Sprite screenSprite;

	sf::Color* pixels;
	sf::Uint8* pixelsRaw;


	//Utility
	sf::Clock clock;

	

	

	void Render() {

		
		for (int i = 0; i < width * height; i++) {
			pixelsRaw[i * 4] = pixels[i].r;
			pixelsRaw[i * 4 + 1] = pixels[i].g;
			pixelsRaw[i * 4 + 2] = pixels[i].b;
			pixelsRaw[i * 4 + 3] = pixels[i].a;
		}

		
		screenTexture.update(pixelsRaw, width, height, 0, 0);
		
		
        window->clear();
        window->draw(screenSprite);
        window->display();
	}

	
protected:
	virtual void Initialize() = 0;
	virtual void Update(float deltaTime) = 0;

	void EndGame() {
		window->close();
	}

	//Getters
	int ScreenWidth() const {
		return width;
	}
	int ScreenHeight() const {
		return height;
	}

	sf::Vector2i ScreenSize() const {
		return sf::Vector2i(width, height);
	}



	//Graphics
	void DrawPixel(int x, int y, sf::Color color) {
		pixels[y * width + x] = color;
	}
	void DrawPixel(sf::Vector2i point, sf::Color color) {
		pixels[point.y * width + point.x] = color;
	}

	void DrawLine(sf::Vector2i start, sf::Vector2i end, sf::Color color) {
		int x, y;
		int xEnd, yEnd;
		int deltaX = end.x - start.x;
		int deltaY = end.y - start.y;

		


		if (abs(deltaY) <= abs(deltaX)) {

			int dstX = 2 * deltaY - deltaX;
			
			

			if(deltaX >= 0){
				x = start.x; y = start.y; xEnd = end.x;
			}
			else {
				x = end.x; y = end.y; xEnd = start.x;
			}

			DrawPixel(x, y, color);

			for (int i = 0; x < xEnd; i++) {
				x += 1;

				if (dstX <= 0)
					dstX += 2 * abs(deltaY);
				else {
					if ((deltaX < 0 && deltaY < 0) || (deltaX > 0 && deltaY > 0)) 
						y += 1;
					else 
						y -= 1;
					
					dstX += 2 * (abs(deltaY) - abs(deltaX));
				}
				DrawPixel(x, y, color);
			}

		}
		else {

			int dstY = 2 * deltaX - deltaY;

			if (deltaY >= 0)
			{
				x = start.x; y = start.y; yEnd = end.y;
			}
			else {
				x = end.x; y = end.y; yEnd = start.y;
			}

			DrawPixel(x, y, color);

			for (int i = 0; y < yEnd; i++) {
				y += 1;
				if (dstY <= 0)
					dstY += 2 * abs(deltaX);
				else {
					if ((deltaX < 0 && deltaY < 0) || (deltaX > 0 && deltaY > 0))
						x += 1;
					else
						x -= 1;
					dstY += 2 * (abs(deltaX) - abs(deltaY));
				}
				DrawPixel(x, y, color);
			}
		}
		
		
	}

	void DrawTriangle(sf::Vector2i point1, sf::Vector2i point2, sf::Vector2i point3, sf::Color color) {

	}

	void FillTriangle(sf::Vector2i point1, sf::Vector2i point2, sf::Vector2i point3, sf::Color color) {

	}

	
	

public:
	ParaonSimpleRenderingEngine() {

	}


	void CreateWindow(int width, int height, int pixelSize = 1) 
	{
		this->width = width;
		this->height = height;
		this->pixelSize = pixelSize;
		window = new sf::RenderWindow(sf::VideoMode(width * pixelSize, height * pixelSize), "Paraon Rendring Engine");
		window->setVerticalSyncEnabled(true);

		screenTexture.create(width, height);
		screenSprite.setTexture(screenTexture);
		screenSprite.scale(pixelSize, pixelSize);

		pixels = new sf::Color[width * height];
		pixelsRaw = new sf::Uint8[width * height * pixelSize * pixelSize * 4];

		for (int i = 0; i < width * height; i ++) {
			pixels[i] = sf::Color::Black;
		}
	}

	void Play() {
		if (window != nullptr) {
			Initialize();

			while (window->isOpen()) {

				sf::Event event;
				while (window->pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						window->close();
				}

				float deltaTime = clock.restart().asSeconds();
				Update(deltaTime);
				Render();
			}
		}
		else {
			std::cout << "Create Window!\n";
		}
	
	}
};