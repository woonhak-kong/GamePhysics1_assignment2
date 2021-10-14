#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include <sstream>
#include <iomanip>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"
#include <cassert>

PlayScene::PlayScene() :
	m_startingX(50.f),
	m_startingY(600.f),
	m_startingXAfterHit(0.0f),
	m_launchElevationAngle(45.f),
	m_launchSpeed(100.f),
	m_accelerationGravity({ 0.f, 9.8f }),
	m_orientation({ 0.5,0.5 }),
	m_isStart(false),
	m_playTime(0),
	m_playTimeAfterHittingGround(0),
	m_massKg(10),
	m_kFrictionCoefficientFloor(0.1f),
	m_isOnTheGround(false),
	m_instantaneousVelocity({ 0, 0 }),
	m_forceFriction({ 0.0f, 0.0f }),
	m_rampHeight(200),
	m_rampWidth(400),
	m_groundHeight(600),
	m_offsetRampPosition(30),
	m_degreeOfRamp(0)

{
	PlayScene::start();

	m_pBall->getTransform()->position.x = m_startingX;
	m_pBall->getTransform()->position.y = m_startingY;

}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	/*TextureManager::Instance().drawBySize("bg", 0, 0, 1280, 720);
	TextureManager::Instance().drawBySize("bg2", 0, 600, 1280, 120);*/


	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 0, 255, 255);
	SDL_RenderDrawLineF(Renderer::Instance().getRenderer(), 0, m_groundHeight, 1280, m_groundHeight);
	SDL_RenderDrawLineF(Renderer::Instance().getRenderer(), 0+ m_offsetRampPosition, m_groundHeight, 0 + m_offsetRampPosition, m_groundHeight - m_rampHeight);
	SDL_RenderDrawLineF(Renderer::Instance().getRenderer(), 0 + m_rampWidth, m_groundHeight, 0 + m_offsetRampPosition, m_groundHeight - m_rampHeight);





	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 0, 255);
	float beforeX = m_startingX;
	float beforeY = m_startingY;
	float x = 0;
	float y = 0;




	drawDisplayList();



	if (m_isStart)
	{
		//drawing instantaneous velocity vector as a red line
		glm::vec2 velocityVector = glm::normalize(glm::vec2(m_instantaneousVelocity.x, m_instantaneousVelocity.y));
		//std::cout << velocityVector.x << ", " << velocityVector.y << std::endl;
		float lineDistance = 200.0f;
		velocityVector.x = m_pBall->getTransform()->position.x + velocityVector.x * lineDistance;
		velocityVector.y = m_pBall->getTransform()->position.y + velocityVector.y * lineDistance;
		SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 0, 0, 255);
		SDL_RenderDrawLineF(Renderer::Instance().getRenderer(), m_pBall->getTransform()->position.x, m_pBall->getTransform()->position.y, velocityVector.x, velocityVector.y);

		//drawing Acceleration vector as a blue line
		glm::vec2 accelerationVector = glm::normalize(glm::vec2(m_accelerationGravity.x, m_accelerationGravity.y));
		//std::cout << velocityVector.x << ", " << velocityVector.y << std::endl;
		accelerationVector.x = m_pBall->getTransform()->position.x + accelerationVector.x * lineDistance;
		accelerationVector.y = m_pBall->getTransform()->position.y + accelerationVector.y * lineDistance;
		SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 255, 255);
		SDL_RenderDrawLineF(Renderer::Instance().getRenderer(), m_pBall->getTransform()->position.x, m_pBall->getTransform()->position.y, accelerationVector.x, accelerationVector.y);
	}

	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 0, 255);
}

