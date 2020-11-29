#include <iostream>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <algorithm>

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.f;
float fPlayerY = 8.f;
float fPlayerA = 0.f;


int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.f;
float fDepth = 16.f;

int not_main() {

	// Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;



	std::wstring map;

	map += L"################";
	map += L"#........#.....#";
	map += L"###......#.....#";
	map += L"#........#.....#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#########......#";
	map += L"#.............##";
	map += L"#..............#";
	map += L"######.........#";
	map += L"#....#.........#";
	map += L"#....#.........#";
	map += L"#..............#";
	map += L"#........#######";
	map += L"#..............#";
	map += L"################";

	//time points
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	while (true) {
		tp1 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();


		//Controls
		//Handle CCW rotation

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (0.8) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (0.8) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			fPlayerX += sinf(fPlayerA) * 5.f * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * 5.f * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX -= sinf(fPlayerA) * 5.f * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * 5.f * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			fPlayerX -= sinf(fPlayerA) * 5.f * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * 5.f * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX += sinf(fPlayerA) * 5.f * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * 5.f * fElapsedTime;
			}
		}


		for (int x = 0; x < nScreenWidth; x++) {

			//For each column, claulate the projected ray angle into world space
			//(fPlayerA - fFOV / 2.f) -> najmniejszy, lewy kat widzenia
			// ((float)x / (float)nScreenWidth) ulamek FOV, ktorym jest obecna kolumna pixeli
			//obliczenia w World space
			float fRayAngle = (fPlayerA - fFOV / 2.f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bHitWall = false;
			bool bBoundary = false;

			//wait, czy to nie powino być x = cos, y = sin??
			//Nie bo najwyrazniej mmay obrone stopnie, tzn prawo to nie jest 0 sotpni ale 90
			float fEyeX = sinf(fRayAngle); //Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth) {
				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				//Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
					bHitWall = true; //Just set distance to maximum dpeth
					fDistanceToWall = fDepth;
				}
				else {
					//Ray is inbound so test to see if the ray cell is a wall block
					if (map[nTestY * nMapWidth + nTestX] == '#') {
						bHitWall = true;

						//CHecking if hit a corner of a segment, so we can display lines between them
						std::vector<std::pair<float, float>> p; //distance, dot;
						for (int tx = 0; tx < 2; tx++) {
							for (int ty = 0; ty < 2; ty++) {
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								//vy, vy, - vector from corner to player
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(std::make_pair(d, dot));

							}
						}

						//Sort pairs from closes to farthest
						std::sort(p.begin(), p.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right)
							{
								return left.first < right.first;
							}
						);
						float fBound = 0.05f;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;

					}
				}
			}

			//y positions of ceiling and flor
			int nCeiling = (float)(nScreenHeight / 2.f) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.f)			nShade = 0x2588; //Very close
			else if (fDistanceToWall < fDepth / 3.f)		nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.f)		nShade = 0x2592;
			else if (fDistanceToWall < fDepth)				nShade = 0x2591;
			else											nShade = ' ';	//Too far away

			if (bBoundary)	nShade = ' ';

			for (int y = 0; y < nScreenHeight; y++) {
				if (y < nCeiling) //Sufit
					screen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor) // Sciany
					screen[y * nScreenWidth + x] = nShade;
				else { //Podloga
					float b = 1.f - (((float)y - nScreenHeight / 2.f) / ((float)nScreenHeight / 2.f));
					//
					if (b <= 0.25)			nShade = '#'; 
					else if (b < 0.5)		nShade = 'x';
					else if (b < 0.75)		nShade = '.';
					else if (b < 0.9)		nShade = '-';
					else					nShade = ' ';

					screen[y * nScreenWidth + x] = nShade;
				}
			}
				
					
		}

		//Display stats
		wprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

		for(int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++) {
				screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
			}

		screen[(int)fPlayerY + 1 * nScreenWidth + (int)fPlayerX] = 'P';

		// Display Frame
		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}


	return 0;
}

		
	



	
	