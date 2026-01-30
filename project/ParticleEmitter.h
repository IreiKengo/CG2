#pragma once
#include <string>
#include "Vector3.h"

class ParticleEmitter
{
public:
    ParticleEmitter(
        const std::string& name,
        const Vector3& position,
        uint32_t count,
        float frequency
    );

    void Update(float deltaTime);


private:
    std::string name_;
    Vector3 position_;
    uint32_t count_;

    float frequency_;//発生頻度
    float frequencyTime_;//頻度用時刻

    void Emit();

  };
