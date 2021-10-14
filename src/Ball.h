#pragma once
#pragma once
#ifndef __BALL__
#define __BALL__

#include "Agent.h"

class Ball final : public Agent
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
