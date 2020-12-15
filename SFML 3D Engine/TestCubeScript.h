#pragma once
#include "GameObject.h"

namespace tge {

class TestCubeScript : public ObjectComponent {

public:
	TestCubeScript(GameObject& _gameObject)
		: ObjectComponent(_gameObject){

	}

	void OnStart() override {
		transform.position.z = 5;
	}

	void Update(float deltaTime) override {

		transform.rotation.x +=  0.2 * deltaTime;
		transform.rotation.y += 0.5 * deltaTime;
		transform.rotation.z += 0.7 * deltaTime;
		

	}
};

}

