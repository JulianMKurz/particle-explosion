#include "ObjectMover.h"

ObjectMover::ObjectMover(Particle p, glm::vec3 direction){

	particle = p;
	v = direction;

}

ObjectMover::ObjectMover() {
}

Particle ObjectMover::getParticle(){

	return particle;
}

glm::vec3 ObjectMover::getDirection(){
	
	return v;
}


