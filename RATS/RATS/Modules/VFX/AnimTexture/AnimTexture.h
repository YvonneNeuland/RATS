#pragma once

#include "../VisualEffect/VisualEffect.h"

class AnimTexture : public VisualEffect
{
private:

	//The UVs of the verts, in order of: (allan please add order)
	XMFLOAT2 m_vertUVs[4];

	// How many images are on the X and Y?
	int m_numPanelsX, m_numPanelsY;

	// What's the current X and Y panel?
	int m_currPanelX = 0, m_currPanelY = 0;

	// the dimensions of each panel, for adding purposes
	float m_panelWidth, m_panelHeight;

	// Does the animtex automatically snap to billboarding the camera?
	bool m_snapToCamera;

	// A pointer to the camera.
	GameObject* m_cam;

	// Length of time it takes to flip panels
	float m_flipTime;

	// current time
	float m_currTime = 0;

	// How many times will the animation loop?
	int m_timesToLoop;

	// Is the animation done looping?
	bool m_isDone = false;

	// The scale of the texture
	float m_scale = 5.0f;

	// Target used as location to play this effect
	GameObject* m_parent;

protected:
public:
	AnimTexture(int panelsX, int panelsY, int timesToLoop = -1, float flipTime = 0.0625f, bool billboardToCam = true);
	~AnimTexture();

	void Update(float dt);

	const Direction* GetBillboardDirection() { return GetTransform().GetZAxis(); }

	//REMEMBER TO NORMALIZE THE DIRECTION
	void SetBillboardDirection(Direction dir);

	bool& GrabSnapToCam() { return m_snapToCamera; }

	GameObject* GrabCameraPointer() { return m_cam; }
	void SetCamera(GameObject* cam) { m_cam = cam;}

	void SetPanels(int numX, int numY)
	{
		m_currPanelX = m_currPanelY = 0;
		m_numPanelsX = numX;
		m_numPanelsY = numY;
		m_panelWidth = 1.0f / m_numPanelsX;
		m_panelHeight = 1.0f / m_numPanelsY;
	}

	XMFLOAT2* GrabUVs(){ return m_vertUVs; }
	XMFLOAT2 GrabUV(int index) { return m_vertUVs[index]; }

	int& GrabTimesToLoop(){ return m_timesToLoop; }

	bool isDone() { return m_isDone; }

	float& GrabScale() { return m_scale; }

	void SetParent(GameObject* set) { m_parent = set; }
	GameObject* GetParent() { return m_parent; }

	void Reset();
};