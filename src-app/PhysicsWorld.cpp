#include "PhysicsWorld.h"

//--------------------------------------------------------------------
void PhysicsWorld::setup(float screenW, float screenH, float screenSizeInMeters, float timeStep,
                         float gravity0)
{
    //_scale = 1.0 / ( 50.0 / 1000.0 );
    _scale = min(screenW, screenH) / screenSizeInMeters;

    _invScale = 1.0 / _scale;
    _w = screenW / _scale;
    _h = screenH / _scale;

    _timeStep = timeStep;


    // Define the gravity vector.
    b2Vec2 gravity(0.0f, gravity0); //-10.0f);

    // Do we want to let bodies sleep?
    bool doSleep = true;

    // Construct a world object, which will hold and simulate the rigid bodies.
    _world = new b2World(gravity);

    _staticEdges.resize(maxStaticEdges);
    _groundBody = 0;
}

//--------------------------------------------------------------------
void PhysicsWorld::clear()
{
    delete _world;

}
//--------------------------------------------------------------------
b2Vec2 PhysicsWorld::toWorld(const ofPoint &p)
{
    return b2Vec2(p.x * _invScale, _h - p.y * _invScale);

}

//--------------------------------------------------------------------
ofPoint PhysicsWorld::toScreen(const b2Vec2 &p)
{
    return ofPoint(p.x * _scale, (_h - p.y) * _scale);
}

//--------------------------------------------------------------------
b2Vec2 PhysicsWorld::forceToWorld(const ofPoint &p)
{
    return b2Vec2(p.x * _invScale, - p.y * _invScale);
}


//--------------------------------------------------------------------
void PhysicsWorld::update()
{
    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (60Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.
    float32 timeStep = _timeStep;
    int32 velocityIterations = 10;
    int32 positionIterations = 10;

    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    _world->Step(timeStep, velocityIterations, positionIterations);

    // Clear applied body forces. We didn't apply any forces, but you
    // should know about this function.
    _world->ClearForces();


    for (int i = 0; i < _circles.size(); i++) {
        CircleData &c = _circles[ i ];
        b2Vec2 position = c._body->GetPosition();
        float32 angle = c._body->GetAngle();
        c.angleDeg = angleToScreen(angle);
        c.p = toScreen(position);
    }

}

//--------------------------------------------------------------------
int PhysicsWorld::addStaticEdge(const ofPoint &p1, const ofPoint &p2, int id)
{
    if (id == -1) {
        return id;
    }
    //if ( _staticEdges[ id ].used ) return -1;

    //ofPoint p1 = edge.p1;
    //ofPoint p2 = edge.p2;

    if (!_staticEdges[ id ].used) {	//еще не создан

        // Call the body factory which allocates memory for the ground body
        // from a pool and creates the ground box shape (also from a pool).
        // The body is also added to the world.
        if (!_groundBody) {
            // Define the ground body.
            b2BodyDef bodyDef;
            bodyDef.position.Set(0.0f, 0.0f);
            _groundBody = world()->CreateBody(&bodyDef);
        }
        b2Body *body = _groundBody;

        // Define the ground box shape.
        b2EdgeShape shape;

        // The extents are the half-widths of the box.
        //groundBox.SetAsBox( _physics.sizeToWorld(500), _physics.sizeToWorld(100));
        shape.Set(toWorld(p1), toWorld(p2));

        // Add the ground fixture to the ground body.
        b2Fixture *fixture = body->CreateFixture(&shape, 0.0f);

        EdgeData edge(p1, p2, fixture);
        _staticEdges[ id ] = edge; //.push_back( edge );
    } else {
        EdgeData &edge = _staticEdges[ id ];
        edge.p1 = p1;
        edge.p2 = p2;
        b2EdgeShape *shape = (b2EdgeShape *)edge._fixture->GetShape();
        shape->Set(toWorld(p1), toWorld(p2));
    }

    return id;

}

//--------------------------------------------------------------------
void PhysicsWorld::removeStaticEdge(int id)			//удаляет ребро
{
    addStaticEdge(ofPoint(-id * 10 - 10, 0), ofPoint(-id * 10 - 10 + 1, 0), id);
    /*if ( id >= 0 && _staticEdges[ id ].used) {
    	EdgeData &edge = _staticEdges[ id ];
    	_groundBody->DestroyFixture( edge._fixture );
    	edge.used = false;
    }*/
}

//--------------------------------------------------------------------
void PhysicsWorld::beginChanges()
{

}

void PhysicsWorld::endChanges()
{
    _groundBody->SetTransform(b2Vec2(0.0, 0.0), 0.0);
}


//--------------------------------------------------------------------
void PhysicsWorld::addCircle(const CircleData &circle0)
{
    CircleData circle = circle0;

    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = toWorld(circle.p);   //.Set(0.0f, 15.0f);
    bodyDef.angle = angleToWorld(circle.angleDeg);
    b2Body *body = world()->CreateBody(&bodyDef);
    circle._body = body;

    // Define another box shape for our dynamic body.
    b2CircleShape circleShape;
    circleShape.m_p.SetZero();
    circleShape.m_radius = sizeToWorld(circle.rad);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = circle.density;

    // Override the default friction.
    fixtureDef.friction = circle.friction;			//0.3f;
    fixtureDef.restitution = circle.restitution;	//0.2;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);

    //запись в список
    _circles.push_back(circle);

}

//--------------------------------------------------------------------
void PhysicsWorld::setCirclePosAndVelocity(int i, ofPoint pos, ofPoint vel, float angVel)
{
    CircleData &c = _circles[ i ];
    b2Body *body = c._body;
    body->SetTransform(toWorld(pos), body->GetAngle());
    body->SetLinearVelocity(forceToWorld(vel));
    body->SetAngularVelocity(angVel);

}

void PhysicsWorld::setCircleRadAndTexture(int i, float rad, int texture)
{
    CircleData &c = _circles[ i ];
    c.rad = rad;
    c.groupIndex = texture;
    b2Body *body = c._body;
    b2Fixture *fixture = body->GetFixtureList();
    b2CircleShape *shape = (b2CircleShape *) fixture->GetShape();
    shape->m_radius = sizeToWorld(rad);

}

//--------------------------------------------------------------------
void PhysicsWorld::addTriangle(const ofPoint &point0, const ofPoint &point1, const ofPoint &point2)
{

    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody; //b2_dynamicBody;
    bodyDef.position.Set(0.0f, 0.0f);
    bodyDef.angle = 0.0;
    b2Body *body = world()->CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    vector<b2Vec2> v(3);
    v[0] = toWorld(point0);
    v[1] = toWorld(point1);
    v[2] = toWorld(point2);
    b2PolygonShape shape;
    shape.Set(&v[0], 3);
    b2Fixture *fixture = body->CreateFixture(&shape, 0.0f);


    TriangleData tri(point0, point1, point2, fixture, body);
    _triangles.push_back(tri);

}

//--------------------------------------------------------------------
void PhysicsWorld::clearTriangles()
{
    for (int i = 0; i < _triangles.size(); i++) {
        world()->DestroyBody(_triangles[i]._body);
    }
    _triangles.clear();

}

//--------------------------------------------------------------------
