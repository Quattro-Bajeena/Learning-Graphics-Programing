#include <iostream>
#include <vector>
#include <string>

#include "olcConsoleGameEngine.h"



class RacingGame : public olcConsoleGameEngine {
public:
	RacingGame() {
		m_sAppName = L'Formula OLC';
	}

private:
	//From -1 to 1
	float fCarPos = 0.f;
	float fDistance = 0.f;

	//From 0 to 1
	float fSpeed = 0.f;

	float fCurvature = 0.f;
	float fTrackCurvature = 0.f;
	float fPlayerCurvature = 0.f;

	float fTrackDistance = 0.f;

	float fCurrentLapTime = 0.f;

	std::vector<std::pair<float, float>> vecTrack; //curavture, distance
	std::list<float> listLapTimes;

protected:
	virtual bool OnUserCreate() override{
		vecTrack.push_back(std::make_pair(0.f, 10.f)); //Short section for start/ finish line
		vecTrack.push_back(std::make_pair(0.f, 200.f));
		vecTrack.push_back(std::make_pair(0.3f, 200.f));
		vecTrack.push_back(std::make_pair(0.5f, 400.f));
		vecTrack.push_back(std::make_pair(-0.1f, 50.f));
		vecTrack.push_back(std::make_pair(0.f, 200.f));
		vecTrack.push_back(std::make_pair(-0.7f, 250.f));
		vecTrack.push_back(std::make_pair(0.8f, 100.f));
		vecTrack.push_back(std::make_pair(0.5f, 100.f));
		vecTrack.push_back(std::make_pair(0.0f, 400.f));

		for (auto t : vecTrack)
			fTrackDistance += t.second;

		listLapTimes = { 0, 0, 0, 0, 0 };

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) override {
		
		
		
		//Input
		if (m_keys[VK_UP].bHeld) {
			fSpeed += 2.0f * fElapsedTime;
		}
			
		else {
			fSpeed -= 1.0f * fElapsedTime;
		}

		int nCarDirection = 0;


		if (m_keys[VK_LEFT].bHeld) {
			fPlayerCurvature -= 0.7f * fElapsedTime;
			nCarDirection = -1;
		}
			

		if (m_keys[VK_RIGHT].bHeld){
			fPlayerCurvature += 0.7f * fElapsedTime;
			nCarDirection = 1;
		}
			

		if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.8f)
			fSpeed -= 5.0f * fElapsedTime;

		if (fSpeed < 0.f) fSpeed = 0.f;
		if (fSpeed > 1.f) fSpeed = 1.f;

		//move car along tack according to car speed
		fDistance += (70.f * fSpeed) * fElapsedTime;

		//Get point on track
		float fOffset = 0;
		int nTrackSection = 0;

		fCurrentLapTime += fElapsedTime;
		if (fDistance >= fTrackDistance) {
			fDistance -= fTrackDistance;
			listLapTimes.push_front(fCurrentLapTime);
			listLapTimes.pop_back();
			fCurrentLapTime = 0.f;
		}
			

		//Find postion on track (could optimise)
		while (nTrackSection < vecTrack.size() && fOffset <= fDistance) {
			fOffset += vecTrack[nTrackSection].second;
			nTrackSection++;
		}

		float fTargetCurvature = vecTrack[nTrackSection - 1].first;

		//Linear interpolation?
		float fTrackCurvatureDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;
		fCurvature += fTrackCurvatureDiff;

		fTrackCurvature += (fCurvature)*fElapsedTime * fSpeed;

		

		//Draw Sky
		for (int y = 0; y < ScreenHeight() / 2; y++)
			for (int x = 0; x < ScreenWidth(); x++)
				Draw(x, y, y < ScreenHeight() / 4 ? PIXEL_HALF : PIXEL_SOLID, FG_DARK_BLUE);

		//Draw Scenery - our hillls are a rectified sine wave, where the plhase is adjusted by the
		// accumulated track curvature
		for (int x = 0; x < ScreenWidth(); x++) {
			int nHillHeight = (int)(fabs(sinf(x * 0.01f + fTrackCurvature) * 16.f));
			for (int y = (ScreenHeight() / 2) - nHillHeight; y < ScreenHeight() / 2; y++)
				Draw(x, y, PIXEL_SOLID, FG_DARK_YELLOW);
		}


