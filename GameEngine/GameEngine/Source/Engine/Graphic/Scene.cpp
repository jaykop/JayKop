/******************************************************************************/
/*!
\file   Scene.cpp
\author Jeong Juyong
\par    email: jeykop14\@gmail.com
\date   2016/06/19(yy/mm/dd)

\description
Contains Scene's class functions

All content (C) 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <algorithm>
#include "Scene.h"
#include "Text.h"
#include "Sprite.h"
#include "Particle.h"
#include "../InputManager/InputManager.h"
#include "../ObjectManager/ObjectManager.h"
#include "../Utilities/Math/MathUtils.h"
#include "../StateManager/GameStateManager/GameStateManager.h"

/******************************************************************************/
/*!
\brief - Scene Constructor

\param pApp - pointer to Application

*/
/******************************************************************************/
Scene::Scene(GameStateManager* gsm)
: m_width(0), m_height(0), m_zNear(0),
m_zFar(0), m_fovy(0), aspectRatio(0), m_radius(0),
m_camera(vec4()), m_bgColor(vec4(0,0,0,0))
{
	m_GSM = gsm;
	m_DrawList.clear();
}

/******************************************************************************/
/*!
\brief - Scene Destructor
*/
/******************************************************************************/
Scene::~Scene(void)
{}

/******************************************************************************/
/*!
\brief - Initiaiize Scene
*/
/******************************************************************************/
void Scene::Init(const ObjectList& objList)
{
	UNREFERENCED_PARAMETER(objList);

	//Get projection information
	ProjectionInfo temp = m_GSM->GetGLManager()->GetProjectionInfo();

	// Set basic scene infomation
	m_width = static_cast<int>(temp.m_width);
	m_height = static_cast<int>(temp.m_height);
	m_zNear = static_cast<float>(temp.m_zNear);
	m_zFar = static_cast<float>(temp.m_zFar);
	m_fovy = static_cast<float>(temp.m_fovy);
	aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
	m_camera = vec4(0, 0, 80, 0);

	// Init every sprites
	//for (auto it = objList.begin(); it != objList.end(); ++it)
		//if (it->second->GetObjectType() == SPRITE) 
			//it->second->GetTexture()->LoadTexture("Resource/Texture/rect.png");

	// Sort Sprites by projection type and z Order
	//ReorderSprites();
}

/******************************************************************************/
/*!
\brief - Draw sprites

\param sprite - sprite to be drawn
*/
/******************************************************************************/
void Scene::DrawSprites(Sprite* sprite)
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex_buffer_data), m_vertex_buffer_data, GL_STATIC_DRAW);

	// Bind our texture in Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, sprite->GetTexture()->GetTexId());

	// Draw the triangle
	glDrawArrays(GL_QUADS, 0, 4);
}

