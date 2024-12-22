#include "DialogueBox.h"

USING_NS_CC;

DialogueBox* DialogueBox::create(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName, int heartPoints)
{
    DialogueBox* dialogueBox = new (std::nothrow) DialogueBox();
    if (dialogueBox && dialogueBox->init(dialogue, characterImagePath, characterName, heartPoints))
    {
        dialogueBox->autorelease();
        return dialogueBox;
    }
    CC_SAFE_DELETE(dialogueBox);
    return nullptr;
}
void DialogueBox::startTypingEffect(Label* label, const std::string& dialogue)
{
    // 当前显示的字符索引
    size_t currentIndex = 0;
    // 清空标签的文本
    label->setString("");
    // 定义一个调度器
    auto typingCallback = [=](float dt) mutable {
        if (currentIndex < dialogue.size()) {
            // 逐渐显示字符
            std::string currentText = label->getString(); // 获取当前文本
            currentText += dialogue[currentIndex]; // 添加下一个字符
            label->setString(currentText); // 更新标签文本
            currentIndex++;
        }
        else {
            // 停止调度
            this->unschedule("typing");
        }
        };

    // 每隔一定时间调用一次
    this->schedule(typingCallback, 0.08f, "typing"); // 0.08秒显示一个字符
}

bool DialogueBox::init(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName, int heartPoints)
{
    if (!Node::init())
    {
        return false;
    }
    this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f)); // 设置锚点
    this->setScale(1.0);

    // 获取屏幕的可见尺寸
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 创建背景
    auto background = Sprite::create("LooseSprites/textBox.png");
    this->setPosition(Vec2(visibleSize.width / 2, 50)); // 设置在屏幕底部中心
    this->setScale(4.0);
    this->addChild(background);

    // 创建角色头像
    auto characterImage = Sprite::create(characterImagePath);
    characterImage->setPosition(Vec2(70, 6)); // 根据需要调整位置
    characterImage->setTextureRect(cocos2d::Rect(0, 0, 64, 64));
    characterImage->setScale(0.4f);
    this->addChild(characterImage);

    // 创建姓名文本
    auto nameLabel = Label::createWithSystemFont(characterName, "Times New Roman", 10);
    nameLabel->setTextColor(cocos2d::Color4B(0, 0, 0, 255)); // 黑色，RGBA格式
    nameLabel->setPosition(Vec2(70, -11)); // 根据需要调整位置
    this->addChild(nameLabel);

    // 创建对话文本
    auto label = Label::createWithSystemFont(dialogue, "Arial", 10);
    label->setDimensions(120, 0); // 设置最大宽度并启用自动换行
    label->setLineBreakWithoutSpace(true); // 不允许在单词中间换行
    this->addChild(label);

    // 创建好感度文本
    auto heartLabel = Label::createWithSystemFont(std::to_string(heartPoints) + " / 10", "Arial", 8);
    heartLabel->setTextColor(cocos2d::Color4B(0, 0, 0, 255)); // 黑色，RGBA格式
    heartLabel->setPosition(Vec2(82, 20)); // 根据需要调整位置
    this->addChild(heartLabel);
    
    // 开始打字效果
    label->setTextColor(cocos2d::Color4B(0, 0, 0, 255)); // 黑色，RGBA格式
    label->setPosition(Vec2(-20, 0)); // 根据需要调整位置
    startTypingEffect(label, dialogue);

    // 创建鼠标监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [this](Event* event) {
        this->close(); // 点击时关闭对话框
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // 添加出现动画
    this->setScale(3.7f); // 初始缩放为3.7
    auto appearAction = ScaleTo::create(0.3f, 4.0f); // 0.3秒内缩放到4.0
    auto fadeInAction = FadeIn::create(0.3f); // 0.3秒内淡入
    auto spawnAction = Spawn::create(appearAction, fadeInAction, nullptr); // 同时执行缩放和淡入
    this->runAction(spawnAction); // 运行组合动画
    
    return true;
}

void DialogueBox::close()
{
    CCLOG("Closing dialogue box...");
    // 添加关闭动画
    this->runAction(Sequence::create(
        ScaleTo::create(0.3f, 0.1f), // 0.3秒内缩放到0.1
        CallFunc::create([this]() {
            CCLOG("Dialogue box removed from parent.");
            this->setVisible(false); // 动画结束后移除对话框

            }),
        nullptr));
}
