#include "stdafx.h"
#include "ObjectComponent.h"
#include "GameObject.h"



tge::ObjectComponent::ObjectComponent(GameObject& _gameObject)
	:gameObject(_gameObject), transform(_gameObject.transform)
{
	
}