		for (int y = 0; y < ScreenHeight() / 2; y++) {
			for (int x = 0; x < ScreenWidth(); x++) {

				//perspective scaling - 0 in the middle of screen, 1 at the bottom
				float fPerspective = (float)y / (ScreenHeight() / 2.f);

				//Working in normalized space, left edge = 0, right = 1
				float fMiddlePoint = 0.5f + fCurvature * powf((1.f - fPerspective), 3);

				//Roaud is going from 90% to 10% of screen width
				float fRoadWidth = 0.1f + 0.8f * fPerspective;
				float fClipWidth = fRoadWidth * 0.15f;

				//Road is symetrical
				fRoadWidth *= 0.5f;

				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();

				
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();


				int nRoadColour = (nTrackSection - 1) == 0 ? FG_WHITE : FG_GREY;


				//so we can draw in the bottom part of the screen
				int  nRow = ScreenHeight() / 2 + y;

				int nGrassColour = sinf(20.f * powf(1.f - fPerspective, 3) + fDistance * 0.1f) > 0.f ? FG_GREEN : FG_DARK_GREEN;
				//80 > 20, bigger frequncy, denser lines
				int nClipColour = sinf(80.f * powf(1.f - fPerspective, 3) + fDistance ) > 0.f ? FG_RED : FG_WHITE;

				//Grass
				if ((x >= 0 && x < nLeftGrass) || (x >= nRightGrass && x < ScreenWidth() ))
					Draw(x, nRow, PIXEL_SOLID, nGrassColour);

				//Clip Pobocze
				if ((x >= nLeftGrass && x < nLeftClip) || (x >= nRightClip && x < nRightGrass))
					Draw(x, nRow, PIXEL_SOLID, nClipColour);

				//Road
				if (x >= nLeftClip && x < nRightClip)
					Draw(x, nRow, PIXEL_SOLID, nRoadColour);

			}
		}

		//Draw a Car



		//Scaling to pixels space, from (-1 to 1)
		// -7 -> width of sproite of car

		fCarPos = fPlayerCurvature - fTrackCurvature;
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.f) - 7;
		

		switch (nCarDirection) {
		case 0:
			DrawStringAlpha(nCarPos, 80, L"   ||####||   ");
			DrawStringAlpha(nCarPos, 81, L"      ##      ");
			DrawStringAlpha(nCarPos, 82, L"     ####     ");
			DrawStringAlpha(nCarPos, 83, L"     ####     ");
			DrawStringAlpha(nCarPos, 84, L"|||  ####  |||");
			DrawStringAlpha(nCarPos, 85, L"|||########|||");
			DrawStringAlpha(nCarPos, 86, L"|||  ####  |||");
			break;

		case 1:
			DrawStringAlpha(nCarPos, 80, L"      //####//");
			DrawStringAlpha(nCarPos, 81, L"         ##   ");
			DrawStringAlpha(nCarPos, 82, L"       ####   ");
			DrawStringAlpha(nCarPos, 83, L"      ####    ");
			DrawStringAlpha(nCarPos, 84, L"///  ####//// ");
			DrawStringAlpha(nCarPos, 85, L"//#######///O ");
			DrawStringAlpha(nCarPos, 86, L"/// #### //// ");
			break;

		case -1:
			DrawStringAlpha(nCarPos, 80, L"\\\\####\\\\      ");
			DrawStringAlpha(nCarPos, 81, L"   ##         ");
			DrawStringAlpha(nCarPos, 82, L"   ####       ");
			DrawStringAlpha(nCarPos, 83, L"    ####      ");
			DrawStringAlpha(nCarPos, 84, L" \\\\\\\\####  \\\\\\");
			DrawStringAlpha(nCarPos, 85, L" O\\\\\\#######\\\\");
			DrawStringAlpha(nCarPos, 86, L" \\\\\\\\ #### \\\\\\");
			break;
		}

		

		DrawString(0, 0, L"Distance: " + std::to_wstring(fDistance));
		DrawString(0, 1, L"Target Curvature: " + std::to_wstring(fCurvature));
		DrawString(0, 2, L"Player Curvature: " + std::to_wstring(fPlayerCurvature));
		DrawString(0, 3, L"Player Speed: " + std::to_wstring(fSpeed));
		DrawString(0, 4, L"Track Curvature: " + std::to_wstring(fTrackCurvature));

		auto disp_time = [](float t) {
			int nMinutes = t / 60.f;
			int nSeconds = t - (nMinutes * 60.f);
			int nMiliseconds = (t - (float)nSeconds) * 1000.f;

			return std::to_wstring(nMinutes) + L"." + std::to_wstring(nSeconds) + L":" + std::to_wstring(nMiliseconds);
		};

		DrawString(10, 8, disp_time(fCurrentLapTime));

		//Display last 5 lap times
		int j = 10;
		for (auto l : listLapTimes) {
			DrawString(10, j, disp_time(l));
			j++;
		}

		return true;
	}
};


int main() {
	RacingGame game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}