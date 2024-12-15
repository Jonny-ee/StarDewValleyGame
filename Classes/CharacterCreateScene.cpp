// CharacterCreateScene.cpp
#include "CharacterCreateScene.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

Scene* CharacterCreateScene::createScene() {
    return CharacterCreateScene::create();
}

bool CharacterCreateScene::init() {
    if (!Scene::init()) {
        return false;
    }

    initUI();
    return true;
}

void CharacterCreateScene::initUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建背景
    auto bg = LayerColor::create(Color4B(245, 245, 245, 255));
    this->addChild(bg);

    // 创建标题
    auto titleLabel = Label::createWithTTF("创建角色", "fonts/Chinese.fnt", 36);
    titleLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height * 0.8f + origin.y));
    this->addChild(titleLabel);

    // 创建名字输入框背景
    auto inputBg = Sprite::create("input_bg.png");  // 需要准备一个输入框背景图片
    inputBg->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height * 0.6f + origin.y));
    this->addChild(inputBg);

    // 创建输入框
    _nameInput = EditBox::create(Size(200, 40), Scale9Sprite::create("input_bg.png"));
    _nameInput->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height * 0.6f + origin.y));
    _nameInput->setPlaceHolder("请输入角色名称");
    _nameInput->setMaxLength(12);
    _nameInput->setFontSize(20);
    _nameInput->setFontColor(Color3B::BLACK);
    this->addChild(_nameInput);

    // 创建确认按钮
    _confirmButton = Button::create("button_normal.png", "button_pressed.png");
    _confirmButton->setTitleText("确认");
    _confirmButton->setTitleFontSize(20);
    _confirmButton->setPosition(Vec2(visibleSize.width / 2 + origin.x,
        visibleSize.height * 0.4f + origin.y));

    _confirmButton->addClickEventListener([this](Ref* sender) {
        this->onConfirmClicked();
        });

    this->addChild(_confirmButton);
}

void CharacterCreateScene::onConfirmClicked() {
    std::string name = _nameInput->getText();

    if (name.empty()) {
        // 显示提示：名字不能为空
        auto label = Label::createWithTTF("名字不能为空！", "fonts/Chinese.fnt", 24);
        label->setPosition(Director::getInstance()->getVisibleSize() / 2);
        this->addChild(label);
        label->runAction(Sequence::create(
            DelayTime::create(1.0f),
            FadeOut::create(1.0f),
            RemoveSelf::create(),
            nullptr
        ));
        return;
    }

    // 保存角色名称
    _playerName = name;

    // 切换到游戏场景
    auto gameScene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, gameScene));
}