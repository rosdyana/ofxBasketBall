#pragma once

#include "ofMain.h"

class pbTextureUtil
{
public:
	//Вспомогательная функция для рендеринга квадов
	//Точки должны идти по 4x2 на квад.
	//Не ставит цвет!
	static void drawQuads( vector<float> &pointP, vector<float> &textureP, ofTexture &tex );

};
