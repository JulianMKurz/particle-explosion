#pragma once

using namespace std;

#include "Shapes.h"

class ObjectMover {

public:

	ObjectMover();
	ObjectMover(Particle p, glm::vec3 direction);

	Particle getParticle();
	glm::vec3 getDirection();

	Particle particle;
	glm::vec3 v; 


};