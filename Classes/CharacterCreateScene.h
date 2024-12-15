#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"

class CharacterCreateScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(CharacterCreateScene);

private:
    cocos2d::ui::EditBox* _nameInput;
    cocos2d::ui::Button* _confirmButton;

    void initUI();
    void onConfirmClicked();
    std::string _playerName;
};
