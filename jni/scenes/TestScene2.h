#pragma once

#include <defines.h>
#include <SceneManaging\BaseScene.h>
#include <Logger.h>
#include <Context.h>
#include <SceneGraph/Object.h>
#include <StarComponents.h>
#include <Shaders/Shader.h>
#include <Objects/QuadCol.h>
#include <Input/XMLContainer.h>
#include <Objects/Sprite.h>

class TestScene2 : public star::BaseScene
{
public:
	TestScene2(const tstring& name);
	virtual ~TestScene2() {}

	status Initialize(const star::Context& context);
	status Update(const star::Context& context);
	status Draw();

private:
	int m_TotalFrames;
	int m_Step;
	int m_FPS;
	float m_PassedMiliseconds;
	star::Sprite* m_pTestSprite;
	star::XMLContainer mTestXMLFile;

	star::Object	*m_pObjectOne, 
					*m_pObjectTwo, 
					*m_pObjectThree,
					*m_pObjectFour, 
					*m_pObjectFive, 
					*m_pObjectSix;

	star::RectangleColliderComponent* m_pRectCompOne;
	star::CircleColliderComponent *m_pRectCompTwo;

	star::PathFindNodeComponent	*m_pPathFindComp, 
								*m_pPathFindCompTwo, 
								*m_pPathFindCompThree, 
								*m_pPathFindCompFour, 
								*m_pPathFindCompFive, 
								*m_pPathFindCompSix;
		
	TestScene2(const TestScene2& t);
	TestScene2(TestScene2&& t);
	TestScene2& operator=(const TestScene2& t);
};
