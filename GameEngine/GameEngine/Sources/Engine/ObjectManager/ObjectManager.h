#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include <hash_map>
#include <vector>

typedef std::hash_map<int, bool> Relation;

class Sprite;
class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void AddObject(const int SpriteID, const std::string& textureDir = "");
	void RemoveObject(const int id);
	Sprite* GetGameObject(const int id);
	bool HasObject(const int id);
	const std::hash_map<int, Sprite*>& GetList(void) const;

	void ClearObjectList(void);

private:
	
	// Sprites info
	std::hash_map<int, Sprite*> m_ObjectList;	// Sprite hashmap
	
	// for scsne
	std::vector<float, Sprite*> m_orthoList;	// Ortho sprites hashmap
	std::vector<float, Sprite*> m_perspList;	// Persp sprites hashmap

	// for world
	std::vector<int, Relation> m_collision;	// Collision relation hashmap

	// The number of the sprites
	int number_of_Spt;

};

#endif //_OBJECTMANAGER_H_