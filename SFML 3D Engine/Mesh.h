#pragma once
#include "Triangle.h"

namespace tge {

class Mesh
{
public:
	std::vector<Triangle> tris;

	Mesh();
	Mesh(std::vector<Triangle> tris);


	bool LoadFromObjectFile(std::string sFilename, bool bHasTexture = false);

};
}



