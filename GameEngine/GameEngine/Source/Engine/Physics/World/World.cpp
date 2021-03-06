/******************************************************************************/
/*!
\file   World.cpp
\author Jeong Juyong
\par    email: jeykop14\@gmail.com
\date   2016/06/19(yy/mm/dd)

\description
Contains World's class member function

All codes are written by Jaykop Jeong...
*/
/******************************************************************************/

#include "World.h"
#include <unordered_map>
#include "../RigidBody/RigidBody.h"
#include "../../Graphic/Sprite/Sprite.h"
#include "../../ObjectManager/ObjectManager.h"

/******************************************************************************/
/*!
\brief - World's costructor
*/
/******************************************************************************/
World::World(void)
:mtd(vec3()), collided_edge(vec3())
{
	new_speed[0] = new_speed[1] = vec3();
}

/******************************************************************************/
/*!
\brief - World's destructor
*/
/******************************************************************************/
World::~World(void)
{}

/******************************************************************************/
/*!
\brief - World's init function
*/
/******************************************************************************/
void World::Init(const ObjectList& objList)
{
	UNREFERENCED_PARAMETER(objList);
	// collided_edge = mtd = vec3();
	// new_speed[0] = new_speed[1] = vec3();
}

/******************************************************************************/
/*!
\brief - World's update function
*/
/******************************************************************************/
void World::Update(const ObjectList& objList)
{
	// Works each bodies' physics
	for (auto it1 = objList.begin(); it1 != objList.end(); ++it1)
	{
		// 1. If sprite has body and activated body to work.
		if (it1->second->HasRigidBody() && 
			it1->second->GetRigidBody()->GetMoveToggle())
		{
			// Work basic motion
			BodyPipeline(it1->second);
			
			// 2. if 2st sprite's colliders to be worked, check colliders
			if (it1->second->GetRigidBody()->GetColliderToggle())
			{
				// Refresh collision-with info
				it1->second->GetRigidBody()->CheckCollided(false);
				it1->second->GetRigidBody()->SetCollisionWith(nullptr);

				auto new_begin = it1;
				for (auto it2 = new_begin; it2 != objList.end(); ++it2)
				{
					// 3. If both objs are differenct and both bodies has own body, 
					// activated body and collider body,
					if (it1 != it2 && it2->second->HasRigidBody() &&
						it2->second->GetRigidBody()->GetColliderToggle())
					{					
						// 4. then check the colliders.
						// Check two sprites' collision status
						bool collisionIntersect = CollisionIntersect(it1->second, it2->second);
						if (collisionIntersect)
						{
							// Collision response
							if (it1->second->GetRigidBody()->GetResponseToggle() ||
								it2->second->GetRigidBody()->GetResponseToggle())
									CollisionResponse(it1->second, it2->second);

							// Refresh the collision with info
							CollisionRelation(it1->second, it2->second);

						}// 4. Check the colliders
					}// 3. Has Rigid Body, 2 toggles to work
				}
			}// 2. Collider Toggle
		}// 1. Body Toggle
	}
}

/******************************************************************************/
/*!
\brief - World's shutdown function
*/
/******************************************************************************/
void World::Shutdown(const ObjectList& objList)
{
	UNREFERENCED_PARAMETER(objList);
}

/******************************************************************************/
/*!
\brief - World's Body Pipeline

\param spt - sprite to work
*/
/******************************************************************************/
void World::BodyPipeline(Sprite* spt)
{
	//Set directed angle 
	vec3 norm_velocity = spt->GetRigidBody()->GetVelocity().Normalize();
	spt->GetRigidBody()->SetDirectionAngle(Math::RadToDeg(acosf(norm_velocity.DotProduct(vec3(1, 0, 0)))));
	if (norm_velocity.y < 0)
		spt->GetRigidBody()->SetDirectionAngle(360 - (spt->GetRigidBody()->GetDirectionAngle()));

	//Implement frction
	spt->GetRigidBody()->SetSpeed(spt->GetRigidBody()->GetSpeed() -
		spt->GetRigidBody()->GetFriction());

	//Control the meaningless force
	if (spt->GetRigidBody()->GetSpeed().x <= 0)
		spt->GetRigidBody()->SetSpeed(vec3(0.f, spt->GetRigidBody()->GetSpeed().y));
	if (spt->GetRigidBody()->GetSpeed().y <= 0)
		spt->GetRigidBody()->SetSpeed(vec3(spt->GetRigidBody()->GetSpeed().x, 0.f));

	//Update body's speed and velocity
	vec3 new_speed = spt->GetRigidBody()->GetSpeed() + spt->GetRigidBody()->GetAcceleration();
	vec3 new_force = vec3(new_speed.x * cosf(Math::DegToRad(spt->GetRigidBody()->GetDirectionAngle())),
		new_speed.y * sinf(Math::DegToRad(spt->GetRigidBody()->GetDirectionAngle())), 0) 
		*  (2.f / spt->GetRigidBody()->GetMass());

	//Update position by velocity and direction
	spt->SetPosition(vec3(
		spt->GetPosition().x + new_force.x,
		spt->GetPosition().y + new_force.y,
		spt->GetPosition().z));
}

