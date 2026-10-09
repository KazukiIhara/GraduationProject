#pragma once
#include "Scene/SampleScene.h"
#include "Animals/Rabbit.h"

class AnimalTestScene final : public SampleScene {
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="init"></param>
	explicit AnimalTestScene(const InitData& init);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const override;

private:
	std::unique_ptr<Rabbit> rabbit_;

};

