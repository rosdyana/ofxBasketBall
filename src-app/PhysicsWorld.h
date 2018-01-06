#pragma once

#include "ofMain.h"
#include <Box2D/Box2D.h>

struct CircleData {
	ofPoint p;
	float rad;
	float angleDeg;
	int groupIndex;
	float density;
	float friction;
	float restitution;

	b2Body *_body;
};

struct EdgeData {
	EdgeData() { used = false; }
	EdgeData( const ofPoint &p01, const ofPoint &p02, b2Fixture* fixture, bool used0 = true ) 
	{
			p1 = p01; 
			p2 = p02; 
			_fixture = fixture;
			used = used0; 
	}
	ofPoint p1, p2;
	bool used;

	b2Fixture* _fixture;

};

struct TriangleData {
	TriangleData( const ofPoint &p01, const ofPoint &p02, const ofPoint &p03, b2Fixture* fixture, b2Body *body ) 
	{
		p[0] = p01;
		p[1] = p02;
		p[2] = p03;
		_fixture = fixture;
		_body = body;
	}
	ofPoint p[3];

	b2Body *_body;
	b2Fixture* _fixture;

};

class PhysicsWorld
{
public:
	void setup( float screenW, float screenH, float screenSizeInMeters = 1.0, float timeStep = 1.0 / 60.0,
		float gravity = -10.f );
	void update();
	void clear();

	b2World *world() { return _world; }

	 b2Vec2 toWorld( const ofPoint &p );
	 ofPoint toScreen( const b2Vec2 &p );
	 b2Vec2 forceToWorld( const ofPoint &p );

	inline float sizeToWorld( float size ) { return size * _invScale; }
	inline float sizeToScreen( float size ) { return size * _scale; }
	inline float angleToWorld( float angle ) { return - ofDegToRad( angle ); }
	inline float angleToScreen( float angle ) { return - ofRadToDeg( angle ); }


	int addStaticEdge( const ofPoint &point0, const ofPoint &point1, int id );
	void removeStaticEdge( int id );
	vector<EdgeData> &staticEdges() { return _staticEdges; }
	void beginChanges();
	void endChanges();

	void addCircle( const CircleData &circle ); 
	vector<CircleData> &circles() { return _circles; }
	void setCirclePosAndVelocity( int i, ofPoint pos, ofPoint vel, float angVel );
	void setCircleRadAndTexture( int i, float rad, int texture );

	void addTriangle( const ofPoint &point0, const ofPoint &point1, const ofPoint &point2 );
	void clearTriangles();
	vector<TriangleData> &triangles() { return _triangles; }


private:
	float _scale, _invScale;
	float _w, _h; 
	float _timeStep;

	b2World *_world;

	static const int maxStaticEdges = 1000;
	vector<EdgeData> _staticEdges;
	b2Body *_groundBody;

	vector<CircleData> _circles;
	vector<TriangleData> _triangles;


};

