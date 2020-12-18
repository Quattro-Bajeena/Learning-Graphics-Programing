#pragma once
#include "Triangle.h"

namespace tge {

class Mesh
{
private:
	std::vector<Triangle> tris;
	sf::Image texture;


public:
	

	Mesh();
	Mesh(std::vector<Triangle> tris);

	std::vector<Triangle> GetTris() const;

	bool LoadFromObjectFile(std::string sFilename, bool bHasTexture = false);
	void LoadTexture(std::string filename);

	const sf::Image& GetTexture() const;

};
}



