#pragma once
#pragma once
#ifndef __BALL__
#define __BALL__

#include "Sprite.h"

class Ball final : public Sprite
{
public:
	Ball();
	~Ball();

	// Life Cycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

private:

};

#endif /* defined (__BALL__) */
