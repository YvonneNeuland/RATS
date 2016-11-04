#pragma once
#include "BaseComponent.h"

// Has owner pointer from base class, allowing for normal access to transform
// String for render_info
class GlowComponent :
	public BaseComponent
{
public:
	GlowComponent();
	~GlowComponent();

	// Necessary virtual function
	void Update(float dt);

	// In case this class gets big enough to merit such a thing
	// Now mandatory to have Reset
	void Reset();

	// Normal accessor stuff
	void SetGlowID(std::string id) { m_szGlowID = id; }
	std::string GetGlowID() { return m_szGlowID; }

private:

	std::string m_szGlowID;
};