void PlayScene::update()
{

	// calculating Displacement
	if (m_isStart)
	{
		float t = Game::Instance().getDeltaTime() * 10;
		m_playTime += t;
		//std::cout << t << std::endl;


		// kinemetic equation
		// Displacement = D(i) + vt + 1/2at^2

		float newPositionX = 0;
		float newPositionY = 0;
		if (!m_isOnTheGround)
		{
			newPositionX = m_startingX + m_startingVelocity.x * m_playTime
				+ (0.5f * m_accelerationGravity.x * (m_playTime * m_playTime));
			newPositionY = m_startingY + m_startingVelocity.y * m_playTime
				+ (0.5f * m_accelerationGravity.y * (m_playTime * m_playTime));
		}
		else
		{
			m_playTimeAfterHittingGround += t;
			newPositionX = m_startingXAfterHit + m_startingVelocity.x * m_playTimeAfterHittingGround
				+ (0.5f * m_forceFriction.x * (m_playTimeAfterHittingGround * m_playTimeAfterHittingGround));
		}


		// if hit the ground
		if (!m_isOnTheGround)
		{
			if (newPositionY >= m_startingY)
			{
				std::cout << "hit the ground" << std::endl;
				m_startingXAfterHit = newPositionX;
				m_startingVelocity.x = m_instantaneousVelocity.x;
				m_isOnTheGround = true;
			}
		}


		if (!m_isOnTheGround)
		{
			m_pBall->getTransform()->position.y = newPositionY;
		}
		else
		{
			m_pBall->getTransform()->position.y = m_startingY;

			// for finding friction acceleration
			//glm::vec2 frictionAcceleration;
			// friction acceleration = N * coefficient / mass
			float xAcceleration = (m_kFrictionCoefficientFloor * abs(m_accelerationGravity.y * m_massKg)) * (-1 * (m_instantaneousVelocity.x / abs(m_instantaneousVelocity.x)))
			/ m_massKg;
			m_forceFriction.x = xAcceleration;
			//std::cout << xAcceleration << std::endl;
		}

		m_pBall->getTransform()->position.x = newPositionX;

		////////////////////// calculation of maxValue for finding time.
		// maximum value of positionX
		float a = 0.5f * m_forceFriction.x;
		float b = m_startingVelocity.x;
		float c = m_startingXAfterHit;

		float maxValue = c - ((b * b) / (4 * a)) - 0.3 /* -0.3 is for that sometimes maxValue can be over graph because of decimal calculation */;

		//std::cout << maxValue << std::endl;

		// finding time when X is max;
		c -= maxValue;

		float timeOfmax[2] {0,};

		quadraticFormula(a, b, c, timeOfmax);
		//std::cout << timeOfmax[0] << std::endl;
		//std::cout << timeOfmax[1] << std::endl;

		// choosing positive value;
		double finalMaxTime = timeOfmax[0] < timeOfmax[1] ? timeOfmax[0] : timeOfmax[1];

		if (m_playTimeAfterHittingGround > finalMaxTime)
		{
			m_isStart = false;
		}
		//////////////////////////////
	}
	std::stringstream string;
	string << std::fixed << std::setprecision(2) << "Posision : (" << m_pBall->getTransform()->position.x << ", "
		<< m_pBall->getTransform()->position.y << ")";
	m_positionLabel->setText(string.str());

	string.str("");
	string << std::fixed << std::setprecision(2) << "Luanch Speed : " << m_launchSpeed;
	m_speedLabel->setText(string.str());

	// calculating velocity
	m_instantaneousVelocity.x = m_isStart ? (m_isOnTheGround ? m_startingVelocity.x + m_forceFriction.x * m_playTimeAfterHittingGround : m_startingVelocity.x) : 0;
	m_instantaneousVelocity.y = m_isStart ? (m_isOnTheGround ? 0 : m_startingVelocity.y + m_accelerationGravity.y * m_playTime) : 0;
	string.str("");
	string << std::fixed << std::setprecision(2) << "Velocity : (" << m_instantaneousVelocity.x << ", " << m_instantaneousVelocity.y << ")";
	m_velocityLabel->setText(string.str());


	string.str("");
	string << std::fixed << std::setprecision(2) << "G Acceleration : " << "(" << m_accelerationGravity.x << ", " << m_accelerationGravity.y << ")";
	m_accelerationLabel->setText(string.str());

	string.str("");
	string << std::fixed << std::setprecision(2) << "Time : " << m_playTime;
	m_timeLabel->setText(string.str());

	string.str("");
	string << std::fixed << std::setprecision(2) << "Angle : " << m_launchElevationAngle;
	m_angleLabel->setText(string.str());

	string.str("");
	string << std::fixed << std::setprecision(2) << "Mass : " << m_massKg << " Kg";
	m_massLabel->setText(string.str());

	string.str("");
	string << std::fixed << std::setprecision(2) << "Friction Acceleration : " << "(" << m_forceFriction.x << ", " << m_forceFriction.y << ")";
	m_forceFrictionAcceleration->setText(string.str());

	string.str("");
	string << std::fixed << std::setprecision(2) << "Coefficient : " << m_kFrictionCoefficientFloor;
	m_coefficientOfFrictionFloor->setText(string.str());




	updateDisplayList();
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();



	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance().changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance().changeSceneState(END_SCENE);
	}

	//if (EventManager::Instance().isKeyDown(SDL_SCANCODE_SPACE))
	//{
	//	reset();
	//	m_isStart = true;
	//}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	/*TextureManager::Instance().load("../Assets/textures/bg1.png", "bg");
	TextureManager::Instance().load("../Assets/textures/bg2.png", "bg2");*/

	const SDL_Color blue = { 0, 255, 255, 255 };
	float yCoor = 10.f;
	float xCoor = 700.0f;
	m_positionLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_positionLabel->setParent(this);
	addChild(m_positionLabel);
	m_speedLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_speedLabel->setParent(this);
	addChild(m_speedLabel);
	m_velocityLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_velocityLabel->setParent(this);
	addChild(m_velocityLabel);
	m_accelerationLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_accelerationLabel->setParent(this);
	addChild(m_accelerationLabel);
	m_timeLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_timeLabel->setParent(this);
	addChild(m_timeLabel);
	m_angleLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_angleLabel->setParent(this);
	addChild(m_angleLabel);
	m_massLabel = new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_massLabel->setParent(this);
	addChild(m_massLabel);
	m_forceFrictionAcceleration= new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_forceFrictionAcceleration->setParent(this);
	addChild(m_forceFrictionAcceleration);
	m_coefficientOfFrictionFloor= new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_coefficientOfFrictionFloor->setParent(this);
	addChild(m_coefficientOfFrictionFloor);
	m_rampDegreeLabel= new Label("", "Consolas", 20, blue, glm::vec2(xCoor, yCoor += 20));
	m_rampDegreeLabel->setParent(this);
	addChild(m_rampDegreeLabel);






	// Plane Sprite
	m_pBall = new Ball();
	addChild(m_pBall);



	m_pBall->getTransform()->position.x = m_startingX;
	m_pBall->getTransform()->position.y = m_startingY;
	m_orientation.x = glm::cos(m_launchElevationAngle * Util::Deg2Rad);
	m_orientation.y = -glm::sin(m_launchElevationAngle * Util::Deg2Rad);
	m_startingVelocity.x = m_orientation.x * m_launchSpeed;
	m_startingVelocity.y = m_orientation.y * m_launchSpeed;



	m_degreeOfRamp = atan((float)m_rampHeight / m_rampWidth) * Util::Rad2Deg;
	m_rampDegreeLabel->getTransform()->position = glm::vec2(m_offsetRampPosition + m_rampWidth - 100, m_groundHeight + 20);
	std::stringstream string;
	string << std::fixed << std::setprecision(2) << "Degree : " << m_degreeOfRamp;
	m_rampDegreeLabel->setText(string.str());
	setInitBall();


	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Setting values", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);


	if (ImGui::Button("Activate"))
	{
		reset();
		m_isStart = true;
	}
	//if(ImGui::SliderFloat("Starting X", &m_startingX, 0.f, 1280.f))
	//{
	//	//m_startingX = startingX;
	//	m_pBall->getTransform()->position.x = m_startingX;
	//}
	//if (ImGui::SliderFloat("GroundHeight", &m_startingY, 0.f, 720))
	//{
	//	//m_startingY = startingY;
	//	m_pBall->getTransform()->position.y = m_startingY;
	//}
	//if (ImGui::SliderFloat("GroundPosition", &m_startingX, 0.f, 1280.f))
	//{
	//	//m_startingY = startingY;
	//	m_pBall->getTransform()->position.x = m_startingX;
	//}


	if (ImGui::SliderInt("Ramp Height", &m_rampHeight, 200, 500))
	{
		m_degreeOfRamp = atan((float)m_rampHeight / m_rampWidth) * Util::Rad2Deg;
		m_rampDegreeLabel->getTransform()->position = glm::vec2(m_offsetRampPosition + m_rampWidth - 100, m_groundHeight + 20);
		std::stringstream string;
		string << std::fixed << std::setprecision(2) << "Degree : " << m_degreeOfRamp;
		m_rampDegreeLabel->setText(string.str());
		setInitBall();
	}
	if (ImGui::SliderInt("Ramp Width", &m_rampWidth, 400, 800))
	{
		m_degreeOfRamp = atan((float)m_rampHeight / m_rampWidth) * Util::Rad2Deg;
		m_rampDegreeLabel->getTransform()->position = glm::vec2(m_offsetRampPosition + m_rampWidth - 100, m_groundHeight + 20);
		std::stringstream string;
		string << std::fixed << std::setprecision(2) << "Degree : " << m_degreeOfRamp;
		m_rampDegreeLabel->setText(string.str());
		setInitBall();
	}


	//if (ImGui::SliderFloat("Launch Speed", &m_launchSpeed, 10.f, 200.f))
	//{
	//	//m_launchSpeed = launchSpeed;
	//	m_startingVelocity.x = m_orientation.x * m_launchSpeed;
	//	m_startingVelocity.y = m_orientation.y * m_launchSpeed;
	//}
	//if (ImGui::SliderFloat("Angle Y", &m_launchElevationAngle, -90.f, 90.f))
	//{
	//	// for calculating launch velocity
	//	m_orientation.x = glm::cos(m_launchElevationAngle * Util::Deg2Rad);
	//	m_orientation.y = -glm::sin(m_launchElevationAngle * Util::Deg2Rad);
	//	m_startingVelocity.x = m_orientation.x * m_launchSpeed;
	//	m_startingVelocity.y = m_orientation.y * m_launchSpeed;
	//	std::cout << m_orientation.x << ", " << m_orientation.y << std::endl;
	//}
	if (ImGui::SliderFloat("Gravity", &m_accelerationGravity.y, 1.f, 20.f))
	{
		//m_accelerationGravity.y = accelationGravity;
		//std::cout << "acceleration - (" << m_accelerationGravity.x << ", " << m_accelerationGravity.y << ")" << std::endl;
	}
	if (ImGui::SliderFloat("Mass", &m_massKg, 1.f, 1000.f))
	{
		//m_accelerationGravity.y = accelationGravity;
		std::cout << "mass = " << m_massKg << std::endl;
	}
	if (ImGui::SliderFloat("coefficient of friction Floor", &m_kFrictionCoefficientFloor, 0.1f, 1.0f))
	{
		//m_accelerationGravity.y = accelationGravity;
		std::cout << "coefficient = " << m_kFrictionCoefficientFloor << std::endl;
	}

	if (ImGui::Button("Reset"))
	{
		reset();
	}


	//static float float3[3] = { 0.0f, 1.0f, 1.5f };
	//if(ImGui::SliderFloat3("My Slider", float3, 0.0f, 2.0f))
	//{
	//	std::cout << float3[0] << std::endl;
	//	std::cout << float3[1] << std::endl;
	//	std::cout << float3[2] << std::endl;
	//	std::cout << "---------------------------\n";
	//}

	ImGui::End();
}

