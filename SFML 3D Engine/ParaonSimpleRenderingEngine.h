#pragma once
#include "math_utilities.h"

class ParaonSimpleRenderingEngine {
private:
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
		PixelsClear();
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
	void PixelsClear() {
		for (int i = 0; i < width * height; i++)
			pixels[i] = sf::Color::Black;
	}

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
		DrawLine(point1, point2, color);
		DrawLine(point2, point3, color);
		DrawLine(point3, point1, color);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void FillTriangle(sf::Vector2i point1, sf::Vector2i point2, sf::Vector2i point3, sf::Color color) {
		int x1 = point1.x; int y1 = point1.y; 
		int x2 = point2.x; int y2 = point2.y;
		int x3 = point3.x; int y3 = point3.y;

		auto SWAP = [](int& x, int& y) { int t = x; x = y; y = t; };
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) DrawPixel(i, ny, color); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
		if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
		if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SWAP(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SWAP(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
										 // Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SWAP(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
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
		//window->setVerticalSyncEnabled(true);

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