#include "pbMappingMesh.h"
#include "pbFiles.h"
#include "parsing.h"


//---------------------------------------------------------
/*
vertices: 3
1: 0, 0
2: 1, 2
3: 3, 4
polygons: 1
1: 1, 2, 3
*/

#define pbMeshLoadError( fileName, k ) { cout << "ERROR: " << fileName << " line " << k << endl; clear(); return; }


//---------------------------------------------------------
void pbMesh::load( string fileName )
{
	if ( fileName == "" ) {
		fileName = _fileName;
	}
	else {
		_fileName = fileName;
	}
	vector<string> list = pbFiles::readStrings( ofToDataPath( fileName ) );
	if ( list.size() >= 2 ) {
		int k = 0;
		//вершины
		int nV = 0;
		{
			vector<string> parse = Parsing::splitFiltered( list[k++], ":" );
			if ( parse.size() >= 2 ) {
				nV = ofToInt( parse[1] );
			}
			else {
				pbMeshLoadError( fileName, k );
			}
		}
		v.resize( nV );
		for (int i=0; i<nV; i++) {
			vector<string> parse = Parsing::splitFiltered( list[k++], ":" );
			if ( parse.size() != 2 || parse[0] != ofToString( i+1 ) ) {
				pbMeshLoadError( fileName, k );

			}
			vector<string> parse1 =  Parsing::splitFiltered( parse[1], "," );
			if ( parse1.size() >= 2 ) {
				v[i].x = ofToFloat( parse1[0] );
				v[i].y = ofToFloat( parse1[1] );
			}
			else {
				pbMeshLoadError( fileName, k );
			}
		}

		//многоугольники

		int nP = 0;
		{
			vector<string> parse = Parsing::splitFiltered( list[k++], ":" );
			if ( parse.size() >= 2 ) {
				nP = ofToInt( parse[1] );
			}
			else {
				pbMeshLoadError( fileName, k );
			}
		}
		poly.resize( nP );
		for (int i=0; i<nP; i++) {
			vector<string> parse = Parsing::splitFiltered( list[k++], ":" );
			if ( parse.size() != 2 || parse[0] != ofToString( i+1 ) ) {
				pbMeshLoadError( fileName, k );
			}
			vector<string> parse1 =  Parsing::splitFiltered( parse[1], "," );
			int n = parse1.size();
			if ( n >= 3 ) {
				pbMeshPolygon &p = poly[i];
				p.resize( n );
				for ( int j=0; j<n; j++ ) {
					int index = ofToFloat( parse1[j] ) - 1;
					if ( !ofInRange( index, 0, nV - 1 ) ) {
						pbMeshLoadError( fileName, k );
					}
					p[j] = index;					
				}	
			}
			else {
				pbMeshLoadError( fileName, k );
			}
		}
	}
	else {
		pbMeshLoadError( fileName, -1 );
	}
}

//---------------------------------------------------------
void pbMesh::save( string fileName )
{
	if ( fileName == "" ) {
		fileName = _fileName;
	}
	else {
		_fileName = fileName;
	}

	vector<string> list;
	//вершины
	list.push_back( "vertices:" + ofToString( int( v.size() ) ) );
	for (int i=0; i<v.size(); i++) {
		string s =  ofToString( i+1 ) + ": " + ofToString( v[i].x ) + ", " + ofToString( v[i].y );
		list.push_back( s );
	}
	//многоугольники
	list.push_back( "polygons:" + ofToString( int(poly.size()) ) );
	for (int i=0; i<poly.size(); i++) {
		pbMeshPolygon &p = poly[i];
		string s = ofToString( i+1 ) + ": ";
		for (int j=0; j<p.size(); j++ ) {
			if ( j > 0 ) { s += ", "; } 
			s += ofToString( p[j] + 1 );	
		}
		list.push_back( s );
	}
	pbFiles::writeStrings( list, ofToDataPath( fileName ) );
}


