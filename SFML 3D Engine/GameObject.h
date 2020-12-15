#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "ObjectComponent.h"

namespace tge {
;



class GameObject
{
private:
	
	

std::vector< std::unique_ptr<ObjectComponent>> components;
	

public:
	Transform transform;
	Mesh mesh;

public:
	GameObject();
	GameObject(const Mesh& _mesh);

	void AddMesh(const Mesh& mesh);
	void Update(float deltaTime);

	
	template<typename T>
	void AddComponent() {
		components.emplace_back(std::make_unique<T>(*this));
		components.back()->OnStart();
		
		
	}

	
};

}