void PlayScene::reset()
{

	m_isStart = false;
	m_isOnTheGround = false;
	m_playTime = 0;
	m_playTimeAfterHittingGround = 0;
	m_startingXAfterHit = 0.0f;
	m_forceFriction = { 0, 0 };
	setInitBall();
}

void PlayScene::quadraticFormula(float a, float b, float c, float array[])
{
	assert(array != nullptr);
	//a = (std::round(a * 100)) / 100.0;
	//b = (std::round(b* 100)) / 100.0;
	//c = (std::round(c * 100)) / 100.0;
	//std::cout << (b * b) - (4 * a * c) << std::endl;
	array[0] = ((-1 * b) + sqrt((b * b) - (4 * a * c))) / (2 * a);
	array[1] = ((-1 * b) - sqrt((b * b) - (4 * a * c))) / (2 * a);
}

void PlayScene::setInitBall()
{

	m_pBall->getTransform()->position.x = m_offsetRampPosition + 10;
	m_pBall->getTransform()->position.y = m_groundHeight - m_rampHeight - m_pBall->getHeight() / 2;
	m_startingX = m_pBall->getTransform()->position.x;
	m_startingY = m_pBall->getTransform()->position.y;
	m_pBall->setCurrentHeading(m_degreeOfRamp);
}