/******************************************************************************/
/*!
\brief - Do line projection and check if it is or not

\param axis - pointer to standard aixes
\param index - axis' index
\param body1 - 1st sprite's body
\param body2 - 2nd sprite's body

\return bool
*/
/******************************************************************************/
bool World::AxisSeparatePolygons(vec3* axis, int& index, RigidBody* body1, RigidBody* body2)
{
	float min_a, max_a;
	float min_b, max_b;

	// Get intervals
	CalculateInterval(axis[index], body1, min_a, max_a);
	CalculateInterval(axis[index], body2, min_b, max_b);

	if (min_a > max_b || min_b > max_a)
		return true;

	// find the interval overlap
	float d0 = max_a - min_b;
	float d1 = max_b - min_a;
	float depth = d0 < d1 ? d0 : d1;

	// conver the separation axis into a push vector (re-normaliz
	// the axis and multiply by interval overlap)
	float axis_length_sqaured = axis[index].DotProduct(axis[index]);

	axis[index] *= depth / axis_length_sqaured;

	// Increase the index
	index++;
	return false;
}

/******************************************************************************/
/*!
\brief - Get mtd and collided segment of body

\param pushVector - pointer to axis vectors
\param iNumVectors - the number of total indexes

\return temp_mtd
*/
/******************************************************************************/
vec3 World::FindMTD(vec3* pushVector, int iNumVectors)
{
	// Init temp mtd 
	// and index of collided side
	vec3 temp_mtd = pushVector[0];
	int min_i = -1;
	float min_d2 = pushVector[0].DotProduct(pushVector[0]);

	// Get collided line
	for (int i = 1; i < iNumVectors; ++i)
	{
		float d2 = pushVector[i].DotProduct(pushVector[i]);
		if (d2 < min_d2)
		{
			// Get mtd and collided line's index
			min_i = i;
			min_d2 = d2;
			temp_mtd = pushVector[i];
		}
	}

	// If index has been found,
	// set it collided edge
	if (min_i != -1)
		collided_edge = pushVector[min_i];

	return temp_mtd;
}


/******************************************************************************/
/*!
\brief - Get interval between the segment and axis

\param axis - standard axis
\param body - sprite's body
\param min - minimumn float
\param max - maximum float
*/
/******************************************************************************/
void World::CalculateInterval(vec3& axis, RigidBody* body, float& min, float&max)
{
	// Get body's vertices
	Vertices verts = body->GetVertices();

	// Init 1st min and max
	min = max = axis.DotProduct(verts[0]);

	// Get min and max
	for (int i = 1; i < 4; ++i)
	{
		float d = axis.DotProduct(verts[i]);
		if (d < min) min = d;
		else if (d > max) max = d;
	}
}

/******************************************************************************/
/*!
\brief - Do Collision Intersect

\param spt1 - get 1st body's verts
\param spt2 - get 2nd body's verts

\return bool
*/
/******************************************************************************/
bool World::CollisionIntersect(Sprite* spt1, Sprite* spt2)
{
	// Collision between 2 balls
	if (spt1->GetRigidBody()->GetShape() == BALL &&
		spt2->GetRigidBody()->GetShape() == BALL)
		return IntersectBallToBall(spt1, spt2);

	// Collision between 2 boxes
	else if (spt1->GetRigidBody()->GetShape() == BOX && 
		spt2->GetRigidBody()->GetShape() == BOX)
		return IntersectBoxToBox(spt1, spt2);

	// Collision between ball and box
	else
	{
		if (spt1->GetRigidBody()->GetShape() == BOX &&
			spt2->GetRigidBody()->GetShape() == BALL)
			return IntersectBoxToBall(spt1, spt2);

		else
			return IntersectBoxToBall(spt2, spt1);
	}
}

