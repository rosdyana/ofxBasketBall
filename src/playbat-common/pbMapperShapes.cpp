#include "pbMapperShapes.h"
#include "pbFiles.h"

//---------------------------------------------------------------
void pbMapperShape::setup( float w, float h, string fileStruc, string fileProj )
{
	_w = w;
	_h = h;
	_fileStruc = fileStruc;
	_fileProj = fileProj;
}

//---------------------------------------------------------------
void pbMapperShape::load()
{
	loadStruc();
	loadProj();
}

//---------------------------------------------------------------
void pbMapperShape::save()
{
	saveStruc();
	saveProj();
}

//---------------------------------------------------------------
void pbMapperShape::loadStruc( string fileName )	
{
	if ( fileName == "" ) {
		fileName = _fileStruc;
	}

	vector<float> list = pbFiles::readFloats( ofToDataPath( fileName ) );
	if ( list.size() == 0 ) return;

	_v.clear();
	_q.clear();
	_proj.clear();

	int j = 0;

	//�������
	{	
		int n = list[ j++ ];
		_v.resize( n );
		for (int i=0; i<n; i++) {
			float x, y, parent;
			x = list[ j++ ];
			y = list[ j++ ];
			parent = list[ j++ ];
			_v[i] = pbMapperStrucVertex( ofPoint( x, y ), parent );
		}
	}
	

	//�����
	{	
		int m = list[ j++ ];
		_q.resize( m );
		for (int k=0; k<m; k++) {
			pbMapperStrucQuad &q = _q[k];
			int n = list[ j++ ];
			q.iv.resize( n );
			for (int i=0; i<n; i++) {
				q.iv[i] = list[ j++ ];
			}
		}
	}
	recalc();
	tuningReset();
}

//---------------------------------------------------------------
void pbMapperShape::saveStruc( string fileName )
{
	if ( fileName == "" ) {
		fileName = _fileStruc;
	}
	vector<float> list;
	//�������
	list.push_back( _v.size() );
	for (int i=0; i<_v.size(); i++) {
		pbMapperStrucVertex &v = _v[i];
		list.push_back( v.p.x );
		list.push_back( v.p.y );
		list.push_back( v.parent );
	}
	//�����
	list.push_back( _q.size() );
	for (int k=0; k<_q.size(); k++) {
		pbMapperStrucQuad &q = _q[k];
		list.push_back( q.iv.size() );
		for (int i=0; i<q.iv.size(); i++) {
			list.push_back( q.iv[i] );
		}
	}

	pbFiles::writeFloats( list, ofToDataPath( fileName ) );

}

//---------------------------------------------------------------
void pbMapperShape::loadProj( string fileName )	
{
	if ( fileName == "" ) {
		fileName = _fileProj;
	}


	tuningReset();

}

//---------------------------------------------------------------
void pbMapperShape::saveProj( string fileName )
{
	if ( fileName == "" ) {
		fileName = _fileProj;
	}

}

//---------------------------------------------------------------
void pbMapperShape::update()		//��������� ����������, ���� ����������
{
	recalcActual();
}

//---------------------------------------------------------------
//���������
void pbMapperShape::draw( ofTexture &tex, int mode )	//������� ��������
{
	recalcActual();
	float texW = tex.texData.tex_w;
	float texH = tex.texData.tex_h;

	tex.bind();
	drawBinded( texW, texH, mode );
	tex.unbind();
}

//---------------------------------------------------------------
void pbMapperShape::drawBinded( float texW, float texH, int mode )	//������� ��������, ��� ������� ��� ������ bind
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	if ( mode == ModeScr ) {
		if ( surfGL.size() > 0 ) {
			glVertexPointer( 2, GL_FLOAT, 0, &surfGL[0] );
			glTexCoordPointer( 2, GL_FLOAT, 0, &texGL[0] );
			glDrawArrays( GL_QUADS, 0, surfGL.size() / 2 );
		}
	}
	else {
		vector<float> surf(8);	//������� ������ ����
		vector<float> tex(8);
		surf[0] = 0;		surf[1] = 0;
		surf[2] = _w;		surf[3] = 0;
		surf[4] = _w;		surf[5] = _h;
		surf[6] = 0;		surf[7] = _h;
		tex = surf;
		for (int i=0; i<tex.size(); i+=2) {
			tex[i+1] = _h - tex[i+1];
		}
		glVertexPointer( 2, GL_FLOAT, 0, &surf[0] );
		glTexCoordPointer( 2, GL_FLOAT, 0, &tex[0] );
		glDrawArrays( GL_QUADS, 0, surf.size() / 2 );
	}

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}

