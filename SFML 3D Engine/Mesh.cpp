#include "stdafx.h"
#include "Mesh.h"

using namespace tge;

tge::Mesh::Mesh() {};

tge::Mesh::Mesh(std::vector<Triangle> tris)
	:tris(tris)
{}

std::vector<Triangle> tge::Mesh::GetTris() const
{
	return tris;
}

bool tge::Mesh::LoadFromObjectFile(std::string sFilename, bool bHasTexture) {
	std::ifstream f(sFilename);
	if (!f.is_open())
		return false;

	//local chache of verts
	std::vector<Vector3D> verts;
	std::vector<VectorTex> texs;

	while (!f.eof()) {
		char line[128];
		f.getline(line, 128);

		std::stringstream ss;
		ss << line;
		std::string junk;
		if (line[0] == 'v') {
			if (line[1] == 't') {
				VectorTex tex;

				ss >> junk >> tex.u >> tex.v;
				texs.push_back(tex);
			}
			else {
				Vector3D v;
				ss >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

		}

		if (bHasTexture == false) {
			if (line[0] == 'f') {
				int f[3];
				ss >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back(
					{ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] }
				);
			}
		}
		else {
			if (line[0] == 'f') {
				ss >> junk;

				std::string tokens[6];
				int nTokenCount = -1;


				while (!ss.eof())
				{
					char c = ss.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append(1, c);
				}

				tokens[nTokenCount].pop_back();


				tris.push_back(Triangle(verts[std::stoi(tokens[0]) - 1], verts[std::stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
					texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1]));
			}
		}

	}

	return true;
}

void tge::Mesh::LoadTexture(std::string filename)
{
	texture.loadFromFile(filename);
}

const sf::Image& tge::Mesh::GetTexture() const
{
	return texture;
}


