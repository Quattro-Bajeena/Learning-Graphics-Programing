#include "stdafx.h"
#include "GameObject.h"
#include "ObjectComponent.h"

namespace tge {


GameObject::GameObject()
{
}

GameObject::GameObject(const Mesh& _mesh)
	: mesh(_mesh)
{
}

void GameObject::AddMesh(const Mesh& mesh)
{
	this->mesh = mesh;
}



void GameObject::Update(float deltaTime)
{
	for (std::unique_ptr<ObjectComponent>& component : components) {
		component->Update(deltaTime);
	}
}




}