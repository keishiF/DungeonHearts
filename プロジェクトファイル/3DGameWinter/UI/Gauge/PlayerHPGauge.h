#pragma once
#include "UI/UIBase.h"

class Player;
class PlayerHPGauge : public UIBase
{
public:
    PlayerHPGauge(const std::shared_ptr<Player> player);
    ~PlayerHPGauge();

    void Init() override;
    void Update() override {}
    void Draw() override;

private:
    const std::shared_ptr<Player> m_player; 

    int m_backImg;
    int m_barImg;
    int m_refCount;
    int m_width;
    int m_height;
};

