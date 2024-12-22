#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h" // 确保包含 Cocos UI 组件
#include <vector>
#include <cstdlib>

class DialogueBox : public cocos2d::Node
{
public:
    static DialogueBox* create(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName, int heartPoints);
    virtual bool init(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName, int heartPoints);
    void close();// 关闭
private:
    void startTypingEffect(cocos2d::Label* label, const std::string& dialogue);// 打字效果
    cocos2d::ui::Button* confirmButton; // 确认按钮
};
