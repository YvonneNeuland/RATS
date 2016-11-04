#pragma once
class BaseObject
{
public:
	BaseObject();
	virtual ~BaseObject( ) = 0;
	virtual void Update(float dt) = 0;

private:
	BaseObject(const BaseObject&) = delete;
	BaseObject(const BaseObject&&) = delete;
	BaseObject& operator=(const BaseObject&) = delete;
	BaseObject& operator=(const BaseObject&&) = delete;
};

