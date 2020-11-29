
#include "olcConsoleGameEngine.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <list>


#define PI 3.14159f

struct sObject {
	float x;
	float y;
	float vx;
	float vy;
	bool bRemove;
	olcSprite* sprite;


};

class NewConsoleFPS : public olcConsoleGameEngine {

private:
	
	float fPlayerX = 8.f;
	float fPlayerY = 8.f;
	float fPlayerA = 0.f;


	int nMapHeight = 32;
	int nMapWidth = 32;

	float fFOV = PI / 4.f;
	float fDepth = 16.f;

	float fSpeed = 5.f;

	float* fDepthBuffer = nullptr;

	std::wstring map;

	olcSprite* spriteWall = nullptr;
	olcSprite* spriteLamp;
	olcSprite* spriteFireBall;

	std::wstring sCurrentSpriteFile;

	std::list<sObject> listObjects;


public:
	NewConsoleFPS() {
		m_sAppName = L"actually working console fps";
	}

	virtual bool OnUserCreate() {


		
		sCurrentSpriteFile = L"fps_wall1.spr";


		map += L"################################";
		map += L"#.................#............#";
		map += L"#.................#............#";
		map += L"#.......##############...##....#";
		map += L"#.................#.......#....#";
		map += L"#.................#............#";
		map += L"#.................######.......#";
		map += L"#...####.#....#................#";
		map += L"#........#....#.........########";
		map += L"#......#.#....#...........######";
		map += L"#......###....#.............####";
		map += L"#........#....#................#";
		map += L"#........#....######...........#";
		map += L"#....#####.........#...........#";
		map += L"#........#.........#...........#";
		map += L"#.......#..........#...........#";
		map += L"#....###################.......#";
		map += L"#..............................#";
		map += L"#.....................##.......#";
		map += L"#....########.........##.......#";
		map += L"#....#.........................#";
		map += L"#....#............#............#";
		map += L"#....#............##########...#";
		map += L"#....#............#............#";
		map += L"#....######.......#............#";
		map += L"#.........#.......#............#";
		map += L"#.........#.......#............#";
		map += L"#.####....#.......#######......#";
		map += L"#.......................#......#";
		map += L"#.......................#......#";
		map += L"#..............................#";
		map += L"################################";

		spriteWall = new olcSprite(L"fps_wall1.spr");
		spriteLamp = new olcSprite(L"fps_lamp1.spr");
		spriteFireBall = new olcSprite(L"fps_fireball1.spr");

		fDepthBuffer = new float[ScreenWidth()];

		listObjects = {
			{8.5f, 8.5f, 0.f, 0.f, false, spriteLamp},
			{7.5f, 6.5f, 0.f, 0.f, false, spriteLamp},
			{10.5f, 5.5f, 0.f, 0.f, false, spriteLamp}
		};

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		//Controls
		//Handle CCW rotation

		if (m_keys[L'A'].bHeld)
			fPlayerA -= (0.8) * fElapsedTime;

		if (m_keys[L'D'].bHeld)
			fPlayerA += (0.8) * fElapsedTime;

		if (m_keys[L'W'].bHeld) {
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;

			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#') {
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		if(m_keys[L'E'].bHeld) {
			fPlayerX += cos(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY -= sin(fPlayerA) * fSpeed * fElapsedTime;

			if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#') {
				fPlayerX -= cos(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY += sin(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		if (m_keys[L'Q'].bHeld) {
			fPlayerX -= cos(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY += sin(fPlayerA) * fSpeed * fElapsedTime;

			if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#') {
				fPlayerX += cos(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY -= sin(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		if (m_keys[L'S'].bHeld) {
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;

			if (map[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#') {
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		if (m_keys[VK_SPACE].bReleased) {
			sObject o;
			o.x = fPlayerX;
			o.y = fPlayerY;

			float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
			o.vx = sinf(fPlayerA + fNoise) * 8.f;
			o.vy = cosf(fPlayerA + fNoise) * 8.f;

			o.sprite = spriteFireBall;
			o.bRemove = false;
			listObjects.push_back(o);
		}

		for (int x = 0; x < ScreenWidth(); x++) {

			//For each column, claulate the projected ray angle into world space
			//(fPlayerA - fFOV / 2.f) -> najmniejszy, lewy kat widzenia
			// ((float)x / (float)ScreenWidth()) ulamek FOV, ktorym jest obecna kolumna pixeli
			//obliczenia w World space
			float fRayAngle = (fPlayerA - fFOV / 2.f) + ((float)x / (float)ScreenWidth()) * fFOV;

			float fDistanceToWall = 0;
			float fStepSize = 0.01f;
			bool bHitWall = false;
			bool bBoundary = false;

			//wait, czy to nie powino być x = cos, y = sin??
			//Nie bo najwyrazniej mmay obrone stopnie, tzn prawo to nie jest 0 sotpni ale 90
			float fEyeX = sinf(fRayAngle); //Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;

			while (!bHitWall && fDistanceToWall < fDepth) {
				fDistanceToWall += fStepSize;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				//Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
					bHitWall = true; //Just set distance to maximum dpeth
					fDistanceToWall = fDepth;
					
				}
				else {
					//Ray is inbound so test to see if the ray cell is a wall block
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#') {
						bHitWall = true;

						//Determine where ray hit wall. Break block boundary
						//int 4 lin segments
						float fBlockMidX = (float)nTestX + 0.5f;
						float fBlockMidY = (float)nTestY + 0.5f;

						//location of point of collision of ray with wall
						float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
						float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

						//angle bewtwen those two points, the quadrant where the coliison happend
						float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

						//Roatationg the quadrant so it aligns wit walls - *0.25 or *0.75
						//Test which side of block we hit cos it determines the way we sample texture
						if (fTestAngle >= -PI* 0.25f && fTestAngle < PI * 0.25f)
							fSampleX = fTestPointY - (float)nTestY;
						if (fTestAngle >= PI * 0.25f && fTestAngle < PI * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle < -PI * 0.25f && fTestAngle >= -PI * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle >= PI * 0.75f || fTestAngle < -PI * 0.75f)
							fSampleX = fTestPointY - (float)nTestY;



					}
				}
			}



			//y positions of ceiling and flor
			int nCeiling = (float)(ScreenHeight() / 2.f) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;

			//Update DepthBuffer
			fDepthBuffer[x] = fDistanceToWall;

			{
				/*
				//Shade wall based on distance
				short nShade = L' ';
				if (fDistanceToWall <= fDepth / 4.f)			nShade = 0x2588; //Very close
				else if (fDistanceToWall < fDepth / 3.f)		nShade = 0x2593;
				else if (fDistanceToWall < fDepth / 2.f)		nShade = 0x2592;
				else if (fDistanceToWall < fDepth)				nShade = 0x2591;
				else											nShade = L' ';	//Too far away

				if (bBoundary)	nShade = L' ';
				*/
			}
			

			for (int y = 0; y < ScreenHeight(); y++) {
				if (y <= nCeiling) //Sufit
					Draw(x, y, L' ');
				else if (y > nCeiling && y <= nFloor) { // Sciany

					if (fDistanceToWall < fDepth) {
						//THe perercantge distance bwtenn ceiling and floor
						float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
						Draw(x, y, spriteWall->SampleGlyph(fSampleX, fSampleY), spriteWall->SampleColour(fSampleX, fSampleY));
					}
					else {
						Draw(x, y, PIXEL_SOLID, 0);
					}

					
				}
					

					

				else { //Podloga
					/*
					//Shade floor based on distance
					float b = 1.f - (((float)y - ScreenHeight() / 2.f) / ((float)ScreenHeight() / 2.f));
					//
					if (b < 0.25)			nShade = L'#';
					else if (b < 0.5)		nShade = L'x';
					else if (b < 0.75)		nShade = L'.';
					else if (b < 0.9)		nShade = L'-';
					else					nShade = L' ';

					Draw(x, y, nShade);
					*/
					Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
					
				}
				
			}

			

		}

		//Update adn Draw Objects
		for (auto& object : listObjects) {
			//update object physics
			object.x += object.vx * fElapsedTime;
			object.y += object.vy * fElapsedTime;

			//Check if object is indide wall  set flag for removal
			if (map.c_str()[(int)object.x * nMapWidth + (int)object.y] == '#')
				object.bRemove = true;

			//Can object be seen?
			float fVecX = object.x - fPlayerX;
			float fVecY = object.y - fPlayerY;
			float fDistanceFromPlayer = sqrtf(fVecX * fVecX + fVecY * fVecY);

			float fEyeX = sinf(fPlayerA);
			float fEyeY = cosf(fPlayerA);

			//subtraction of 2 atan2 to give angle between 2 vectores but not excatly
			float fObjectAngle = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);
			if (fObjectAngle < -PI)
				fObjectAngle += 2.f * PI;
			if (fObjectAngle > PI)
				fObjectAngle -= 2.f * PI;
			//^ this is to give result bewten 0 and 2 PI

			bool bInPlayerFOV = fabs(fObjectAngle) < fFOV / 2.f;

			if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < fDepth) {

				//Calcualtiung hgeight of the object
				float fObjectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceFromPlayer);
				float fObjectFloor = ScreenHeight() - fObjectCeiling;
				float fObjectHeight = fObjectFloor - fObjectCeiling;
				float fObjectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
				float fObjectWidth = fObjectHeight / fObjectAspectRatio;
				float fMiddleOfObject = (0.5f * (fObjectAngle / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();
				
				

				for (float lx = 0; lx < fObjectWidth; lx++) {
					for (float ly = 0; ly < fObjectHeight; ly++) {

						//Sampling in normalized spcae
						float fSampleX = lx / fObjectWidth;
						float fSampleY = ly / fObjectHeight;

						wchar_t c = object.sprite->SampleGlyph(fSampleX, fSampleY);
						int nObjectColumn = (int)(fMiddleOfObject + lx - (fObjectWidth / 2.f));
						if (nObjectColumn >= 0 && nObjectColumn < ScreenWidth()) {

							if (c != L' ' && fDepthBuffer[nObjectColumn] >= fDistanceFromPlayer) {
								Draw(nObjectColumn, fObjectCeiling + ly, c, object.sprite->SampleColour(fSampleX, fSampleY));
								fDepthBuffer[nObjectColumn] = fDistanceFromPlayer;
							}
								
						}
					}
				}
			}
		}

		//Remove dead objects from object list
		listObjects.remove_if([](sObject& o) {return o.bRemove; });
		

		//Display Map
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++) 
				Draw(nx + 1, ny + 1, map[ny * nMapWidth + nx]);
			

		
		Draw(1 + (int)fPlayerY, 1 + (int)fPlayerX, L'P');

		return true;

	}


};



int main() {

	NewConsoleFPS game;
	game.ConstructConsole(320, 240, 4, 4);
	game.Start();


	return 0;
}







