#include "BaseStage/BaseStage.h"

class PauseStage : public Stage
{
public:

	PauseStage(GameStateManager* gsm = 0);

	virtual ~PauseStage();
	virtual void Init(GameData& gd);
	virtual void Update(GameData& gd);
	virtual void Shutdown();

private:

	//Obj Manager
	ObjectManager m_ObjM;
	Scene* m_scene;
	World* m_world;

};