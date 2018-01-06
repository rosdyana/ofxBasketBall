#include "pbTextureUtil.h"


//--------------------------------------------------------------------------
//Вспомогательная функция для рендеринга квадов
//Точки должны идти по 4x2 на квад.
//Не ставит цвет!
void pbTextureUtil::drawQuads( vector<float> &pointP, vector<float> &textureP, ofTexture &tex )
{
	int k = pointP.size();
	int nQuads = k / 8;
	if ( nQuads * 8 != k || k <= 0 || textureP.size() != k ) {
		cout << "ERROR in pbTextureUtil::drawQuads, have " <<  pointP.size() << " " << textureP.size() << endl;
	}
	else {
		tex.bind();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &pointP[0]);
		glTexCoordPointer( 2, GL_FLOAT, 0, &textureP[0] );
		glDrawArrays(GL_QUADS, 0, nQuads * 4);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		tex.unbind();
	}
}

 //--------------------------------------------------------------------------
