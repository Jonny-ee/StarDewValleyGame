#pragma once
#include "cocos2d.h"
#include <vector>
#include <cstdlib>

class DialogueBox : public cocos2d::Node
{
public:
    static DialogueBox* create(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName);
    virtual bool init(const std::string& dialogue, const std::string& characterImagePath, const std::string& characterName);

private:
    void startTypingEffect(cocos2d::Label* label, const std::string& dialogue);// 打字效果
    void close();// 关闭
};
