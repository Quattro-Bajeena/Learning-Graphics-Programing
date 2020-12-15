#pragma once
#include "Transform.h"



namespace tge {
;
class GameObject;


class ObjectComponent
{

protected:
	GameObject& gameObject;
	Transform& transform;


public:
	ObjectComponent(GameObject& _gameObject);

	virtual void Update(float deltaTime) = 0;
	virtual void OnStart() = 0;
};


}



