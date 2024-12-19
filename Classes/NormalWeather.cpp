#include "NormalWeather.h"
#include"cocos2d.h"

NormalWeather* NormalWeather::create() {
    NormalWeather* ret = new (std::nothrow) NormalWeather();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool NormalWeather::init() {
    //暂无美术素材
    return true;
}

void NormalWeather::update(float dt) {
    // 暂无美术素材
}