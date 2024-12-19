#include "Rain.h"
#include"cocos2d.h"

Rain* Rain::create() {
    Rain* ret = new (std::nothrow) Rain();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Rain::init() {
   //暂无美术素材
    return true;
}

void Rain::update(float dt) {
    // 暂无美术素材
}