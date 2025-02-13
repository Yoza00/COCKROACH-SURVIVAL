#pragma once
#include"../../../../GameObject/Stage/Stage.h"

class House :public Stage
{
public:

	House() {}
	~House()override {}

	void Init()override;

	void SetModel(const std::string& _filePath)override;

private:

};