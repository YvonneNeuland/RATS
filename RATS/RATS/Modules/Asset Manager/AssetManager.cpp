#include "stdafx.h"
#include "AssetManager.h"
#include <fstream>
#include "DDSTextureLoader.h"

using namespace std;
using namespace DirectX;


AssetManager::AssetManager()
{
}


AssetManager::~AssetManager()
{
	//Shutdown();
}

void AssetManager::Initialize(ID3D11Device* device)
{
	m_device = device;
}

void AssetManager::LoadAssets()
{

	m_meshes["Ribbon"] = new Mesh();
	LoadTexture(m_meshes["Ribbon"], L"Assets/Textures/RibonTexture.dds");

	m_meshes["Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Fire Particle"], L"Assets/Textures/Particles/Fire.dds", 0.5f);

	m_meshes["Red Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Red Fire Particle"], L"Assets/Textures/Particles/Red.dds", 0.5f);

	m_meshes["Pink Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Pink Fire Particle"], L"Assets/Textures/Particles/Pink.dds", 0.5f);

	m_meshes["Yellow Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Yellow Fire Particle"], L"Assets/Textures/Particles/Yellow.dds", 0.5f);

	m_meshes["Purple Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Purple Fire Particle"], L"Assets/Textures/Particles/PurpleExp.dds", 0.5f);

	m_meshes["Green Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Green Fire Particle"], L"Assets/Textures/Particles/Green.dds", 0.5f);

	m_meshes["Cyan Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Cyan Fire Particle"], L"Assets/Textures/Particles/Cyan.dds", 0.5f);

	m_meshes["Orangey Fire Particle"] = new Mesh();
	LoadBillboard(m_meshes["Orangey Fire Particle"], L"Assets/Textures/Particles/Orange.dds", 0.5f);

	m_meshes["Fire Particle 2"] = new Mesh();
	LoadBillboard(m_meshes["Fire Particle 2"], L"Assets/Textures/Particles/Whip.dds", 0.5f);

	m_meshes["Purple Particle"] = new Mesh();
	LoadBillboard(m_meshes["Purple Particle"], L"Assets/Textures/Particles/Purple.dds", 0.5f);

	m_meshes["Ice Particle"] = new Mesh();
	LoadBillboard(m_meshes["Ice Particle"], L"Assets/Textures/Glow/CyanGlow.dds", 0.5f);

	m_meshes["Gold Particle"] = new Mesh();
	LoadBillboard(m_meshes["Gold Particle"], L"Assets/Textures/Glow/YellowGlow.dds", 0.5f);

	m_meshes["Green Particle"] = new Mesh();
	LoadBillboard(m_meshes["Green Particle"], L"Assets/Textures/Glow/GreenGlow.dds", 0.5f);

	m_meshes["Red Particle"] = new Mesh();
	LoadBillboard(m_meshes["Red Particle"], L"Assets/Textures/Glow/RedGlow.dds", 0.5f);

	m_meshes["White Particle"] = new Mesh();
	LoadBillboard(m_meshes["White Particle"], L"Assets/Textures/Glow/WhiteGlow.dds", 0.5f);

	m_meshes["Invisible"] = new Mesh();
	LoadBillboard(m_meshes["Invisible"], L"INVISIBLE.dds", 0.5f);

	m_meshes["Spark Particle2"] = new Mesh();
	LoadBillboard(m_meshes["Spark Particle2"], L"Assets/Textures/Particles/Sparkle2.dds", 0.5f);

	m_meshes["Spark Particle4"] = new Mesh();
	LoadBillboard(m_meshes["Spark Particle4"], L"Assets/Textures/Particles/Sparkle4.dds", 0.5f);

	m_meshes["Smokey Particle"] = new Mesh();
	LoadBillboard(m_meshes["Smokey Particle"], L"Assets/Textures/Particles/Smoky.dds", 0.5f);

	m_meshes["Energy Battery"] = new Mesh();
	LoadBillboard(m_meshes["Energy Battery"], L"Assets/Textures/Particles/battery_energy.dds", 0.5f);

	m_meshes["Health Battery"] = new Mesh();
	LoadBillboard(m_meshes["Health Battery"], L"Assets/Textures/Particles/battery_health.dds", 0.5f);

	m_meshes["Warning Skull"] = new Mesh();
	LoadBillboard(m_meshes["Warning Skull"], L"Assets/Textures/Particles/WarningSkull.dds", 0.5f);

	/////////////////////////
	//ENVIRONMENTAL HAZARDS
	////////////////////////

	//TORNADO
	m_meshes["Tornado"] = new Mesh();
	LoadModel(m_meshes["Tornado"], L"Assets/Textures/Bullets/PurpleBullet.dds", "Assets/Mesh/Tornado.mesh");

	//PROMINENCE
	m_meshes["Prominence"] = new Mesh();
	LoadModel(m_meshes["Prominence"], L"Assets/Textures/Bullets/OrangeBullet.dds", "Assets/Mesh/Prominence.mesh");

	//PROMINENCE BULLET
	m_meshes["ProminenceBullet"] = new Mesh();
	LoadModel(m_meshes["ProminenceBullet"], L"Assets/Textures/Bullets/OrangeBullet.dds", "Assets/Mesh/ProminenceBullet.mesh");

	//SINGULARITY
	m_meshes["Singularity"] = new Mesh();
	LoadModel(m_meshes["Singularity"], L"Assets/Textures/BlackMetallic3.dds", "Assets/Mesh/Singularity.mesh");

	///////////
	//BULLETS//
	///////////

	//WHIP
	m_meshes["Player Whip Bullet"] = new Mesh();
	LoadModel(m_meshes["Player Whip Bullet"], L"Assets/Textures/Bullets/CyanWhiteBullet.dds", "Assets/Mesh/BulletB.mesh");


	m_meshes["Player Whip Bullet Glow"] = new Mesh();
	LoadBillboard(m_meshes["Player Whip Bullet Glow"], L"Assets/Textures/Glow/CyanGlow.dds", 5.0f);

	//SPREAD

	m_meshes["Player Spread Bullet"] = new Mesh();
	LoadModel(m_meshes["Player Spread Bullet"], L"Assets/Textures/Bullets/GoldWhiteBullet.dds", "Assets/Mesh/SpikeBullet.mesh");

	m_meshes["Player Spread Bullet Glow"] = new Mesh();
	LoadBillboard(m_meshes["Player Spread Bullet Glow"], L"Assets/Textures/Glow/YellowGlow.dds", 3.9f);

	//MISSILE

	m_meshes["Player Missile Bullet"] = new Mesh();
	LoadModel(m_meshes["Player Missile Bullet"], L"Assets/Textures/Bullets/PurpleWhiteBullet.dds", "Assets/Mesh/Missile.mesh");

	m_meshes["Player Missile Bullet Glow"] = new Mesh();
	LoadBillboard(m_meshes["Player Missile Bullet Glow"], L"Assets/Textures/Glow/PurpleGlow.dds", 5);

	//SEEKER
	m_meshes["Player Seeker Bullet"] = new Mesh();
	LoadModel(m_meshes["Player Seeker Bullet"], L"Assets/Textures/Bullets/PurpleBullet.dds", "Assets/Mesh/SpikeBullet.mesh");

	m_meshes["Player Seeker Bullet Glow"] = new Mesh();
	LoadBillboard(m_meshes["Player Seeker Bullet Glow"], L"Assets/Textures/Glow/PurpleGlow.dds", 3.9f);

	//Enemy simple
	m_meshes["Enemy Simple Bullet"] = new Mesh();
	LoadModel(m_meshes["Enemy Simple Bullet"], L"Assets/Textures/Bullets/RedWhiteBullet.dds", "Assets/Mesh/SpikeBullet.mesh");

	m_meshes["Enemy Simple Bullet Glow"] = new Mesh();
	LoadBillboard(m_meshes["Enemy Simple Bullet Glow"], L"Assets/Textures/Glow/RedGlow.dds", 3.9f);

	//Enemy missile
	m_meshes["Enemy Missile"] = new Mesh();
	LoadModel(m_meshes["Enemy Missile"], L"Assets/Textures/Bullets/RedBullet.dds", "Assets/Mesh/Missile.mesh");

	m_meshes["Enemy Missile Glow"] = new Mesh();
	LoadBillboard(m_meshes["Enemy Missile Glow"], L"Assets/Textures/Glow/OrangeGlow.dds", 7);

	//////////
	//SKYBOX//
	//////////

	m_meshes["Skybox"] = new Mesh();
	LoadTexture(m_meshes["Skybox"], L"Assets/Textures/Skybox.dds");


	//////////
	//SPHERE//
	//////////

	m_meshes["Sphere"] = new Mesh();
	LoadDynamicModel(m_meshes["Sphere"], L"Assets/Textures/Grid.dds", "Assets/Mesh/Sphere.mesh");

	m_meshes["Distortion A Sphere"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion A Sphere"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/Sphere.mesh");

	m_meshes["Distortion B Sphere"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion B Sphere"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/Sphere.mesh");

	m_meshes["Distortion C Sphere"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion C Sphere"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/Sphere.mesh");

	m_meshes["Distortion D Sphere"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion D Sphere"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/Sphere.mesh");

	m_meshes["InnerStar"] = new Mesh();
	LoadModel(m_meshes["InnerStar"], L"Assets/Textures/Sun.dds", "Assets/Mesh/Sphere.mesh");


	/////////
	//TORUS//
	/////////
	
	m_meshes["Torus"] = new Mesh();
	LoadDynamicModel(m_meshes["Torus"], L"Assets/Textures/Grid.dds", "Assets/Mesh/Torus.mesh");

	m_meshes["Distortion A Torus"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion A Torus"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/DistortionTorus.mesh");

	m_meshes["Distortion B Torus"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion B Torus"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/DistortionTorus.mesh");

	m_meshes["Distortion C Torus"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion C Torus"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/DistortionTorus.mesh");

	m_meshes["Distortion D Torus"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion D Torus"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/DistortionTorus.mesh");

	m_meshes["InnerTorus"] = new Mesh();
	LoadModel(m_meshes["InnerTorus"], L"Assets/Textures/Sun.dds", "Assets/Mesh/InnerTorus.mesh");


	////////////////
	//ROUNDED CUBE//
	////////////////

	m_meshes["RoundedCube"] = new Mesh();
	LoadDynamicModel(m_meshes["RoundedCube"], L"Assets/Textures/Grid.dds", "Assets/Mesh/RoundedCube.mesh");

	m_meshes["Distortion A RoundedCube"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion A RoundedCube"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/RoundedCube.mesh");

	m_meshes["Distortion B RoundedCube"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion B RoundedCube"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/RoundedCube.mesh");

	m_meshes["Distortion C RoundedCube"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion C RoundedCube"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/RoundedCube.mesh");

	m_meshes["Distortion D RoundedCube"] = new Mesh();
	LoadDynamicModel(m_meshes["Distortion D RoundedCube"], L"Assets/Textures/Distortion.dds", "Assets/Mesh/RoundedCube.mesh");

	m_meshes["InnerCube"] = new Mesh();
	LoadModel(m_meshes["InnerCube"], L"Assets/Textures/Sun.dds", "Assets/Mesh/InnerCube.mesh");


	/////////
	//FONTS//
	/////////

	m_meshes["Font"] = new Mesh();
	LoadTexture(m_meshes["Font"], L"Assets/Textures/Fonts/Audiowide.dds");


	m_meshes["InnerRock"] = new Mesh();
	LoadBillboard(m_meshes["InnerRock"], L"Assets/Textures/InnerRock.dds");



	//////////////////
	//HUD COMPONENTS//
	//////////////////

	m_meshes["PlayerHealthBarBack"] = new Mesh();
	LoadTexture(m_meshes["PlayerHealthBarBack"], L"Assets/Textures/HealthBar.dds");

	m_meshes["PlayerHealthBarFill"] = new Mesh();
	LoadTexture(m_meshes["PlayerHealthBarFill"], L"Assets/Textures/HudFillBarGreen.dds");

	m_meshes["PlayerEnergyFill"] = new Mesh();
	LoadTexture(m_meshes["PlayerEnergyFill"], L"Assets/Textures/EnergyFillBar2.dds");

	m_meshes["PlayerEnergyBarBack"] = new Mesh();
	LoadTexture(m_meshes["PlayerEnergyBarBack"], L"Assets/Textures/HealthBar.dds");

	m_meshes["PylonEnergyFill"] = new Mesh();
	LoadTexture(m_meshes["PylonEnergyFill"], L"Assets/Textures/HudFillBarVertical2.dds");

	m_meshes["PylonEnergyBarBack"] = new Mesh();
	LoadTexture(m_meshes["PylonEnergyBarBack"], L"Assets/Textures/HudBarVertical.dds");

	m_meshes["GameOver"] = new Mesh();
	LoadTexture(m_meshes["GameOver"], L"Assets/Textures/GameOver.dds");

	m_meshes["GameWon"] = new Mesh();
	LoadTexture(m_meshes["GameWon"], L"Assets/Textures/YouWon.dds");

	m_meshes["iconPylon1"] = new Mesh();
	LoadTexture(m_meshes["iconPylon1"], L"Assets/Textures/Icons/LambaCircle.dds");

	m_meshes["iconPylon2"] = new Mesh();
	LoadTexture(m_meshes["iconPylon2"], L"Assets/Textures/Icons/SigmaCircle.dds");

	m_meshes["iconPylon3"] = new Mesh();
	LoadTexture(m_meshes["iconPylon3"], L"Assets/Textures/Icons/OmegaCircle.dds");

	m_meshes["iconSpreadGun"] = new Mesh();
	LoadTexture(m_meshes["iconSpreadGun"], L"Assets/Textures/SpreadGunIcon.dds");

	m_meshes["iconWhipGun"] = new Mesh();
	LoadTexture(m_meshes["iconWhipGun"], L"Assets/Textures/WhipGunIcon.dds");

	m_meshes["iconMissileGun"] = new Mesh();
	LoadTexture(m_meshes["iconMissileGun"], L"Assets/Textures/MissileGunIcon.dds");

	m_meshes["iconOutline"] = new Mesh();
	LoadTexture(m_meshes["iconOutline"], L"Assets/Textures/IconOutline.dds");

	///////////////
	//PYLON ARROWS
	//////////////
	m_meshes["arrowLambda"] = new Mesh();
	LoadBillboard(m_meshes["arrowLambda"], L"Assets/Textures/Icons/LambaArrow.dds", 2.3f);

	m_meshes["arrowLamda Red Glow"] = new Mesh();
	LoadBillboard(m_meshes["arrowLamda Red Glow"], L"Assets/Textures/Glow/RedGlow.dds", 6.0f);

	m_meshes["arrowSigma"] = new Mesh();
	LoadBillboard(m_meshes["arrowSigma"], L"Assets/Textures/Icons/SigmaArrow.dds", 2.5f);

	m_meshes["arrowSigma Red Glow"] = new Mesh();
	LoadBillboard(m_meshes["arrowSigma Red Glow"], L"Assets/Textures/Glow/RedGlow.dds", 6.0f);

	m_meshes["arrowOmega"] = new Mesh();
	LoadBillboard(m_meshes["arrowOmega"], L"Assets/Textures/Icons/OmegaArrow.dds", 2.5f);

	m_meshes["arrowOmega Red Glow"] = new Mesh();
	LoadBillboard(m_meshes["arrowOmega Red Glow"], L"Assets/Textures/Glow/RedGlow.dds", 6.0f);




	m_meshes["Landing Indicator"] = new Mesh();
	LoadBillboard(m_meshes["Landing Indicator"], L"Assets/Textures/Landing.dds", 6.0f);

	/////////////////////
	//MAIN MENU BUTTONS//
	/////////////////////
	m_meshes["menuBackground"] = new Mesh();
	LoadTexture(m_meshes["menuBackground"], L"Assets/Textures/Menu_Background.dds");

	m_meshes["newGameButton"] = new Mesh();
	LoadTexture(m_meshes["newGameButton"], L"Assets/Textures/NewGameButton.dds");

	m_meshes["howToPlayButton"] = new Mesh();
	LoadTexture(m_meshes["howToPlayButton"], L"Assets/Textures/HowToPlayButton.dds");

	m_meshes["quitButton"] = new Mesh();
	LoadTexture(m_meshes["quitButton"], L"Assets/Textures/QuitButton.dds");

	m_meshes["selectButton"] = new Mesh();
	LoadTexture(m_meshes["selectButton"], L"Assets/Textures/SelectButton.dds");

	m_meshes["optionsButton"] = new Mesh();
	LoadTexture(m_meshes["optionsButton"], L"Assets/Textures/OptionsButton.dds");

	m_meshes["loadGameButton"] = new Mesh();
	LoadTexture(m_meshes["loadGameButton"], L"Assets/Textures/LoadGameButton.dds");

	m_meshes["menuNavKeyboardInstr"] = new Mesh();
	LoadTexture(m_meshes["menuNavKeyboardInstr"], L"Assets/Textures/MenuNavigationKeyboardInstructions.dds");

	m_meshes["menuNavGamepadInstr"] = new Mesh();
	LoadTexture(m_meshes["menuNavGamepadInstr"], L"Assets/Textures/MenuNavigationGamepadInstructions.dds");

	m_meshes["newGameButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["newGameButtonSelected"], L"Assets/Textures/NewGameButtonSelected.dds");

	m_meshes["howToPlayButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["howToPlayButtonSelected"], L"Assets/Textures/HowToPlayButtonSelected.dds");

	m_meshes["quitButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["quitButtonSelected"], L"Assets/Textures/QuitButtonSelected.dds");

	m_meshes["optionsButtonSelect"] = new Mesh();
	LoadTexture(m_meshes["optionsButtonSelect"], L"Assets/Textures/OptionsButtonSelected.dds");

	m_meshes["loadGameButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["loadGameButtonSelected"], L"Assets/Textures/LoadGameButtonSelected.dds");

	///////////////////
	//OPTIONS BUTTONS
	//////////////////
	m_meshes["soundOptionsButton"] = new Mesh();
	LoadTexture(m_meshes["soundOptionsButton"], L"Assets/Textures/SoundOptionsButton.dds");

	m_meshes["graphicsOptionsButton"] = new Mesh();
	LoadTexture(m_meshes["graphicsOptionsButton"], L"Assets/Textures/GraphicsOptionsButton.dds");

	m_meshes["keybindingsOptionsButton"] = new Mesh();
	LoadTexture(m_meshes["keybindingsOptionsButton"], L"Assets/Textures/KeybindingsOptionsButton.dds");

	m_meshes["gameModeOptionsButton"] = new Mesh();
	LoadTexture(m_meshes["gameModeOptionsButton"], L"Assets/Textures/GameModeOptionsButton.dds");

	m_meshes["backToMainMenuButton"] = new Mesh();
	LoadTexture(m_meshes["backToMainMenuButton"], L"Assets/Textures/BackToMainMenuButton.dds");

	m_meshes["soundOptionsButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["soundOptionsButtonSelected"], L"Assets/Textures/SoundOptionsButtonSelected.dds");

	m_meshes["graphicsOptionsButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["graphicsOptionsButtonSelected"], L"Assets/Textures/GraphicsOptionsButtonSelected.dds");

	m_meshes["keybindingsOptionsButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["keybindingsOptionsButtonSelected"], L"Assets/Textures/KeybindingsOptionsButtonSelected.dds");

	m_meshes["gameModeOptionsButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["gameModeOptionsButtonSelected"], L"Assets/Textures/GameModeOptionsButtonSelected.dds");

	m_meshes["backToMainMenuButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["backToMainMenuButtonSelected"], L"Assets/Textures/BackToMainMenuButtonSelected.dds");

	///////////////////////////
	//UPGRADE OPTIONS BUTTONS//
	///////////////////////////
	m_meshes["SpreadDmgButton"] = new Mesh();
	LoadTexture(m_meshes["SpreadDmgButton"], L"Assets/Textures/SpreadDmg.dds");

	m_meshes["WhipDmgButton"] = new Mesh();
	LoadTexture(m_meshes["WhipDmgButton"], L"Assets/Textures/WhipDmg.dds");

	////////////////////////
	//SPLASH SCREEN IMAGES
	///////////////////////
	m_meshes["RatsLogo"] = new Mesh();
	LoadTexture(m_meshes["RatsLogo"], L"Assets/Textures/RATSLogo.dds");

	m_meshes["RatsAlpha"] = new Mesh();
	LoadTexture(m_meshes["RatsAlpha"], L"Assets/Textures/RATSAlpha.dds");

	m_meshes["WhiteSquare"] = new Mesh();
	LoadTexture(m_meshes["WhiteSquare"], L"Assets/Textures/WhiteSquare.dds");

	m_meshes["HFGLogo"] = new Mesh();
	LoadTexture(m_meshes["HFGLogo"], L"Assets/Textures/HFGLogo.dds");

	m_meshes["GPGamesLogo"] = new Mesh();
	LoadTexture(m_meshes["GPGamesLogo"], L"Assets/Textures/GpGames.dds");

	////////////////////////
	//SOUND OPTIONS BUTTONS
	///////////////////////
	m_meshes["bgmVolumeButton"] = new Mesh();
	LoadTexture(m_meshes["bgmVolumeButton"], L"Assets/Textures/BGMVolumeButton.dds");

	m_meshes["sfxVolumeButton"] = new Mesh();
	LoadTexture(m_meshes["sfxVolumeButton"], L"Assets/Textures/SFXVolumeButton.dds");

	m_meshes["bgmVolumeButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["bgmVolumeButtonSelected"], L"Assets/Textures/BGMVolumeButtonSelected.dds");

	m_meshes["sfxVolumeButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["sfxVolumeButtonSelected"], L"Assets/Textures/SFXVolumeButtonSelected.dds");

	m_meshes["dialogueVolumeButton"] = new Mesh();
	LoadTexture(m_meshes["dialogueVolumeButton"], L"Assets/Textures/DialogueVolumeButton.dds");

	m_meshes["backButton"] = new Mesh();
	LoadTexture(m_meshes["backButton"], L"Assets/Textures/BackButton.dds");

	m_meshes["backButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["backButtonSelected"], L"Assets/Textures/BackButtonSelected.dds");

	m_meshes["sliderBar"] = new Mesh();
	LoadTexture(m_meshes["sliderBar"], L"Assets/Textures/SliderBar.dds");

	m_meshes["sliderIndicator"] = new Mesh();
	LoadTexture(m_meshes["sliderIndicator"], L"Assets/Textures/SliderIndicator.dds");

	m_meshes["leftArrow"] = new Mesh();
	LoadTexture(m_meshes["leftArrow"], L"Assets/Textures/LeftArrow.dds");

	m_meshes["rightArrow"] = new Mesh();
	LoadTexture(m_meshes["rightArrow"], L"Assets/Textures/RightArrow.dds");

	m_meshes["creditsButton"] = new Mesh();
	LoadTexture(m_meshes["creditsButton"], L"Assets/Textures/CreditsButton.dds");

	m_meshes["inputButton"] = new Mesh();
	LoadTexture(m_meshes["inputButton"], L"Assets/Textures/InputButton.dds");

	m_meshes["creditsButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["creditsButtonSelected"], L"Assets/Textures/CreditsButtonSelected.dds");

	m_meshes["inputButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["inputButtonSelected"], L"Assets/Textures/InputButtonSelected.dds");

	///////////////////////////
	//GRAPHICS OPTIONS BUTTONS
	//////////////////////////
	m_meshes["screenResolutionButton"] = new Mesh();
	LoadTexture(m_meshes["screenResolutionButton"], L"Assets/Textures/ScreenResolutionButton.dds");

	m_meshes["windowModeButton"] = new Mesh();
	LoadTexture(m_meshes["windowModeButton"], L"Assets/Textures/WindowModeButton.dds");

	m_meshes["msaaLevelButton"] = new Mesh();
	LoadTexture(m_meshes["msaaLevelButton"], L"Assets/Textures/MSAALevelButton.dds");

	m_meshes["vsyncButton"] = new Mesh();
	LoadTexture(m_meshes["vsyncButton"], L"Assets/Textures/VSyncButton.dds");

	m_meshes["screenResolutionButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["screenResolutionButtonSelected"], L"Assets/Textures/ScreenResolutionButtonSelected.dds");

	m_meshes["windowModeButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["windowModeButtonSelected"], L"Assets/Textures/WindowModeButtonSelected.dds");

	m_meshes["msaaLevelButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["msaaLevelButtonSelected"], L"Assets/Textures/MSAALevelButtonSelected.dds");

	m_meshes["vsyncButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["vsyncButtonSelected"], L"Assets/Textures/VSyncButtonSelected.dds");

	m_meshes["ApplyButton"] = new Mesh();
	LoadTexture(m_meshes["ApplyButton"], L"Assets/Textures/ApplyButton.dds");

	m_meshes["ApplyButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["ApplyButtonSelected"], L"Assets/Textures/ApplyButtonSelected.dds");

	m_meshes["gammaButton"] = new Mesh();
	LoadTexture(m_meshes["gammaButton"], L"Assets/Textures/GammaButton.dds");

	m_meshes["gammaButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["gammaButtonSelected"], L"Assets/Textures/GammaButtonSelected.dds");

	//////////////////////
	//LOAD SCREEN BUTTONS
	/////////////////////
	m_meshes["slot1Button"] = new Mesh();
	LoadTexture(m_meshes["slot1Button"], L"Assets/Textures/Slot1Button.dds");

	m_meshes["slot2Button"] = new Mesh();
	LoadTexture(m_meshes["slot2Button"], L"Assets/Textures/Slot2Button.dds");

	m_meshes["slot3Button"] = new Mesh();
	LoadTexture(m_meshes["slot3Button"], L"Assets/Textures/Slot3Button.dds");

	////////////////////////////
	//HOW TO PLAY INSTRUCTIONS//
	////////////////////////////

	m_meshes["howToPlayInstructions"] = new Mesh();
	LoadTexture(m_meshes["howToPlayInstructions"], L"Assets/Textures/HowToPlayInstructions.dds");

	m_meshes["instructions1"] = new Mesh();
	LoadTexture(m_meshes["instructions1"], L"Assets/Textures/Instructions1.dds");

	m_meshes["instructions2"] = new Mesh();
	LoadTexture(m_meshes["instructions2"], L"Assets/Textures/Instructions2.dds");

	m_meshes["instructions3"] = new Mesh();
	LoadTexture(m_meshes["instructions3"], L"Assets/Textures/Instructions3.dds");

	m_meshes["instructions4"] = new Mesh();
	LoadTexture(m_meshes["instructions4"], L"Assets/Textures/Instructions4.dds");

	m_meshes["instructions5"] = new Mesh();
	LoadTexture(m_meshes["instructions5"], L"Assets/Textures/Instructions5.dds");

	m_meshes["instructions6"] = new Mesh();
	LoadTexture(m_meshes["instructions6"], L"Assets/Textures/Instructions6.dds");

	m_meshes["instructions7"] = new Mesh();
	LoadTexture(m_meshes["instructions7"], L"Assets/Textures/Instructions7.dds");

	m_meshes["instructions8"] = new Mesh();
	LoadTexture(m_meshes["instructions8"], L"Assets/Textures/Instructions8.dds");

	m_meshes["instructions9"] = new Mesh();
	LoadTexture(m_meshes["instructions9"], L"Assets/Textures/Instructions9.dds");

	m_meshes["nextButton"] = new Mesh();
	LoadTexture(m_meshes["nextButton"], L"Assets/Textures/NextButton.dds");

	m_meshes["nextButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["nextButtonSelected"], L"Assets/Textures/NextButtonSelected.dds");

	m_meshes["previousButton"] = new Mesh();
	LoadTexture(m_meshes["previousButton"], L"Assets/Textures/PreviousButton.dds");

	m_meshes["previousButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["previousButtonSelected"], L"Assets/Textures/PreviousButtonSelected.dds");

	/////////////////////
	//PAUSE MENU BUTTONS
	////////////////////
	m_meshes["resumeGameButton"] = new Mesh();
	LoadTexture(m_meshes["resumeGameButton"], L"Assets/Textures/ResumeGameButton.dds");

	m_meshes["resumeGameButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["resumeGameButtonSelected"], L"Assets/Textures/ResumeGameButtonSelected.dds");

	m_meshes["EmpIcon"] = new Mesh();
	LoadTexture(m_meshes["EmpIcon"], L"Assets/Textures/EmpIcon.dds");

	/////////////////////
	//LEVEL SELECT MENU BUTTONS
	////////////////////
	m_meshes["levelSelectSphereButton"] = new Mesh();
	LoadTexture(m_meshes["levelSelectSphereButton"], L"Assets/Textures/SphereLevelButton.dds");

	m_meshes["levelSelectRoundedCubeButton"] = new Mesh();
	LoadTexture(m_meshes["levelSelectRoundedCubeButton"], L"Assets/Textures/SphereLevelButton.dds");

	m_meshes["levelSelectTorusButton"] = new Mesh();
	LoadTexture(m_meshes["levelSelectTorusButton"], L"Assets/Textures/TorusLevelButton.dds");

	m_meshes["UnlockedLevelButton"] = new Mesh();
	LoadTexture(m_meshes["UnlockedLevelButton"], L"Assets/Textures/UnlockedLevelButton.dds");

	m_meshes["LockedLevelButton"] = new Mesh();
	LoadTexture(m_meshes["LockedLevelButton"], L"Assets/Textures/LockedLevelButton.dds");

	m_meshes["EarnedStar"] = new Mesh();
	LoadTexture(m_meshes["EarnedStar"], L"Assets/Textures/EarnedStar.dds");

	m_meshes["UnearnedStar"] = new Mesh();
	LoadTexture(m_meshes["UnearnedStar"], L"Assets/Textures/UnearnedStar.dds");

	m_meshes["LevelButton"] = new Mesh();
	LoadTexture(m_meshes["LevelButton"], L"Assets/Textures/LevelButton.dds");

	m_meshes["LevelButtonSelected"] = new Mesh();
	LoadTexture(m_meshes["LevelButtonSelected"], L"Assets/Textures/LevelButtonSelected.dds");

	m_meshes["SphereIcon"] = new Mesh();
	LoadTexture(m_meshes["SphereIcon"], L"Assets/Textures/SphereIcon.dds");

	m_meshes["CubeIcon"] = new Mesh();
	LoadTexture(m_meshes["CubeIcon"], L"Assets/Textures/CubeIcon.dds");

	m_meshes["TorusIcon"] = new Mesh();
	LoadTexture(m_meshes["TorusIcon"], L"Assets/Textures/TorusIcon.dds");

	m_meshes["BackgroundStatic"] = new Mesh();
	LoadTexture(m_meshes["BackgroundStatic"], L"Assets/Textures/BackgroundStatic.dds");

	m_meshes["Lock"] = new Mesh();
	LoadTexture(m_meshes["Lock"], L"Assets/Textures/Lock.dds");

	m_meshes["LevelSelectBackground"] = new Mesh();
	LoadTexture(m_meshes["LevelSelectBackground"], L"Assets/Textures/LevelSelectMenuBackground.dds");

	////////////////////
	//ABNORMAL OBJECTS//
	////////////////////
	m_meshes["Effect Swirl"] = new Mesh();
	LoadBillboard(m_meshes["Effect Swirl"], L"Assets/Swirl2.dds", 1.0f);

	m_meshes["Basic Molecule"] = new Mesh();
	LoadBillboard(m_meshes["Basic Molecule"], L"Assets/Textures/Cornelius.dds", 1.5f);




	//////////////////
	//NORMAL OBJECTS//
	//////////////////

	m_meshes["Shield"] = new Mesh();
	LoadModel(m_meshes["Shield"], L"Assets/shield.dds", "Assets/shield.mesh");




	m_meshes["Cube"] = new Mesh();
	LoadModel(m_meshes["Cube"], L"Assets/shield.dds", "Assets/Mesh/Pyramid.mesh");

	m_meshes["Nasa Particle"] = new Mesh();
	LoadBillboard(m_meshes["Nasa Particle"], L"Assets/nasa.dds", 0.5f);



	m_meshes["Player"] = new Mesh();
	LoadModel(m_meshes["Player"], L"Assets/Textures/Player.dds", "Assets/Mesh/Player.mesh");

	m_meshes["Enemy"] = new Mesh();
	LoadModel(m_meshes["Enemy"], L"Assets/Textures/Enemy.dds", "Assets/Mesh/Enemy.mesh");

	m_meshes["Enemy Glow"] = new Mesh();
	LoadBillboard(m_meshes["Enemy Glow"], L"Assets/Textures/Glow/PinkGlow.dds", 7);

	m_meshes["Pylon"] = new Mesh();
	LoadModel(m_meshes["Pylon"], L"Assets/Textures/Pylon.dds", "Assets/Mesh/Pylon.mesh");

	m_meshes["Pylon Sigma"] = new Mesh();
	LoadModel(m_meshes["Pylon Sigma"], L"Assets/Textures/PylonSigma.dds", "Assets/Mesh/PylonSigma.mesh");

	m_meshes["Pylon Omega"] = new Mesh();
	LoadModel(m_meshes["Pylon Omega"], L"Assets/Textures/PylonOmega.dds", "Assets/Mesh/PylonOmega.mesh");

	m_meshes["Fire"] = new Mesh();
	LoadBillboard(m_meshes["Fire"], L"Assets/Textures/Explosion A.dds");

	m_meshes["Spark A"] = new Mesh();
	LoadBillboard(m_meshes["Spark A"], L"Assets/Textures/Sparks A.dds");

	m_meshes["Spark B"] = new Mesh();
	LoadBillboard(m_meshes["Spark B"], L"Assets/Textures/Sparks B.dds");

	m_meshes["Crosshair"] = new Mesh();
	LoadTexture(m_meshes["Crosshair"], L"Assets/Textures/Crosshair.dds");


	m_meshes["Energy"] = new Mesh();
	LoadModel(m_meshes["Energy"], L"Assets/Textures/Energy.dds", "Assets/Mesh/Energy.mesh");

	m_meshes["Energy Glow"] = new Mesh();
	LoadBillboard(m_meshes["Energy Glow"], L"Assets/Textures/Glow/YellowGlow.dds", 6);


	m_meshes["Koi A"] = new Mesh();
	LoadModel(m_meshes["Koi A"], L"Assets/Textures/Koi_A.dds", "Assets/Mesh/Koi_A.mesh");

	m_meshes["Koi A Glow"] = new Mesh();
	LoadBillboard(m_meshes["Koi A Glow"], L"Assets/Textures/Glow/RedGlow.dds", 5);

	m_meshes["Koi D"] = new Mesh();
	LoadModel(m_meshes["Koi D"], L"Assets/Textures/Koi_D.dds", "Assets/Mesh/Koi_D.mesh");

	m_meshes["Koi D Glow"] = new Mesh();
	LoadBillboard(m_meshes["Koi D Glow"], L"Assets/Textures/Glow/YellowGlow.dds", 5);

	m_meshes["Bomber"] = new Mesh();
	LoadModel(m_meshes["Bomber"], L"Assets/Textures/Bomber.dds", "Assets/Mesh/Bomber.mesh");

	m_meshes["Bomber Glow"] = new Mesh();
	LoadBillboard(m_meshes["Bomber Glow"], L"Assets/Textures/Glow/CyanGlow.dds", 5);

	m_meshes["Mine Layer"] = new Mesh();
	LoadModel(m_meshes["Mine Layer"], L"Assets/Textures/MineLayer.dds", "Assets/Mesh/MineLayer.mesh");

	m_meshes["Mine Layer Glow"] = new Mesh();
	LoadBillboard(m_meshes["Mine Layer Glow"], L"Assets/Textures/Glow/PurpleGlow.dds", 6);

	m_meshes["Mine"] = new Mesh();
	LoadModel(m_meshes["Mine"], L"Assets/Textures/Mine.dds", "Assets/Mesh/Mine.mesh");

	m_meshes["Mine Glow"] = new Mesh();
	LoadBillboard(m_meshes["Mine Glow"], L"Assets/Textures/Glow/PurpleGlow.dds", 7);

	m_meshes["Mine Glow2"] = new Mesh();
	LoadBillboard(m_meshes["Mine Glow2"], L"Assets/Textures/Glow/RedGlow.dds", 7);

	m_meshes["Pickup Health"] = new Mesh();
	LoadModel(m_meshes["Pickup Health"], L"Assets/Textures/PickupHealth.dds", "Assets/Mesh/PickupHealth.mesh");

	m_meshes["Pickup Health Glow"] = new Mesh();
	LoadBillboard(m_meshes["Pickup Health Glow"], L"Assets/Textures/Glow/GreenGlow.dds", 6);

	m_meshes["Shield Effect"] = new Mesh();
	LoadModel(m_meshes["Shield Effect"], L"Assets/Textures/ShieldEffect.dds", "Assets/Mesh/ShieldEffect.mesh");

	m_meshes["Boss Shield Effect"] = new Mesh();
	LoadModel(m_meshes["Boss Shield Effect"], L"Assets/Textures/BossShieldEffect.dds", "Assets/Mesh/ShieldEffect.mesh");

	m_meshes["Pickup Shield"] = new Mesh();
	LoadModel(m_meshes["Pickup Shield"], L"Assets/Textures/ShieldEffect.dds", "Assets/Mesh/ShieldPickup.mesh");

	m_meshes["Pickup Shield Glow"] = new Mesh();
	LoadBillboard(m_meshes["Pickup Shield Glow"], L"Assets/Textures/Glow/WhiteGlow.dds", 7);

	m_meshes["Dodger"] = new Mesh();
	LoadModel(m_meshes["Dodger"], L"Assets/Textures/Dodger.dds", "Assets/Mesh/Dodger.mesh");

	m_meshes["Dodger Glow"] = new Mesh();
	LoadBillboard(m_meshes["Dodger Glow"], L"Assets/Textures/Glow/OrangeGlow.dds", 7);

	m_meshes["Goliath"] = new Mesh();
	LoadModel(m_meshes["Goliath"], L"Assets/Textures/Goliath.dds", "Assets/Mesh/Goliath.mesh");

	m_meshes["Goliath Glow"] = new Mesh();
	LoadBillboard(m_meshes["Goliath Glow"], L"Assets/Textures/Glow/YellowGlow.dds", 7);

	m_meshes["Fence Layer"] = new Mesh();
	LoadModel(m_meshes["Fence Layer"], L"Assets/Textures/FenceLayer.dds", "Assets/Mesh/FenceLayer.mesh");

	m_meshes["Fence Layer Glow"] = new Mesh();
	LoadBillboard(m_meshes["Fence Layer Glow"], L"Assets/Textures/Glow/CyanGlow.dds", 7);

	m_meshes["Fence Post"] = new Mesh();
	LoadModel(m_meshes["Fence Post"], L"Assets/Textures/FencePost.dds", "Assets/Mesh/FencePost.mesh");

	m_meshes["Fence Post Glow"] = new Mesh();
	LoadBillboard(m_meshes["Fence Post Glow"], L"Assets/Textures/Glow/CyanGlow.dds", 7);


	m_meshes["Boss"] = new Mesh();
	LoadModel(m_meshes["Boss"], L"Assets/Textures/Boss.dds", "Assets/Mesh/Boss.mesh");

	m_meshes["Boss Glow"] = new Mesh();
	LoadBillboard(m_meshes["Boss Glow"], L"Assets/Textures/Glow/RedGlow.dds", 35);

	m_meshes["Boss Glow2"] = new Mesh();
	LoadBillboard(m_meshes["Boss Glow2"], L"Assets/Textures/Glow/CyanGlow.dds", 35);

	//////////////////
	//Scenery Objects//
	//////////////////

	m_meshes["Asteroid01_Plain"] = new Mesh();
	LoadModel(m_meshes["Asteroid01_Plain"], L"Assets/Textures/Asteroid01.dds", "Assets/Mesh/Asteroid01.mesh");

	m_meshes["Asteroid01_Cyan"] = new Mesh();
	LoadModel(m_meshes["Asteroid01_Cyan"], L"Assets/Textures/Asteroid01_Cyan.dds", "Assets/Mesh/Asteroid01.mesh");

	m_meshes["Asteroid01_Red"] = new Mesh();
	LoadModel(m_meshes["Asteroid01_Red"], L"Assets/Textures/Asteroid01_Red.dds", "Assets/Mesh/Asteroid01.mesh");

	m_meshes["Asteroid02_Ice"] = new Mesh();
	LoadModel(m_meshes["Asteroid02_Ice"], L"Assets/Textures/Asteroid02_Ice.dds", "Assets/Mesh/Asteroid02.mesh");

	m_meshes["Asteroid02_Rock"] = new Mesh();
	LoadModel(m_meshes["Asteroid02_Rock"], L"Assets/Textures/Asteroid02_Rock.dds", "Assets/Mesh/Asteroid02.mesh");

	m_meshes["Planet01"] = new Mesh();
	LoadModel(m_meshes["Planet01"], L"Assets/Textures/Planet01.dds", "Assets/Mesh/Planet01.mesh");

	m_meshes["Planet02"] = new Mesh();
	LoadModel(m_meshes["Planet02"], L"Assets/Textures/Planet02.dds", "Assets/Mesh/Planet02.mesh");

	m_meshes["Planet03_Pink"] = new Mesh();
	LoadModel(m_meshes["Planet03_Pink"], L"Assets/Textures/Planet03_Pink.dds", "Assets/Mesh/Planet03.mesh");

	m_meshes["Planet03_Road"] = new Mesh();
	LoadModel(m_meshes["Planet03_Road"], L"Assets/Textures/Planet03_Road.dds", "Assets/Mesh/Planet03.mesh");

	m_meshes["Planet03_Black"] = new Mesh();
	LoadModel(m_meshes["Planet03_Black"], L"Assets/Textures/Planet03_Black.dds", "Assets/Mesh/Planet03.mesh");

	m_meshes["Planet04_Gas"] = new Mesh();
	LoadModel(m_meshes["Planet04_Gas"], L"Assets/Textures/Planet04_Green.dds", "Assets/Mesh/Planet04.mesh");

	m_meshes["Planet04_Grass"] = new Mesh();
	LoadModel(m_meshes["Planet04_Grass"], L"Assets/Textures/Planet04_Grass.dds", "Assets/Mesh/Planet04.mesh");

	m_meshes["Planet04_Fire"] = new Mesh();
	LoadModel(m_meshes["Planet04_Fire"], L"Assets/Textures/Planet04_Fire.dds", "Assets/Mesh/Planet04.mesh");

	m_meshes["Planet04_Ice"] = new Mesh();
	LoadModel(m_meshes["Planet04_Ice"], L"Assets/Textures/Planet04_Ice.dds", "Assets/Mesh/Planet04.mesh");

	m_meshes["Planet04_Sepia"] = new Mesh();
	LoadModel(m_meshes["Planet04_Sepia"], L"Assets/Textures/Planet04_Sepia.dds", "Assets/Mesh/Planet04.mesh");

}

void AssetManager::LoadTexture(Mesh *object, const wchar_t* textureFileName)
{
	HRESULT hr = CreateDDSTextureFromFile(m_device, textureFileName, 0, &object->m_srv);
}

void AssetManager::LoadBillboard(Mesh* object, const wchar_t* textureFileName, float size)
{

	LoadTexture(object, textureFileName);

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	POSUVNRM_VERTEX* vertices = new POSUVNRM_VERTEX[4];
	UINT* indexes = new UINT[6];
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 2;
	indexes[4] = 1;
	indexes[5] = 3;

	vertices[0].position = XMFLOAT4(-size, size, 0.0f, 1.0f);  // Top left.
	vertices[0].uv = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT4(size, size, 0.0f, 1.0f);   // Top right.
	vertices[1].uv = XMFLOAT2(1.0f, 0.0f);

	vertices[2].position = XMFLOAT4(-size, -size, 0.0f, 1.0f);   // Bottom left.
	vertices[2].uv = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT4(size, -size, 0.0f, 1.0f);   // Bottom right.
	vertices[3].uv = XMFLOAT2(1.0f, 1.0f);

	object->m_vertexArray = vertices;
	object->m_numVertices = 4;

	object->m_indexArray = indexes;
	object->m_numIndices = 6;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(POSUVNRM_VERTEX) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &object->m_vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indexes;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	m_device->CreateBuffer(&indexBufferDesc, &indexData, &object->m_indexBuffer);


	object->m_stride = sizeof(POSUVNRM_VERTEX);

}

void AssetManager::LoadModel(Mesh *object, const wchar_t* textureFileName, const char* modelFileName)
{

	//object = new Mesh();

	object->m_vertexArray = nullptr;
	object->m_indexArray = nullptr;

	HRESULT hr = CreateDDSTextureFromFile(m_device, textureFileName, 0, &object->m_srv);

	fstream fin;

	unsigned int vertexCount;
	unsigned int indexCount;

	UINT* indexArray;
	POSUVNRM_VERTEX * vertexArray;

	fin.open(modelFileName, ios_base::binary | ios_base::in);

	if (fin.is_open())
	{

		unsigned int lengthOfName;
		fin.read((char*)&lengthOfName, 4);

		char* name = new char[lengthOfName];
		fin.read(name, lengthOfName);
		string realName = name;
		delete[] name;

		unsigned int numTextures;
		fin.read((char*)&numTextures, 4);

		for (unsigned int i = 0; i < numTextures; i++)
		{
			unsigned int textNameLen;
			fin.read((char*)&textNameLen, 4);

			char* tempText = new char[textNameLen];
			fin.read(tempText, textNameLen);

			char* fileName = new char[50];
			char* ext = new char[50];
			_splitpath_s(tempText, nullptr, 0, nullptr, 0, fileName, 50, ext, 50);
			string textName = tempText;
			textName = fileName;
			textName = textName + ext;
			delete[] tempText;
			delete[] fileName;
			delete[] ext;
		}

		//Good up to this point

		fin.read((char*)&vertexCount, 4);

		//unsigned int coolInt = vertexCount * 8;

		vertexArray = new POSUVNRM_VERTEX[vertexCount];

		for (unsigned int i = 0; i < vertexCount; i++)
		{
			float posX;
			fin.read((char*)&posX, 4);
			float posY;
			fin.read((char*)&posY, 4);
			float posZ;
			fin.read((char*)&posZ, 4);


			vertexArray[i].position.x = posX;
			vertexArray[i].position.y = posY;
			vertexArray[i].position.z = posZ;
			vertexArray[i].position.w = 1;

			float normX;
			fin.read((char*)&normX, 4);
			float normY;
			fin.read((char*)&normY, 4);
			float normZ;
			fin.read((char*)&normZ, 4);

			vertexArray[i].normal.x = normX;
			vertexArray[i].normal.y = normY;
			vertexArray[i].normal.z = normZ;

			float theU;
			fin.read((char*)&theU, 4);
			float theV;
			fin.read((char*)&theV, 4);

			vertexArray[i].uv.x = theU;
			vertexArray[i].uv.y = theV;
		}

		unsigned int numTriangles = 1337;
		fin.read((char*)&numTriangles, 4);

		unsigned int numIndicies = numTriangles * 3;

		indexArray = new UINT[numIndicies];


		for (unsigned int i = 0; i < numIndicies; i++)
		{
			unsigned int indexX;
			fin.read((char*)&indexX, 4);

			indexArray[i] = indexX;
		}

		indexCount = numIndicies;

		fin.close();
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	object->m_numIndices = indexCount;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indexArray;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	m_device->CreateBuffer(&indexBufferDesc, &indexData, &object->m_indexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(POSUVNRM_VERTEX) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	object->m_numVertices = vertexCount;


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexArray;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &object->m_vertexBuffer);

	object->m_stride = sizeof(POSUVNRM_VERTEX);

	object->m_indexArray = indexArray;
	object->m_vertexArray = vertexArray;

	//delete[] indexArray;
	//delete[] vertexArray;
}

void AssetManager::LoadDynamicModel(Mesh *object, const wchar_t* textureFileName, const char* modelFileName)
{

	//object = new Mesh();

	object->m_vertexArray = nullptr;
	object->m_indexArray = nullptr;

	HRESULT hr = CreateDDSTextureFromFile(m_device, textureFileName, 0, &object->m_srv);

	fstream fin;

	unsigned int vertexCount;
	unsigned int indexCount;

	UINT* indexArray;
	POSUVNRM_VERTEX * vertexArray;

	fin.open(modelFileName, ios_base::binary | ios_base::in);

	if (fin.is_open())
	{

		unsigned int lengthOfName;
		fin.read((char*)&lengthOfName, 4);

		char* name = new char[lengthOfName];
		fin.read(name, lengthOfName);
		string realName = name;
		delete[] name;

		unsigned int numTextures;
		fin.read((char*)&numTextures, 4);

		for (unsigned int i = 0; i < numTextures; i++)
		{
			unsigned int textNameLen;
			fin.read((char*)&textNameLen, 4);

			char* tempText = new char[textNameLen];
			fin.read(tempText, textNameLen);

			char* fileName = new char[50];
			char* ext = new char[50];
			_splitpath_s(tempText, nullptr, 0, nullptr, 0, fileName, 50, ext, 50);
			string textName = tempText;
			textName = fileName;
			textName = textName + ext;
			delete[] tempText;
			delete[] fileName;
			delete[] ext;
		}

		//Good up to this point

		fin.read((char*)&vertexCount, 4);

		//unsigned int coolInt = vertexCount * 8;

		vertexArray = new POSUVNRM_VERTEX[vertexCount];

		for (unsigned int i = 0; i < vertexCount; i++)
		{
			float posX;
			fin.read((char*)&posX, 4);
			float posY;
			fin.read((char*)&posY, 4);
			float posZ;
			fin.read((char*)&posZ, 4);


			vertexArray[i].position.x = posX;
			vertexArray[i].position.y = posY;
			vertexArray[i].position.z = posZ;
			vertexArray[i].position.w = 1;

			float normX;
			fin.read((char*)&normX, 4);
			float normY;
			fin.read((char*)&normY, 4);
			float normZ;
			fin.read((char*)&normZ, 4);

			vertexArray[i].normal.x = normX;
			vertexArray[i].normal.y = normY;
			vertexArray[i].normal.z = normZ;

			float theU;
			fin.read((char*)&theU, 4);
			float theV;
			fin.read((char*)&theV, 4);

			vertexArray[i].uv.x = theU;
			vertexArray[i].uv.y = theV;
		}

		unsigned int numTriangles = 1337;
		fin.read((char*)&numTriangles, 4);

		unsigned int numIndicies = numTriangles * 3;

		indexArray = new UINT[numIndicies];


		for (unsigned int i = 0; i < numIndicies; i++)
		{
			unsigned int indexX;
			fin.read((char*)&indexX, 4);

			indexArray[i] = indexX;
		}

		indexCount = numIndicies;

		fin.close();
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	object->m_numIndices = indexCount;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indexArray;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	m_device->CreateBuffer(&indexBufferDesc, &indexData, &object->m_indexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(POSUVNRM_VERTEX) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	object->m_numVertices = vertexCount;


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexArray;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &object->m_vertexBuffer);

	object->m_stride = sizeof(POSUVNRM_VERTEX);

	object->m_indexArray = indexArray;
	object->m_vertexArray = vertexArray;

	//delete[] indexArray;
	//delete[] vertexArray;
}

void AssetManager::Shutdown()
{

	for (auto iter : m_meshes)
	{

		if (iter.second)
		{
			if (iter.second->m_indexBuffer)
			{
				iter.second->m_indexBuffer->Release();
				iter.second->m_indexBuffer = nullptr;
			}
			if (iter.second->m_vertexBuffer)
			{
				iter.second->m_vertexBuffer->Release();
				iter.second->m_vertexBuffer = nullptr;
			}
			if (iter.second->m_srv)
			{
				iter.second->m_srv->Release();
				iter.second->m_srv = nullptr;
			}
			if (iter.second->m_indexArray)
			{
				delete[] iter.second->m_indexArray;
				iter.second->m_indexArray = nullptr;
			}
			if (iter.second->m_vertexArray)
			{
				delete[] iter.second->m_vertexArray;
				iter.second->m_vertexArray = nullptr;
			}

			delete iter.second;
			iter.second = nullptr;
		}



	}
}