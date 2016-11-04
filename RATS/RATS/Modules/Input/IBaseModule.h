#pragma once

#include "Interface.h"
//--------------------------------------------------------------------------------
CREATE_INTERFACE(IBaseModule) {
	virtual ~IBaseModule() {}
	virtual bool Init() = 0;
	virtual void Update(float _delta) = 0;
	virtual bool ShutDown() = 0;
};