//---------------------------------------------------------------
void drawQuad( const vector<ofPoint> &p ) 
{
	int n = p.size();
	for (int i=0; i<n; i++) {
		ofLine( p[i].x, p[i].y, p[ (i+1)%n ].x, p[ (i+1)%n ].y );
	}
}

//---------------------------------------------------------------
void pbMapperShape::drawWireframe( int mode, int selQ, int selV, int selQ1, int selV1,
								  int colorVariant )
{
	recalcActual();

	float kRad = 3;

	//����� - �����
	ofSetColor( 128, 128, 128 );
	for (int k=0; k<_q.size(); k++) {
		drawQuad( quadVerticesGlobal( mode, k ) );
	}

	//���������� ����
	if ( colorVariant == 0 ) {
		ofSetColor( 0, 0, 255 );
		drawQuad( quadVerticesGlobal( mode, selQ ) );
		//���������� ����1
		if ( selQ1 >= 0 ) {
			ofSetColor( 255, 255, 0 );
			drawQuad( quadVerticesGlobal( mode, selQ1 ) );
		}
	}
	if ( colorVariant == 1 ) {	//����� �������������� �����
		ofSetColor( 255, 0, 255 );
		drawQuad( quadVerticesGlobal( mode, selQ1 ) );
	}
	if ( colorVariant == 2 ) {	//����� �������������� �������
		ofSetColor( 255, 128, 128);
		drawQuad( quadVerticesGlobal( mode, selQ1 ) );
	}


	//�������
	ofFill();
	if ( colorVariant != 2 ) {
		if ( mode == ModeTex )	ofSetColor( 0, 0, 255 );
		if ( mode == ModeScr )	ofSetColor( 0, 0, 255 );
	}
	else ofSetColor( 128, 128, 128 );
	ofFill();
	for (int i=0; i<_v.size(); i++) {
		ofPoint p = getPointGlobal( mode, i );
		ofCircle( p.x, p.y, kRad );
	}
	//���������� �������
	if ( colorVariant == 2 && selV >= 0 ) {
		ofSetColor( 255, 255, 0 );
		ofPoint p = getPointGlobal( mode, selV );
		ofCircle( p.x, p.y, kRad );
	}

}


//---------------------------------------------------------------
//������
void pbMapperShape::tuningReset()
{
	int n = _v.size();
	_proj.resize( n );
	for (int i=0; i<n; i++) {
		_proj[i] = _v[i].p;
	}
	recalc();
}

//---------------------------------------------------------------
//��������������
void pbMapperShape::editClearQuad( int q )
{

	recalc();
}

//---------------------------------------------------------------
void pbMapperShape::editFillQuadUniform( int q, int w, int h )
{
	w++;
	h++;
	if ( w < 2 || h < 2 ) return;

	//�������� ������
	int n0 = _v.size();
	_v.resize( n0 + w * h );
	_proj.resize( _v.size() );

	for ( int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			int i = n0 + x + w * y;
			float X = 1.0 * x / (w-1);
			float Y = 1.0 * y / (h-1);
			_v[i] = pbMapperStrucVertex( ofPoint( X, Y ), q );
			_proj[i] = ofPoint( X, Y );
		}
	}

	//�������� ������
	int m0 = _q.size();
	_q.resize( m0 + (w-1) * (h-1) );
	for (int y=0; y<h-1; y++) {
		for (int x=0; x<w-1; x++) {
			int ind = m0 + x + y * (w-1);
			vector<int> iv(4);
			iv[ 0 ] = n0 + x + w * y;
			iv[ 1 ] = n0 + (x+1) + w * y;
			iv[ 2 ] = n0 + (x+1) + w * (y+1);
			iv[ 3 ] = n0 + x + w * (y+1);
			_q[ ind ].iv = iv;
		}
	}


	recalc();
}