/******************************************************************************/
/*!
\brief - Do Collision Intersect between 2 boxes

\param box1 - get 1st body's verts
\param box2 - get 2nd body's verts

\return bool
*/
/******************************************************************************/
bool World::IntersectBoxToBox(Sprite* spt1, Sprite* spt2)
{
	// Get bodies' edges
	Edges body1_edges = spt1->GetRigidBody()->GetEdges();
	Edges body2_edges = spt2->GetRigidBody()->GetEdges();

	// init helper variables
	vec3 vec_axis[8];
	int iNumAxis = 0;

	// Check overlapped for the 1st sprite
	for (int i = 0; i < 4; ++i)
	{
		vec_axis[iNumAxis] = vec3(-body1_edges[i].y, body1_edges[i].x);

		if (AxisSeparatePolygons(vec_axis, iNumAxis, spt1->GetRigidBody(), spt2->GetRigidBody()))
			return false;

	}

	// Check overlapped for the 2nd sprite
	for (int i = 0; i < 4; ++i)
	{
		vec_axis[iNumAxis] = vec3(-body2_edges[i].y, body2_edges[i].x);

		if (AxisSeparatePolygons(vec_axis, iNumAxis, spt1->GetRigidBody(), spt2->GetRigidBody()))
			return false;
	}

	//Find munumum transition distance
	mtd = FindMTD(vec_axis, iNumAxis);

	vec3 d = spt1->GetPosition() - spt2->GetPosition();

	// Reverse the mtd's sign
	if (d.DotProduct(mtd) < 0.f)
		mtd = -mtd;

	return true;
}

/******************************************************************************/
/*!
\brief - Do Collision Intersect between 2 balls

\param ball1 - get 1st body's position and radius
\param ball2 - get 2nd body's position and radius

\return bool
*/
/******************************************************************************/
bool World::IntersectBallToBall(Sprite* ball1, Sprite* ball2)
{
	// Get the distance of 2 circles' position and radius
	float distance = Math::DistanceOf2Points(ball1->GetPosition(), ball2->GetPosition());
	float sum_radius = (ball1->GetRigidBody()->GetScale().x + ball2->GetRigidBody()->GetScale().x) / 2.f;

	// Check if they are overlapped or not
	if (distance < sum_radius)
		return true;
	
	return false;

}

/******************************************************************************/
/*!
\brief - Do Collision Intersect between box and ball

\param box - get 1st body's verts
\param ball - get 2nd body's position and radius

\return bool
*/
/******************************************************************************/
bool World::IntersectBoxToBall(Sprite* box, Sprite* ball)
{
	// Call this radius
	float radius = ball->GetRigidBody()->GetScale().x / 2.f;

	vec3 relPos =  ball->GetPosition() - box->GetPosition();
	float relDIs = sqrt(relPos.x * relPos.x + relPos.y * relPos.y);
	float relDeg = Math::DegToRad(box->GetRotation()) - atan2(relPos.y, relPos.x);

	vec3 new_xy = vec3(cosf(relDeg), sinf(relDeg)) * relDIs;
	vec3 halfSize = box->GetRigidBody()->GetScale() / 2.f;

	if (new_xy.x >= -halfSize.x && new_xy.x <= halfSize.x &&
		new_xy.y >= -halfSize.y && new_xy.y <= halfSize.y)
		return true;

	// Init closest;
	vec3 closest;

	// Find the closest point in the X axis
	if (new_xy.x  < -halfSize.x)
		closest.x = -halfSize.x;
	else if (new_xy.x > halfSize.x)
		closest.x = halfSize.x;
	else
		closest.x = new_xy.x;

	// Find the closest point in the Y axis
	if (new_xy.y < -halfSize.y)
		closest.y = -halfSize.y;
	else if (new_xy.y > halfSize.y)
		closest.y = halfSize.y;
	else
		closest.y = new_xy.y;

	vec3 dt = new_xy - closest;
	float distance_sq = dt.DotProduct(dt);
	
	// Get mtd
	if (distance_sq <= radius * radius)
	{
		float dist = sqrt(distance_sq);
		if (dist < 0.0000001f) return false;

		mtd = dt * (radius - dist) / dist;

		if (relPos.DotProduct(mtd) < 0.f)
			mtd = -mtd;

		return true;
	}

	return false;

}

