#pragma once

class BaseDialog
{
public:
	BaseDialog() = default;
	virtual ~BaseDialog() = default;

	virtual void update() = 0;
	virtual void draw() const = 0;

	virtual bool isActive() const = 0;
	virtual void close() = 0;
};
