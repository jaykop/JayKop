/******************************************************************************/
/*!
\file   Logic.h
\author Jeong Juyong
\par    email: jeykop14\@gmail.com
\date   2016/06/25(yy/mm/dd)

\description
Contains Logic builder's class

All content (C) 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#ifndef _LOGICBUILDER_H_
#define _LOGICBUILDER_H_

#include "GameLogic.h"

class Object;

//! Logic builder
class LogicBuilder{

public:

	virtual ~LogicBuilder(void) {};
	virtual GameLogic* BuildLogic(Object* GSM) = 0;

private:

};

#endif //_LOGICBUILDER_H_