//---------------------------------------------------------
void pbMesh::createCircle( ofPoint center, float rad, int segments )
{
	clear();
	if ( segments > 0 ) {
		v.push_back( center );
		for ( int i=0; i<segments; i++ ) {
			float a = M_TWO_PI * i / segments;
			ofPoint p;
			p.x = center.x + rad * cos( a );
			p.y = center.y +  rad * sin( a );
			v.push_back( p );
		}
		for (int i=0; i<segments; i++) {
			int i1 = (i+1) % segments;
			pbMeshPolygon p;
			p.push_back( 0 );
			p.push_back( i1 + 1 );
			p.push_back( i + 1 );
			poly.push_back( p );
		}
	}
}

//---------------------------------------------------------
void pbMesh::move( ofPoint shift )
{
	int n = v.size();
	for ( int i=0; i<n; i++ ) {
		v[i] += shift;
	}
}

//---------------------------------------------------------
void pbMesh::resize( ofPoint shift, ofPoint center )
{
	int n = v.size();
	for ( int i=0; i<n; i++ ) {
		ofPoint &p = v[i];
		ofPoint delta = p - center;
		delta.x *= shift.x;
		delta.y *= shift.y;
		v[i] = center + delta;
	}

}

//---------------------------------------------------------
void pbMesh::moveVertex( ofPoint shift, int i )
{
	if ( ofInRange( i, 0, v.size() - 1 ) ) {
		v[i] += shift;
	}
}

//---------------------------------------------------------
//рисует wireframe, без установки собственного цвета
void pbMesh::draw( int selectedVertex, int selectedColor )		
{
	for (int k=0; k<poly.size(); k++) {
		pbMeshPolygon &p = poly[k];
		int n = p.size();
		for (int i=0; i<n; i++) {
			ofPoint a = v[ p[i] ];
			ofPoint b = v[ p[ (i+1) % n ] ];
			ofLine( a.x, a.y, b.x, b.y );
		}	
	}
	if ( ofInRange( selectedVertex, 0, v.size() - 1 ) ) {
		ofFill();
		ofSetColor( selectedColor );
		ofPoint &p = v[ selectedVertex ];
		ofCircle( p.x, p.y, 5 );
	}

}


//---------------------------------------------------------
//Мэппинг
vector<float> texGL;            //OpenGL - текстурные координаты
vector<float> surfGL;           //OpenGL - результирующие координаты

void pbMeshMapping( pbMesh &meshA, pbMesh &meshB, ofTexture &tex, bool wireframe, int onlyOneTriangle )
{
	if ( onlyOneTriangle > int( meshA.poly.size() ) ) {
		cout << "ERROR: pbMeshMapping want draw triangle " << onlyOneTriangle << endl;
		return;
	}

	if ( wireframe ) {
		//Wireframe
		ofSetColor( 255, 255, 255 );
		meshB.draw();
	}
	else {
		//Мэппинг

		//Расчет массивов OpenGL
		int n = meshA.poly.size();
		int N = n;
		if ( onlyOneTriangle >= 0 ) { N = 1; }
		surfGL.resize(  3 * 2 * N );
		texGL.resize( 3 * 2 * N );

		int h = tex.getHeight();
		int index = 0;
		for (int k=0; k<n; k++) {
			if ( onlyOneTriangle == -1 || onlyOneTriangle == k ) {
				vector<int> &tri = meshA.poly[k];
				for (int i=0; i<tri.size(); i++) {
					ofPoint tex = meshA.v[ tri[i] ];
					ofPoint scr = meshB.v[ tri[i] ];
					texGL[ index ] = tex.x;
					texGL[ index + 1 ] = h - tex.y;

					surfGL[ index ] = scr.x;
					surfGL[ index + 1 ] = scr.y;
					index += 2;
				}
			}
		}

		//Рисование

		tex.bind();    
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		if ( surfGL.size() > 0 ) {
			glVertexPointer( 2, GL_FLOAT, 0, &surfGL[0] );
			glTexCoordPointer( 2, GL_FLOAT, 0, &texGL[0] );
			glDrawArrays( GL_TRIANGLES, 0, surfGL.size() / 2 );
		}

		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_VERTEX_ARRAY );

		tex.unbind();
	}
	
	
}

//---------------------------------------------------------
