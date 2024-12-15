// StartScene.cpp
#include "StartScene.h"
#include "GameScene.h"
#include"CharacterCreateScene.h"

USING_NS_CC;

Scene* StartScene::createScene() {
    return StartScene::create();
}

bool StartScene::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建并添加开场图片
    auto startImage = Sprite::create("start.png");  // 确保在Resources文件夹中有start.png
    if (startImage) {
        // 将图片放在屏幕中央
        startImage->setPosition(Vec2(visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y));

        // 调整图片大小以适应屏幕
        float scaleX = visibleSize.width / startImage->getContentSize().width;
        float scaleY = visibleSize.height / startImage->getContentSize().height;
        startImage->setScale(scaleX, scaleY);

        this->addChild(startImage);
    }

    // 设置触摸监听
    setupTouchListener();

    return true;
}

void StartScene::setupTouchListener() {
    auto listener = EventListenerTouchOneByOne::create();

    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        // 点击任意位置后切换场景
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(StartScene::switchToGameScene), 0.1f);
        return true;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void StartScene::switchToGameScene(float dt) {
    // 创建游戏场景并切换
    auto gameScene = CharacterCreateScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, gameScene));
}