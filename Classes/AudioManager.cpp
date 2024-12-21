#include "AudioManager.h"

AudioManager* AudioManager::_instance = nullptr;

AudioManager::AudioManager() : _volume(1.0f) { // 默认音量为1.0（最大）
}

AudioManager* AudioManager::getInstance() {
    if (_instance == nullptr) {
        _instance = new AudioManager();
    }
    return _instance;
}

void AudioManager::playBGM(const std::string& filename) {
    AudioEngine::play2d(filename, true, _volume);
}

void AudioManager::pauseBGM() {
    AudioEngine::pauseAll();
}

void AudioManager::resumeBGM() {
    AudioEngine::resumeAll();
}

void AudioManager::setVolume(float volume) {
    _volume = volume;
    AudioEngine::setVolume(1, _volume); // 假设你只播放一个音轨
}

void AudioManager::increaseVolume(float amount) {
    _volume += amount;
    if (_volume > 1.0f) _volume = 1.0f; // 限制最大音量
    setVolume(_volume);
}

void AudioManager::decreaseVolume(float amount) {
    _volume -= amount;
    if (_volume < 0.0f) _volume = 0.0f; // 限制最小音量
    setVolume(_volume);
}