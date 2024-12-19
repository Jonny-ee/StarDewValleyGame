#include "Drought.h"
#include"cocos2d.h"

Drought* Drought::create() {
    Drought* ret = new (std::nothrow) Drought();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Drought::init() {
    //暂无美术素材
    return true;
}

void Drought::update(float dt) {
    // 暂无美术素材
}