//---------------------------------------------------------------
void pbMapperShape::editAddPoint( int q, ofPoint p )		//������������ globalToLocalCoord()
{

	recalc();
}

//---------------------------------------------------------------
void pbMapperShape::editMovePoint( int mode, int i, ofPoint shift )	//����� �������� �����, �� ����� - �������� � ����� ����������� �� ����� shift
{
	pbMapperStrucVertex &v = _v[i];
	int parent = v.parent;
	ofPoint &V = ( mode == ModeTex ) ? v.p : _proj[ i ];
	ofPoint p0 = getPointGlobal( mode, i );
	ofPoint p1 = localToGlobalCoord( mode, parent, V + shift );
	float len1 = ofxVec2f( shift ).length();	//�������� �����
	float len2 = ofxVec2f( p1 - p0 ).length();	//������������ �����
	if ( len2 > 0.0001 ) {
		shift *= len1 / len2;
		V += shift;
		recalc();
	}
}

//---------------------------------------------------------------
void pbMapperShape::editMoveQuad( int mode, int q, ofPoint shift )
{
	if ( q >= 0 ) {
		vector<int> iv = _q[ q ].iv;
		for (int i=0; i<iv.size(); i++) {
			editMovePoint( mode, iv[i], shift );
		}
	}
}

//---------------------------------------------------------------
void pbMapperShape::editResizeQuad( int mode, int q, ofPoint shift )
{
	if ( q >= 0 ) {
		vector<int> iv = _q[ q ].iv;
		//������� ����������� �����
		ofPoint c;
		for (int i=0; i<iv.size(); i++) {
			c += _v[ iv[i] ].p;
		}
		c /= iv.size();
		for (int i=0; i<iv.size(); i++) {
			ofPoint delta = _v[ iv[i] ].p - c;
			ofPoint shift1 = shift;
			if ( delta.x < 0 ) shift1.x = -shift1.x;
			if ( delta.y < 0 ) shift1.y = -shift1.y;
			editMovePoint( mode, iv[i], shift1 );
		}
	}
}

//---------------------------------------------------------------
//��������� ���������� �����
ofPoint pbMapperShape::getPointLocal( int mode, int i )
{
	return ( mode == ModeTex ) ? _v[ i ].p : _proj[ i ];
}

//---------------------------------------------------------------
//���������� ���������� �����
ofPoint pbMapperShape::getPointGlobal( int mode, int i )
{
	return ( mode == ModeTex ) ? _tex[ i ] : _scr[ i ];
}

//---------------------------------------------------------------
//���������� �����-���������� �����
vector<ofPoint> pbMapperShape::quadVerticesGlobal( int mode, int q0 )
{
	//0  1
	//3  2
	int n = 4;
	vector<ofPoint> res( n );
	if ( q0 == -1 ) {				//��������� ����, ������ �� ���� �����
		res[0] = ofPoint( 0, 0 );
		res[1] = ofPoint( _w, 0 );
		res[2] = ofPoint( _w, _h );
		res[3] = ofPoint( 0, _h );
	}
	else {
		pbMapperStrucQuad &q = _q[ q0 ];
		for (int i=0; i<n; i++) {
			res[i] = getPointGlobal( mode, q.iv[i] );
		}
	}
	return res;
}

//---------------------------------------------------------------
//������� ������ ��������� � �����
ofPoint pbMapperShape::localToGlobalCoord( int mode, int q, const ofPoint &local )
{
	//0  1
	//3  2
	vector<ofPoint> p = quadVerticesGlobal( mode, q );
	ofPoint res;
	res = p[0] * (1-local.x) * (1-local.y) 
		+ p[1] * (local.x) * (1-local.y)
		+ p[2] * (local.x) * (local.y)
		+ p[3] * (1-local.x) * (local.y);
	return res;
}

