/******************************************************************************/
/*!
\file   Object.cpp
\author Jeong Juyong
\par    email: jeykop14\@gmail.com
\date   2016/06/25(yy/mm/dd)

\description
Contains Object's class functions

All codes are written by Jaykop Jeong...
*/
/******************************************************************************/

#include "Object.h"
#include "../ObjectManager.h"
#include "../../Logic/Logic.h"
#include "../../StateManager/GameStateManager/GameStateManager.h"

/******************************************************************************/
/*!
\brief - Object's constructor
\param id - Object's id
\param type - Object's type
\param obm - Pointer to object manager
*/
/******************************************************************************/
Object::Object(const int id, ObjectManager* obm)
: m_id(id), m_OBM(obm)
{}

/******************************************************************************/
/*!
\brief - Object's Copy Constructor
*/
/******************************************************************************/
Object::Object(const Object& object)
{
	m_id = object.m_id;
	m_logicList = object.m_logicList;
	m_OBM = object.m_OBM;
	m_type = object.m_type;
}

/******************************************************************************/
/*!
\brief - Object's Destructor
*/
/******************************************************************************/
Object::~Object()
{
 	ClearLogicList();
}

/******************************************************************************/
/*!
\brief - Get Object's id

\return m_id - Object's id
*/
/******************************************************************************/
const int Object::GetID(void) const
{
	return m_id;
}

/******************************************************************************/
/*!
\brief - Set Object's id

\param id - Object's id
*/
/******************************************************************************/
void Object::SetID(const int id)
{
	m_id = id;
}

/******************************************************************************/
/*!
\brief - Set Object manager

\param obm - Pointer to Object manager
*/
/******************************************************************************/
void Object::SetObjectManager(ObjectManager* obm)
{
	m_OBM = obm;
}

/******************************************************************************/
/*!
\brief - Clear all logics in the list
*/
/******************************************************************************/
void Object::ClearLogicList(void)
{
	for (auto it = m_logicList.begin(); it != m_logicList.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = 0;
		}
	}
	m_logicList.clear();
}

/******************************************************************************/
/*!
\brief - Add a logic to object's list
\param logic_name - logic's name
*/
/******************************************************************************/
void Object::AddLogic(GameLogic* logic)
{
	// Add logic to the object's logic list
	m_logicList[logic->GetKey()] = logic;
}

/******************************************************************************/
/*!
\brief - Get the object's logic list
\return m_logicList
*/
/******************************************************************************/
const LogicMap& Object::GetLogicList(void) const
{
	return m_logicList;
}

/******************************************************************************/
/*!
\brief - Get pointer to the OBM
\return m_OBM
*/
/******************************************************************************/
ObjectManager* Object::GetOBM(void)
{
	return m_OBM;
}

/******************************************************************************/
/*!
\brief - Set object's type
\param type
*/
/******************************************************************************/
void Object::SetType(ObjectType type)
{
	m_type = type;
}

/******************************************************************************/
/*!
\brief - Get object's type
\return m_type
*/
/******************************************************************************/
ObjectType Object::GetType(void) const
{
	return m_type;
}