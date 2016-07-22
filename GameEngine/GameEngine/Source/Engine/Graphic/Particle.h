/******************************************************************************/
/*!
\file   Particle.h
\author Jeong Juyong
\par    email: jeykop14\@gmail.com
\date   2016/06/27(yy/mm/dd)

\description
Contains Particle and Emitter's class and members

All content (C) 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <vector>
#include <algorithm>
#include "Sprite.h"

#define MaxParticle 100

enum ParticleMode{ NORMAL, FIRE, SMOKE, EXPLOSION };

class Emitter;
//! Particle class
class Particle : public Sprite
{
public:
	Particle(Emitter* parent = 0, int index = 0);
	virtual ~Particle(void);
	int m_index;
	float m_life;
	float m_dirAngle;
	float m_slow;
	float m_speed;
	vec3 velocity; 
	Emitter *m_parent;

private:
	
};

//! Set of particles
class Emitter : public Sprite
{

public:

	Emitter(const int id = 0, ObjectManager* obm = 0);
	virtual ~Emitter(void);

	// Scale functions
	virtual void SetScale(const vec3& scale);
	virtual vec3 GetScale(void) const;

	// Mode functions
	void SetMode(ParticleMode mode);
	ParticleMode GetMode(void) const;

	// Quantity of particle of emitter
	int GetNumOfParticle(void) const;
	
	// direction functions
	void SetDirection(const vec3& dir);
	vec3 GetDirection(void) const;

	// speed function
	void SetSpeed(float speed);
	float GetSpeed(void) const;

	// boundary functions
	void SetBoundary(float range);
	float GetBoundary(void) const;

	// Renderer functions
	void Update(const int index);
	void Render(const int index);
	void Refresh(const int index);

	// Particle container
	Particle ParticlesContainer[MaxParticle];

	//ParticleList& GetParticleContainer(void);

private:

	//ParticleList ParticlesContainer;
	int m_quantity;
	float m_boundary;
	float m_emitterSpd;

	vec3 m_emitterScl;
	vec3 m_emitterDir;
	ParticleMode m_emitterMode;

};

#endif //_PARTICLE_H_