/******************************************************************************/
/*!
\brief - Do Collision Response

\param spt1 - get 1st body's verts
\param spt2 - get 2nd body's verts

*/
/******************************************************************************/
void World::CollisionResponse(Sprite* spt1, Sprite* spt2)
{	
	// Collision between 2 balls
	if (spt1->GetRigidBody()->GetShape() == BALL &&
		spt2->GetRigidBody()->GetShape() == BALL)
		ResponseBallToBall(spt1, spt2);

	// Collision between 2 boxes
	else if (spt1->GetRigidBody()->GetShape() == BOX &&
		spt2->GetRigidBody()->GetShape() == BOX)
		ResponseBoxToBox(spt1, spt2);

	// Collision between ball and box
	else
	{
		if (spt1->GetRigidBody()->GetShape() == BOX &&
			spt2->GetRigidBody()->GetShape() == BALL)
			ResponseBoxToBall(spt1, spt2);

		else
			ResponseBoxToBall(spt2, spt1);
	}

	// Refresh global minimum transition distance
	mtd = 0.f;
}

/******************************************************************************/
/*!
\brief - Do Collision Response between two balls

\param ball1 - set new body info
\param ball2 - set new body info

*/
/******************************************************************************/
void World::ResponseBallToBall(Sprite* ball1, Sprite* ball2)
{			
	// Get pointer to collided 2 rigid bodies
	RigidBody *body1 = ball1->GetRigidBody();
	RigidBody *body2 = ball2->GetRigidBody();

	// Subtract to balls position
	vec3 dt = ball1->GetPosition() - ball2->GetPosition();
	float d = dt.Length();

	// Get minumum transition distance
	mtd = dt * ((body1->GetScale().x / 2.f + body2->GetScale().x / 2.f) - d) / d;

	// Get reflection velocity
	float ball1_ci = body1->GetVelocity().DotProduct(dt);
	float ball2_ci = body2->GetVelocity().DotProduct(dt);

	// If 2nd ball is movable, refresh the 2nd ball's body info too
	if (body2->GetMoveToggle())
	{
		// Move to the uncollided last position
		ball1->SetPosition(ball1->GetPosition() + .5f * mtd);
		ball2->SetPosition(ball2->GetPosition() - .5f * mtd);

		// Set new velocity
		body1->SetVelocity(body1->GetVelocity() +
			(ball2_ci - ball1_ci) * dt );

		body2->SetVelocity(body2->GetVelocity() +
			(ball1_ci - ball2_ci) * dt );

		// Save new speeds of two boxes
		new_speed[0] = body1->GetSpeed() *
			(body1->GetMass() /
			(body2->GetMass() + body1->GetMass()));

		new_speed[1] = body2->GetSpeed() *
			(body2->GetMass() /
			(body2->GetMass() + body1->GetMass()));

		// Reset the speed
		body1->SetSpeed(new_speed[0] + new_speed[1] / body2->GetMass());
		body2->SetSpeed(new_speed[1] + new_speed[0] / body1->GetMass());
	}

	else
	{
		// Move to the uncollided last position
		ball1->SetPosition(ball1->GetPosition() + mtd);

		// Set new velocity
		body1->SetVelocity((body1->GetVelocity() +
			(ball2_ci - ball1_ci) * dt / d));

		// Save new speeds of two boxes
		new_speed[0] = body1->GetSpeed() *
			(body1->GetMass() /
			(body2->GetMass() + body1->GetMass()));

		// Set new speed
		body1->SetSpeed(new_speed[0] / body2->GetMass());
	}
}

/******************************************************************************/
/*!
\brief - Do Collision Response between two boxes

\param box1 - set new body info
\param box2 - set new body info

*/
/******************************************************************************/
void World::ResponseBoxToBox(Sprite* box1, Sprite* box2)
{			
	// Get pointer to collided 2 rigid bodies
	RigidBody *body1 = box1->GetRigidBody();
	RigidBody *body2 = box2->GetRigidBody();

	// If 2nd box is movable, refresh both 2 bodies info
	if (body2->GetMoveToggle())
	{
		// Move both onjects to be proper position
		// after the colliding
		box2->SetPosition(box2->GetPosition() - .5f * mtd);
		box1->SetPosition(box1->GetPosition() + .5f * mtd);

		// Calculate new velocity
		body1->SetVelocity(
			body1->GetVelocity().Reflection(
			collided_edge));

		// if 1st body's force is stronger than 2nd.
		// 2nd sprite's velocity to be set same as the 1st
		if (body1->GetMass() > body2->GetMass())
			body2->SetVelocity(body1->GetVelocity());

		// else if (the 2nd stronger, than set opposite velocity)
		// Set opposite velocity
		else
			body2->SetVelocity(-body1->GetVelocity());

		// Reset the speed
		// Save new speeds of two boxes
		new_speed[0] = body1->GetSpeed() *
			(body1->GetMass() /
			(body2->GetMass() + body1->GetMass()));

		new_speed[1] = body2->GetSpeed() *
			(body2->GetMass() /
			(body2->GetMass() + body1->GetMass()));

		// Reset the speeds
		body1->SetSpeed(new_speed[0] + new_speed[1] / body2->GetMass());
		body2->SetSpeed(new_speed[1] + new_speed[0] / body1->GetMass());
	}

	// 2nd sprite is unmovable,
	else
	{
		// Here refresh only 1sr sprite
		box1->SetPosition(box1->GetPosition() + mtd);

		// Calculate new reflected velocity
		body1->SetVelocity(
			body1->GetVelocity().Reflection(
			collided_edge));

		// Reset the speed
		new_speed[0] = body1->GetSpeed() *
			(body1->GetMass() /
			(body2->GetMass() + body1->GetMass()));

		// Reset the speeds
		body1->SetSpeed(new_speed[0]);
	}
}

