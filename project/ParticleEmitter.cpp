#include "ParticleEmitter.h"
#include "ParticleManager.h"

ParticleEmitter::ParticleEmitter(
    const std::string& name,
    const Vector3& position,
    uint32_t emitCount,
    float frequency)
    : name_(name)
    , position_(position)
    , count_(emitCount)
    , frequency_(frequency)
    , frequencyTime_(0.0f)
   
   
{
}

void ParticleEmitter::Update(float deltaTime)
{

    //時刻を進める
    frequencyTime_ += deltaTime;
    
    //発生頻度より大きいなら発生
    while (frequency_ <= frequencyTime_)
    {
        Emit();
        frequencyTime_ -= frequency_;//余計に過ぎた時間も加味して頻度計算する
    }
    
}


    //呼び出すだけの関数
void ParticleEmitter::Emit()
{
    ParticleManager::GetInstance()->Emit(name_, position_, count_);


}
