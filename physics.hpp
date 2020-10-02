#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#define real float

typedef struct {
	real x;
	real y;
} Vec2;

class Body {
	public:
	Vec2 pos;
	Vec2 vel;
	real width;
	real height;
};

class ForceField {
	public:
	virtual void updatePosition(Body*, long) = 0;
	virtual void updateVelocity(Body*, long) = 0;
};

class ConstantForceField: public ForceField {
	public:
	Vec2 force;

	void updatePosition(Body*, long) override;
	void updateVelocity(Body*, long) override;
};

// more force field definitions here

void updateCollisions(Body** bodies, int nbodies);

#endif
