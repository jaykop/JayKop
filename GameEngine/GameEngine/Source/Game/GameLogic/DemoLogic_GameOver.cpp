#include "DemoLogic_GameOver.h"
#include "../../Engine/Logic/Logic.h"

GameLogic* DemoLogic_GameOverBuilder::BuildLogic(Object* Owner) const
{
	return new DemoLogic_GameOver(Owner);
}

DemoLogic_GameOver::DemoLogic_GameOver(Object* owner)
: m_Owner(owner), m_OBM(owner->GetOBM()), m_GSM(owner->GetOBM()->GetGSM())
{}

void DemoLogic_GameOver::Load(const Json::Value& data)
{
	std::cout << "SanpleLogic Load\n";
	
	if (data.isMember("NewColor") &&
		data["NewColor"].isArray() &&
		data["NewColor"].size() == 4 &&
		data["NewColor"][0].isNumeric())
		color = vec4(
			data["NewColor"][0].asFloat(),
			data["NewColor"][1].asFloat(),
			data["NewColor"][2].asFloat(),
			data["NewColor"][3].asFloat());

	if (data.isMember("NewSize") &&
		data["NewSize"].isArray() &&
		data["NewSize"].size() == 3 &&
		data["NewSize"][0].isNumeric())
		size = vec3(
			data["NewSize"][0].asFloat(),
			data["NewSize"][1].asFloat(),
			data["NewSize"][2].asFloat());
}

void DemoLogic_GameOver::Init(GameData& gd)
{
	UNREFERENCED_PARAMETER(gd);
	std::cout << "SanpleLogic Inited\n";

	Sprite* newOwner = static_cast<Sprite*>(m_Owner);
	newOwner->SetColor(color);
	newOwner->SetScale(size);
}

void DemoLogic_GameOver::Update(GameData& gd)
{
	UNREFERENCED_PARAMETER(gd);
	std::cout << "SanpleLogic Update\n";
}

void DemoLogic_GameOver::Shutdown(GameData& gd)
{
	UNREFERENCED_PARAMETER(gd);
	std::cout << "SanpleLogic Shutdown\n";
}

void DemoLogic_GameOver::Unload(void)
{
	std::cout << "SanpleLogic Unload\n";
	
}