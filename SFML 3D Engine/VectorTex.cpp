#include "stdafx.h"
#include "VectorTex.h"

using namespace tge;

void VectorTex::ScaleByW() {
	u /= w;
	v /= w;
	w = 1;
}