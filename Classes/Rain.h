#pragma once
#ifndef RAIN_H
#define RAIN_H
#include "cocos2d.h"
#include "Weather.h"

class Rain : public Weather {
public:
    static Rain* create();  

    virtual void update(float dt) override;
    virtual std::string getWeatherName() const override { return "Rain"; }

private:
    Rain() {};
    bool init();
    cocos2d::ParticleSystemQuad* rainParticleSystem;
};
#endif//RAIN_H

