#pragma once
#include "audio/include/AudioEngine.h" // 包含音频引擎的头文件
using namespace cocos2d::experimental; // 使用音频引擎的命名空间
#include "cocos2d.h"

class AudioManager{
public:
    static AudioManager* getInstance();

    void playBGM(const std::string& filename);
    void pauseBGM();
    void resumeBGM();
    void setVolume(float volume); // 设置音量
    void increaseVolume(float amount); // 增加音量
    void decreaseVolume(float amount); // 减小音量

private:
    AudioManager();
    static AudioManager* _instance;
    float _volume; // 当前音量
};