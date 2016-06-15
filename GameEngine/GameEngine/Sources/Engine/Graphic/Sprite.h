#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <string>
#include "../Apps/GLApplication.h"
#include "../Utilities/Math/MathUtils.h"

//Sprites' shape
enum Shape { CIRCLE, RECTANGLE };
enum Projt { PERSP, ORTHO };
class RigidBody;
class Sprite
{
	//friend ObjectManager;

public:
	
	//Constructor and destructor
	Sprite(const int id);
	~Sprite();

	//Cretae and delete function
	static Sprite* Create(const std::string& textureDir = 0);
	void Init();

	const int GetID(void) const;

	//Transforming functions
	void SetPosition(const vec3& position);
	void SetScale(const vec3& scale);
	void SetRotation(float degree);
	
	vec3 GetPosition(void) const;
	vec3 GetScale(void) const;
	float GetRotation(void) const;

	//Color and texture setting
	void SetColor(const vec4& color);
	vec4 GetColor(void) const;
	
	//Physics setting
	void BindRigidBody(void);
	RigidBody* GetRigidBody(void) const;
	void RemoveRigidBody(void);
	bool HasRigidBody(void) const;

	void SetSpriteShape(Shape type);
	Shape GetSpriteShape(void) const;
		
	/********** To do *********/
	//void SetTexture();
	//Texture GetTexture(void) const;

private:
	
	// Sprite id
	int m_id;

	// Drawing Info
	// Math info
	vec3 m_position;
	vec3 m_scale;
	vec4 m_color;
	float m_degree;

	// Type Info
	Shape m_type;
	Projt m_prjt;

	// Texture Info
	// Texture m_texure;

	// Physics Info
	bool m_HasBody;
	RigidBody* m_body;

};

#endif // _SPRITE_H_