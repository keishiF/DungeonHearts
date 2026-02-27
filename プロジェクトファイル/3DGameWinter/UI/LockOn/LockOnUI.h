#pragma once
#include "UI/UIBase.h"

class Player;
class LockOnUI : public UIBase
{
public:
	LockOnUI(std::weak_ptr<Player> player);
	~LockOnUI();
	
	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() override;
	
private:
	std::weak_ptr<Player> m_player;
	// ロックオンの画像ハンドル
	int m_image;
	int m_refCount;
	// スクリーン座標
	Vector3 m_screenPos;
	// 描画するかどうか
	bool m_isDrawing;
	// 画像の回転角度
	float m_angle;
};

