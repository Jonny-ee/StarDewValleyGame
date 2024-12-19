#pragma once
#ifndef DROUGHT_H
#define DROUGHT_H
#include "cocos2d.h"
#include "Weather.h"

class Drought : public Weather {
public:
    static Drought* create();

    virtual void update(float dt) override;
    virtual std::string getWeatherName() const override { return "Drought"; }

private:
    Drought() {};
    bool init();
};
#endif//DROUGHT_H