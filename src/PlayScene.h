#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Ball.h"
#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;

	glm::vec2 m_mousePosition;

	Ball* m_pBall;


	float m_startingX;
	float m_startingY;
	float m_startingXAfterHit;
	float m_launchElevationAngle;
	float m_launchSpeed;
	float m_playTime;
	float m_playTimeAfterHittingGround;
	float m_massKg;
	float m_kFrictionCoefficientFloor;
	float m_degreeOfRamp;
	int m_rampHeight;
	int m_rampWidth;
	int m_groundHeight;
	int m_offsetRampPosition;
	glm::vec2 m_accelerationGravity;
	glm::vec2 m_orientation;
	glm::vec2 m_startingVelocity;
	glm::vec2 m_instantaneousVelocity;
	glm::vec2 m_forceFriction;

	bool m_isStart;
	bool m_isOnTheGround;

	Label* m_positionLabel;
	Label* m_speedLabel;
	Label* m_velocityLabel;
	Label* m_accelerationLabel;
	Label* m_timeLabel;
	Label* m_angleLabel;
	Label* m_massLabel;
	Label* m_coefficientOfFrictionFloor;
	Label* m_forceFrictionAcceleration;
	Label* m_rampDegreeLabel;




private:
	void reset();
	void quadraticFormula(float a, float b, float c, float array[]);
};

#endif /* defined (__PLAY_SCENE__) */