/******************************************************************************/
/*!
\brief - Do Collision Response between ball and box

\param box - set new body info
\param ball - set new body info

*/
/******************************************************************************/
void World::ResponseBoxToBall(Sprite* box, Sprite* ball)
{		
	// Get pointer to collided 2 rigid bodies
	RigidBody *body1 = box->GetRigidBody();
	RigidBody *body2 = ball->GetRigidBody();

	vec3 n = mtd.Normalize();
	
	// Save new velocities
	vec3 new_vel[2];
	new_vel[0] = body1->GetVelocity() - n ;
	new_vel[1] = body2->GetVelocity() + n ;

	// Save new speed
	new_speed[0] = body1->GetSpeed() *
		(body1->GetMass() /
		(body2->GetMass() + body1->GetMass()));

	new_speed[1] = body2->GetSpeed() *
		(body2->GetMass() /
		(body2->GetMass() + body1->GetMass()));

	// If both 2 sprites are movable...
	if (body1->GetMoveToggle() && body2->GetMoveToggle())
	{
		// Set sprites' position with mtd
		box->SetPosition(box->GetPosition() - mtd * .5f);
		ball->SetPosition(ball->GetPosition() + mtd * .5f);

		// Set new velocity
		body1->SetVelocity(new_vel[0]);
		body2->SetVelocity(new_vel[1]);

		// Set new speed
		body1->SetSpeed(new_speed[0] + new_speed[1] / body2->GetMass());
		body2->SetSpeed(new_speed[1] + new_speed[0] / body1->GetMass());
	}

	// If only box sprite is movable...
	else if (!body2->GetMoveToggle() && body1->GetMoveToggle())
	{
		// Set sprites' position with mtd
		box->SetPosition(box->GetPosition() - mtd);

		// Set new velocity
		body1->SetVelocity(new_vel[0]);

		// Set new speed
		body1->SetSpeed(new_speed[0] + new_speed[1] / body2->GetMass());
	}

	// If only ball sprite is movable...
	else if (!body1->GetMoveToggle() && body2->GetMoveToggle())
	{
		// Set sprites' position with mtd
		ball->SetPosition(ball->GetPosition() + mtd);

		// Set new velocity
		body2->SetVelocity(new_vel[1]);

		// Set new speed
		body2->SetSpeed(new_speed[1] + new_speed[0] / body1->GetMass());
	}
}

/******************************************************************************/
/*!
\brief - Do Collision Relation

\param spt1 - get 1st sprite
\param spt2 - get 2nd sprite
\param coliided - Set 2 bodies collision status

*/
/******************************************************************************/
void World::CollisionRelation(Sprite* spt1, Sprite* spt2)
{
	//Set each sprite's collision status
	spt1->GetRigidBody()->CheckCollided(true);
	spt2->GetRigidBody()->CheckCollided(true);
	
	spt1->GetRigidBody()->SetCollisionWith(spt2);
	spt2->GetRigidBody()->SetCollisionWith(spt1);
}

/******************************************************************************/
/*!
\brief - Get 2 sprites' collision relation

\param spt1 - 1st sprite
\param spt2 - 2nd sprite
*/
/******************************************************************************/
bool World::GetCollisionRelation(Sprite* spt1, Sprite* spt2)
{
	if (spt1->GetRigidBody()->GetCollisionWith() == spt2
		&& spt2->GetRigidBody()->GetCollisionWith() == spt1)
		return true;

	return false;
}