/******************************************************************************/
/*!
\brief - Draw texts

\param text - text to be drawn
*/
/******************************************************************************/
void Scene::DrawTexts(Text* text)
{
	GLfloat new_x = static_cast<GLfloat>(text->GetPosition().x);

	// Iterate all character
	std::wstring::const_iterator c;
	for (c = text->GetText().begin(); c != text->GetText().end(); ++c)
	{
		Character ch = m_GSM->GetGLManager()->GetCharacters()[*c];
		GLfloat xpos = new_x + ch.Bearing.x * text->GetScale().x;
		GLfloat ypos = text->GetPosition().y - (ch.Size.y - ch.Bearing.y) * text->GetScale().y;
		GLfloat zpos = text->GetPosition().z;

		GLfloat w = ch.Size.x * text->GetScale().x;
		GLfloat h = ch.Size.y * text->GetScale().y;

		//Update vbo
		GLfloat vertices[4][5] = {
			{ xpos, ypos + h, zpos, 0.0, 0.0 },
			{ xpos, ypos, zpos, 0.0, 1.0 },
			{ xpos + w, ypos, zpos, 1.0, 1.0 },
			{ xpos + w, ypos + h, zpos, 1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_QUADS, 0, 4);
		new_x += (ch.Advance >> 6) * text->GetScale().x;
	}
}

/******************************************************************************/
/*!
\brief - Draw particles

\param particle - particle to be drawn
*/
/******************************************************************************/
void Scene::DrawParticle(Particle* particle)
{
	UNREFERENCED_PARAMETER(particle);

	//vec3 v(0, 0);
	//float velocity, theta, phi;
	//GLfloat* data = new GLfloat[nParticles * 3];
	//for (GLuint i = 0; i < nParticles; ++i)
	//{
	//	//Piack the direction of the velocity
	//	theta = ;
	//	phi = ;

	//	v.x = sinf(theta) * cosf(phi);
	//	v.y = cosf(theta);
	//	v.z = sinf(theta) * sinf(phi);

	//	// Scale to set the magnitude of the velocity (speed)
	//	velocity = ;
	//	v = v * velocity;

	//	data[3 * i] = v.x;
	//	data[3 * i + 1] = v.y;
	//	data[3 * i + 2] = v.z;

	//	glBindBuffer(GL_ARRAY_BUFFER. initVel);
	//	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * 3 * sizeof(GLfloat), data);
	//}
}

/******************************************************************************/
/*!
\brief - Draw Scene
*/
/******************************************************************************/
void Scene::Update(const ObjectList& objList)
{
	UNREFERENCED_PARAMETER(objList);

	//Refresh the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(m_bgColor.x, m_bgColor.y, m_bgColor.z, m_bgColor.w);

	GetPerspPosition();
	GetOrthoPosition();

	//Todo: Lambda loop expression
	//std::for_each(m_DrawList.begin(), m_DrawList.end(), [&](DrawList::iterator& it)
	//{

	for (auto it = m_DrawList.begin(); it != m_DrawList.end(); ++it)
	{
		//Update pipeline
		Pipeline((*it));
		vec4 sptColor = ((*it)->GetColor());

		glUniformMatrix4fv(m_GSM->GetGLManager()->GetUnifrom(TRANSFORM), 1, GL_FALSE, &m_mvp.m_member[0][0]);
		glUniformMatrix4fv(m_GSM->GetGLManager()->GetUnifrom(UV), 1, GL_FALSE, &m_animation.m_member[0][0]);
		glUniform4f(m_GSM->GetGLManager()->GetUnifrom(COLOR), sptColor.x, sptColor.y, sptColor.z, sptColor.w);
		glUniform1d(m_GSM->GetGLManager()->GetUnifrom(TYPE), (*it)->GetObjectType());

		//Todo: high quality?
		//glUniformMatrix4fv();

		// Draw Sprites
		if ((*it)->GetObjectType() == SPRITE)
			DrawSprites(*it);

		// Draw Texts 
		else if ((*it)->GetObjectType() == TEXT) 
			DrawTexts(static_cast<Text*>(*it));

		// Draw Particles
		else if ((*it)->GetObjectType() == PARTICLE)
			DrawParticle(static_cast<Particle*>(*it));
	}

	//std::cout <<  "\n";
	//}); //Lambda loop expression
}

/******************************************************************************/
/*!
\brief - Shutdown Scene
*/
/******************************************************************************/
void Scene::Shutdown(const ObjectList& objList)
{
	UNREFERENCED_PARAMETER(objList);

	// Clear all sprites from the list to draw
	m_DrawList.clear();
}

/******************************************************************************/
/*!
\brief - Scene's graphic pipeline

\param sprite - sprite to be drawed
*/
/******************************************************************************/
void Scene::Pipeline(Sprite* sprite)
{
	//Init model matrix
	mat44 model;
	model.SetIdentity();

	//Setting Perspection
	mat44 projection;
	if (sprite->GetProjectionType() == PERSPECTIVE)
		projection = mat44::Perspective(m_fovy, aspectRatio, m_zNear, m_zFar);
	else
		projection = mat44::Ortho(-m_width / 2.f, m_width / 2.f, -m_height / 2.f, m_height / 2.f, m_zNear, m_zFar);

	//Setting Camera
	mat44 camera = mat44::LookAt(
		vec3(m_camera),
		vec3(m_camera.x, m_camera.y, 0),
		vec3(cosf(Math::DegToRad((m_camera.w + 90.f))),
			sinf(Math::DegToRad((m_camera.w + 90.f))),
			0));

	//Transform model mat
	model = model * mat44::Scale(sprite->GetScale());
	model = model * mat44::Rotate(Math::DegToRad(sprite->GetRotation()), vec3(.0f, .0f, 1.f));
	model = model * mat44::Translate(sprite->GetPosition());

	//calculate fined final matrix
	m_mvp = projection.Transpose() * camera.Transpose() * model.Transpose();
	m_mvp = m_mvp.Transpose();

	// Refresh the animation scene
	if (sprite->GetPlayToggle())
	{
		if (sprite->GetDividedSpeed() <= sprite->GetTimer().GetElapsedTime())
		{
			// If current scene is over, refresh
			if (sprite->GetCurrentScene() + sprite->GetDividedFrame() >= 1)
				sprite->SetCurrentScene(0);

			else
				sprite->SetCurrentScene(sprite->GetCurrentScene() + sprite->GetDividedFrame());

			sprite->GetTimer().StartTime();
		}
	}

	// Animation pipeline
	mat44 animation;
	animation.SetIdentity();
	animation = animation * mat44::Scale(vec3(sprite->GetDividedFrame(), 1.f));
	animation = animation * mat44::Translate(vec3(sprite->GetCurrentScene(), 0.f));

	m_animation = animation;

}

/******************************************************************************/
/*!
\brief - Camera settor

\param camera
*/
/******************************************************************************/
void Scene::SetCamera(const vec4& camera)
{
	m_camera = camera;
}

/******************************************************************************/
/*!
\brief - Camera gettor

\return m_camera
*/
/******************************************************************************/
vec4 Scene::GetCamera(void) const
{
	return m_camera;
}

/******************************************************************************/
/*!
\brief - Background Color settor

\param background
*/
/******************************************************************************/
void Scene::SetBackgroundColor(const vec4& background)
{
	m_bgColor = background;
}

/******************************************************************************/
/*!
\brief - Background Color gettor

\return m_bgColor
*/
/******************************************************************************/
vec4 Scene::GetBackgroundColor(void) const
{
	return m_bgColor;
}

namespace
{
	/******************************************************************************/
	/*!
	\brief - Order algorithm

	\param a - 1st compared
	\param b - 2nd compared

	*/
	/******************************************************************************/
	bool reorder_sprites(const Sprite* a, const Sprite* b)
	{
		// Distinguish ortho type and pers type 
		if (a->GetProjectionType() == ORTHOGONAL && b->GetProjectionType() == PERSPECTIVE)
			return false;
		
		else if (a->GetProjectionType() == PERSPECTIVE && b->GetProjectionType() == ORTHOGONAL)
			return true;
		
		// Return higher z order on
		else
			return a->GetPosition().z < b->GetPosition().z;
	}
}

/******************************************************************************/
/*!
\brief - Order algorithm

*/
/******************************************************************************/
void Scene::ReorderSprites(void)
{
	// Order algorithm
	std::sort(m_DrawList.begin(), m_DrawList.end(), reorder_sprites);
}

/******************************************************************************/
/*!
\brief - Add Sprite to the list

\param newSpt - sprite to put into the list
*/
/******************************************************************************/
void Scene::AddSprite(Sprite* sprite)
{
	// Add sprite by Zorder and Projection type
	auto it = m_DrawList.begin();

	for (; it != m_DrawList.end(); ++it)
	{
		// If new sprite's z order is lower than next sprite
		// Stop here
		if ((*it)->GetPosition().z > sprite->GetPosition().z)
			break;
	}

	// Add sprite to the place where stopped
	m_DrawList.insert(it, sprite);

	// Sort Sprites by projection type and z Order
	ReorderSprites();

}

/******************************************************************************/
/*!
\brief - Delete Sprite from the list

\param id - sprite's id to delete
*/
/******************************************************************************/
void Scene::DeleteSprite(const int id)
{
	//Find the sprite from the vector
	for (auto it = m_DrawList.begin(); it != m_DrawList.end(); ++it)
	{
		// If found same id that client want to get
		if ((*it)->GetID() == id)
		{
			// Delete it
			m_DrawList.erase(it);
			// m_OrderList.erase(it++); 
			break;
		}
	}

	// Sort Sprites by projection type and z Order
	ReorderSprites();
}

/******************************************************************************/
/*!
\brief Set mouse position on perpective world
*/
/******************************************************************************/
void Scene::GetPerspPosition(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(m_fovy, aspectRatio, m_zNear, m_zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(m_camera.x, m_camera.y, m_camera.z,
		m_camera.x, m_camera.y, 0.0,
		(double)cosf((float)((m_camera.w + 90.f) * RADIAN)),
		(double)sinf((float)((m_camera.w + 90.f) * RADIAN)),
		0.0);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLdouble winX = 0, winY = 0, winZ = 0;

	gluProject(0, 0, 0, modelview, projection, viewport, &winX, &winY, &winZ);

	winX = (float)InputManager::GetInstance().GetRawMousePosition().x;
	winY = (float)InputManager::GetInstance().GetRawMousePosition().y;
	winY = (float)viewport[3] - winY;

	GLdouble posX = m_camera.x, posY = m_camera.y, posZ = m_camera.z;

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	InputManager::GetInstance().SetPerspMouse(vec3((float)posX, (float)posY, (float)posZ));

}

/******************************************************************************/
/*!
\brief Set mouse position on orthogonal world
*/
/******************************************************************************/
void Scene::GetOrthoPosition(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-m_width / 2.f, m_width / 2.f, m_height / 2.f, -m_height / 2.f, m_zNear, m_zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	GLdouble winX = 0, winY = 0, winZ = 0;

	gluProject(0, 0, 0, modelview, projection, viewport, &winX, &winY, &winZ);

	winX = (float)InputManager::GetInstance().GetRawMousePosition().x;
	winY = (float)InputManager::GetInstance().GetRawMousePosition().y;
	winY = (float)viewport[3] - winY;

	GLdouble posX = 0, posY = 0, posZ = 0;

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	InputManager::GetInstance().SetOrthoMouse(vec3((float)posX, (float)posY, (float)posZ));
}