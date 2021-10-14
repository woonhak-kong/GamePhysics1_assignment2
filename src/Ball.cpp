#include "Ball.h"
#include "TextureManager.h"

Ball::Ball()
{
	TextureManager::Instance().load("../Assets/textures/ball.png", "ball");

	const auto size = TextureManager::Instance().getTextureSize("ball");
	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(100.0f, 100.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	setType(TARGET);
}

Ball::~Ball()
{

}

void Ball::draw()
{
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	TextureManager::Instance().draw("ball", x, y, 0, 255, true);
}

void Ball::update()
{
}

void Ball::clean()
{
}
