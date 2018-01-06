#include "pbMapperEditor.h"

//----------------------------------------------------------
pbMapperEditor::pbMapperEditor(void)
{
}

//----------------------------------------------------------
pbMapperEditor::~pbMapperEditor(void)
{
}

//----------------------------------------------------------
void pbMapperEditor::setup( float w, float h, pbMapperShape *shape )
{
	_shape = shape;
	_w = w;
	_h = h;
	_enabled = false;
	_mode = ModeTuning;
	_editMode = EditQuadSelect;
	_quad = -1;
	_v = -1;
	_quad1 = -1;
	_v1 = -1;

	_gridX = 2;
	_gridY = 2;
	//Font
	_font.loadFont("fonts/frabk.ttf", 10);
}

//----------------------------------------------------------
void pbMapperEditor::enable( bool yes )
{
	_enabled = yes;
}

//----------------------------------------------------------
void pbMapperEditor::setMode( int mode )
{
	_mode = mode;
}

//----------------------------------------------------------
void pbMapperEditor::update( float dt )
{
	if ( enabled() ) {

	}
}

//----------------------------------------------------------
void pbMapperEditor::draw( ofTexture &tex )
{
	if ( enabled() ) {
		_shape->draw(tex, _mode);
		//делаем темнее
		if ( _mode == ModeEdit ) ofSetColor( 128, 0, 0, 48 );
		else ofSetColor( 0, 0, 0, 32 );
		ofFill();
		ofRect(0,0,_w,_h);
		//рисуем рамку
		int colorVariant = 0;
		if ( _editMode == EditQuadMove || _editMode == EditQuadScale ) colorVariant = 1;
		if ( _editMode == EditVertexEdit ) colorVariant = 2;
		_shape->drawWireframe( _mode, _quad, _v, _quad1, _v1, colorVariant );
		//cout << "quad1 " << _quad1 << endl;

		if ( _editMode == EditQuadGrid ) {
			ofSetColor( 255, 255, 255 );
			string s = "Grid: " + ofToString(_gridX) + " x " + ofToString(_gridY);
			_font.drawString( s, 10, 20 );
		}
	}
}

//----------------------------------------------------------
void pbMapperEditor::mousePressed( ofPoint p, int button )
{
	if ( enabled() ) {

	}
}

//----------------------------------------------------------
bool pbMapperEditor::keyPressed( int key )
{
	if ( enabled() ) {
		ofPoint shift;
		if ( key == OF_KEY_LEFT ) {
			shift.x = -1;
		}
		if ( key == OF_KEY_RIGHT ) {
			shift.x = +1;
		}
		if ( key == OF_KEY_UP ) {
			shift.y = -1;
		}
		if ( key == OF_KEY_DOWN ) {
			shift.y = +1;
		}

		//Вершины
		int vertex = -1;
		if ( key == '1' ) vertex = 0;
		if ( key == '2' ) vertex = 1;
		if ( key == '3' ) vertex = 2;
		if ( key == '4' ) vertex = 3;
		if ( vertex >= 0 && _quad1 >= 0 ) {
			int v = _shape->quadVertexIndex( _quad1, vertex );
			if ( v >= 0 ) {
				if ( _v == v ) {
					//Выключение режима
					setEditMode( EditQuadSelect );
					return true;		
				}
				else {
					setEditMode( EditVertexEdit );
					_v = v;
					return true;
				}
			}
		}
		if ( _editMode == EditVertexEdit ) {
			if ( key == ' ' || key == OF_KEY_BACKSPACE || key == OF_KEY_RETURN ) {
				//Выключение режима
				setEditMode( EditQuadSelect );
				return true;
			}
			//Сдвиг вершины
			float kMove = 1.0;
			if ( shift.x != 0 || shift.y != 0 ) {
				_shape->editMovePoint( _mode, _v, shift * kMove );
			}
		}


		//Сдвиг квада
		if ( key == ' ' && _quad1 >= 0 ) {
			setEditMode( ( _editMode != EditQuadMove ) ? EditQuadMove : EditQuadSelect );
			 return true;
		}

		//Скейл квада
		if ( key == 's' && _quad1 >= 0 ) {
			setEditMode( ( _editMode != EditQuadScale ) ? EditQuadScale : EditQuadSelect );
			 return true;
		}

		//Вставка сетки
		if ( key == 'g' ) {			
			if ( _editMode != EditQuadGrid ) {
				setEditMode( EditQuadGrid );
				return true;
			}
		}
		if ( _editMode == EditQuadGrid ) {
			if ( key == ' ' || key == OF_KEY_RETURN || key == OF_KEY_BACKSPACE ) {
				setEditMode( EditQuadSelect );
			}
			//Процесс вставки
			if ( key == 'g' ) {
				if ( _quad1 >= -1 && _mode == ModeEdit ) {
					_shape->editFillQuadUniform( _quad1, _gridX, _gridY );
					setEditMode( EditQuadSelect );
				}
			}
			if ( shift.x != 0 || shift.y != 0 ) {
				_gridX += shift.x;
				_gridY += -shift.y;
				return true;
			}
		}


		//Ходим по квадам
		if ( _editMode == EditQuadSelect ) {
			if ( shift.x != 0 ) {
				_quad1 = _shape->nextQuad( _quad, _quad1, shift.x );
			}
			if ( key == OF_KEY_RETURN ) {			//зайти в квад	
				if ( _quad1 >= -1 ) {
					_quad = _quad1;
					_quad1 = _shape->nextQuad( _quad, -1, shift.x );
					_v = -1;
					_v1 = -1;
				}
			}
			if ( key == OF_KEY_BACKSPACE ) {		//выйти из квада	
				if ( _quad1 >= -1 ) {
					_quad1 = _quad;
					_quad = _shape->quadParent( _quad );
					_v = -1;
					_v1 = -1;
				}
			}
		}
		//Изменение размера квада
		if ( _editMode == EditQuadMove ) {
			float kMove = 2.0;
			if ( key == ' ' || key == OF_KEY_BACKSPACE || key == OF_KEY_RETURN ) {
				setEditMode( EditQuadSelect );
			}
			if ( shift.x != 0 || shift.y != 0 ) {
				_shape->editMoveQuad( _mode, _quad1, shift * kMove );
			}
		}
		//Скейл квада
		if ( _editMode == EditQuadScale ) {
			float kMove = 2.0;
			if ( key == 's' || key == OF_KEY_BACKSPACE || key == OF_KEY_RETURN ) {
				setEditMode( EditQuadSelect );
			}
			if ( shift.x != 0 || shift.y != 0 ) {
				_shape->editResizeQuad( _mode, _quad1, shift * kMove );
			}
		}

		if ( _mode == ModeTuning ) {
			//Сброс тюнинга
			if ( key == 'r' ) {
				_shape->tuningReset();
			}
		}

		return true;
	}
	else return false;
}

//----------------------------------------------------------
void pbMapperEditor::setEditMode( int editMode )
{
	_editMode = editMode;
	if ( editMode != EditVertexEdit ) {
		_v = -1;
	}	
}

//----------------------------------------------------------