//---------------------------------------------------------------
void pbMapperShape::recalc()					//����������� ��� ��������� ������
{
	_needRecalc = true;
}

//---------------------------------------------------------------
void pbMapperShape::recalcActual()			//����������� ������ recalc
{
	if ( !_needRecalc ) return;
	_needRecalc = false;
	int n = _v.size();
	_tex.resize( n );
	_scr.resize( n );

	int m = _q.size();
	_qRender.resize( m );
	fill( _qRender.begin(), _qRender.end(), true );

	for (int i=0; i<_v.size(); i++) {
		pbMapperStrucVertex &v = _v[i];
		int parent = v.parent;
		_tex[i] = localToGlobalCoord( ModeTex, parent, v.p );
		_scr[i] = localToGlobalCoord( ModeScr, parent, _proj[i] );

		//����� �������� �������� - ��� �� ���� ���������
		if ( parent >= 0 ) {
			_qRender[ parent ] = false;
		}
	}

	//�������� ������ ������ ��� ����������
	vector<int> list;
	for (int i=0; i<_q.size(); i++ ) {
		if ( _qRender[i] ) {
			list.push_back( i );
		}
	}
	_qRenderList = list;

	//OpenGL
	recalcOpenGL();
}

//---------------------------------------------------------------
void pbMapperShape::recalcOpenGL()			//����������� ������� OpenGL
{
	vector<int> &list = _qRenderList;
	int n = list.size();

	surfGL.resize(  4 * 2 * n );
	texGL.resize( 4 * 2 * n );

	int index = 0;
	for (int k=0; k<n; k++) {
		vector<int> &iv = _q[ list[k] ].iv;
		for (int i=0; i<iv.size(); i++) {
			ofPoint tex = getPointGlobal( ModeTex, iv[i] );
			ofPoint scr = getPointGlobal( ModeScr, iv[i] );
			texGL[ index ] = tex.x;
			texGL[ index + 1 ] = _h - tex.y;

			surfGL[ index ] = scr.x;
			surfGL[ index + 1 ] = scr.y;
			index += 2;
		}
	}
}

//---------------------------------------------------------------
int pbMapperShape::findNearestPointIndex( int q, ofPoint p )	//���� ��������� ����� � ������ ����� 4-���������
{
	return 0;
}

//---------------------------------------------------------------
int pbMapperShape::nextQuad( int q, int q1, int dir )	//����� ���������� � q �����, ����� q1
{
	int n = _q.size();
	//�������� ����� ������-�������� q
	vector<bool> use( n, false );
	for (int k=0; k<n; k++) {
		vector<int> &iv = _q[k].iv;
		for (int i=0; i<iv.size(); i++) {
			if ( _v[iv[i]].parent == q ) {
				use[ k ] = true;
			}
		}
	}
	//�������� ������
	vector<int> list;
	int index = -1;
	for (int k=0; k<n; k++) {
		if ( use[k] ) {
			list.push_back( k );
			if ( k == q1 ) index = list.size() - 1;
		}
	}
	int m = list.size();
	if ( m > 0 ) {
		if ( index >= 0 ) {
			index = ( index + m + dir ) % m;
			return list[ index ];
		}
		else {
			return list[0];
		}	
	}
	else return -1;

}

//---------------------------------------------------------------
int pbMapperShape::quadParent( int q )			//����� �������� ����� q
{
	if ( q >= 0 ) {
		vector<int> &iv = _q[q].iv;
		return _v[ iv[0] ].parent;
	}
	else return -1;
}

int pbMapperShape::quadVertexIndex( int q, int num )
{
	if ( q >= 0 ) {
		return _q[q].iv[ num ];
	}
	else return -1;
}


//---------------------------------------------------------------
//pbMapperStrucQuad pbMapperShape::quadVertices(int q)			//������ ������ �����
//{
//	pbMapperStrucQuad quad;
//	return quad;
//}

//---------------------------------------------------------------
