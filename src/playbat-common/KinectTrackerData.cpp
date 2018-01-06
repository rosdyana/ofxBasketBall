#include "KinectTrackerData.h"
#include "ofxIniSettings.h"
#include "ofxXmlSettings.h"



//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------- KinectTrackerCalibrator ----------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//Калибровка данных
//xml-файл
void KinectTrackerCalibrator::setup( const string &fileName, float w, float h )
{
	_fileName = fileName;
	_w = w;
	_h = h;
	_enabled = true;

	load();
	save();
}

//--------------------------------------------------------------	
ofPoint xmlLoadPoint( ofxXmlSettings &xml, const string &tag, const ofPoint &pDef )
{
	ofPoint p;
	p.x = xml.getValue( tag + ":x", pDef.x );
	p.y = xml.getValue( tag + ":y", pDef.y );
	p.z = xml.getValue( tag + ":z", pDef.z );
	return p;
}

void xmlSavePoint( ofxXmlSettings &xml, const string &tag, const ofPoint &p )
{
	xml.setValue( tag + ":x", p.x );
	xml.setValue( tag + ":y", p.y );
	xml.setValue( tag + ":z", p.z );
}

//--------------------------------------------------------------	
void KinectTrackerCalibrator::load()
{
//считывание xml
	ofxXmlSettings xml;
	xml.loadFile( _fileName );
	
	string section = "calibration:";
	_screenSizeMm	= xmlLoadPoint( xml, section + "screenSizeMm", ofPoint( 3500, 0 ) );
	_roomSizeMm		= xmlLoadPoint( xml, section + "roomSizeMm",   ofPoint( 7000, 6000, 5000 ) );
	_shiftMm		= xmlLoadPoint( xml, section + "shiftMm",   ofPoint(0, 0, 0) );
	_rearShiftMm	= xmlLoadPoint( xml, section + "rearShiftMm",   ofPoint(0, 0, 0) );

	_skewZ			= xml.getValue( section + "skewZ", 0.0f );

	if ( _screenSizeMm.y == 0 ) {
		_screenSizeMm.y = _screenSizeMm.x * _h / _w;
	}
	_scale = ofPoint( _w / _screenSizeMm.x, -_h / _screenSizeMm.y, 1 );
	
//	_shift += ofPoint( shift.x * _scale.x, shift.y * _scale.y );
}

//--------------------------------------------------------------	
void KinectTrackerCalibrator::save()
{
	ofxXmlSettings xml;
	xml.loadFile( _fileName );

	string section = "calibration:";
	xmlSavePoint( xml, section + "screenSizeMm", _screenSizeMm );
	xmlSavePoint( xml, section + "roomSizeMm",   _roomSizeMm );
	xmlSavePoint( xml, section + "shiftMm",   _shiftMm );
	xmlSavePoint( xml, section + "rearShiftMm",   _rearShiftMm );

	xml.setValue( section + "skewZ", _skewZ );

	xml.saveFile( _fileName );
}

//--------------------------------------------------------------	
//конвертировать миллиметры в экранные координаты, с учетом сдвига и поворота камеры
void KinectTrackerCalibrator::convertMmToPixel( KinectTrackerDataItem &item )
{
	if ( item.valid && _enabled ) {
		ofPoint &mm = item.mm;
		ofPoint &pixel = item.pixel;
		pixel.x = _w/2 + _scale.x * (mm.x 
			+ ofMap( mm.z, 0, _roomSizeMm.z, _shiftMm.x, _shiftMm.x + _rearShiftMm.x ) );
		pixel.y = _h/2 + _scale.y * (mm.y 
			+ ofMap( mm.z, 0, _roomSizeMm.z, _shiftMm.y, _shiftMm.y + _rearShiftMm.y ) );
		pixel.z = mm.z * _scale.z;
	}
}

//--------------------------------------------------------------	
void KinectTrackerCalibrator::convertMmToPixel( KinectTrackerDataPerson &person )
{
	for (int i=0; i<person.item.size(); i++) {
		convertMmToPixel( person.item[i] );
	}
}

//--------------------------------------------------------------	
void KinectTrackerCalibrator::convertMmToMm( const KinectTrackerDataItem &item, KinectTrackerDataItem &itemOut )
{
	itemOut = item;
	if ( item.valid && _enabled ) {
		const ofPoint &mm = item.mm;
		ofPoint &mmOut = itemOut.mm;
		mmOut.x = mm.x + ofMap( mm.z, 0, _roomSizeMm.z, _shiftMm.x, _shiftMm.x + _rearShiftMm.x );
		mmOut.y = mm.y + ofMap( mm.z, 0, _roomSizeMm.z, _shiftMm.y, _shiftMm.y + _rearShiftMm.y );
		mmOut.z = mm.z;
	} 
}

//--------------------------------------------------------------
void KinectTrackerCalibrator::shiftReset( int topSide )	//сбросить сдвиги сбоку или сверху
{
	if ( topSide == 0 ) {
		_shiftMm.x = 0;
		_rearShiftMm.x = 0;
	}
	else {
		_shiftMm.y = 0;
		_rearShiftMm.y = 0;
	}
	save();
}

//--------------------------------------------------------------
void KinectTrackerCalibrator::skewReset()				//сброс сдвига
{
	_skewZ = 0;
	save();
}

//--------------------------------------------------------------
void KinectTrackerCalibrator::skewMove( float shiftSkewZ )	//сдвиг
{
	_skewZ += shiftSkewZ;
	save();
}

//--------------------------------------------------------------
void KinectTrackerCalibrator::shiftMove( const ofPoint &shiftMm, const ofPoint &rearShiftMm )
{
	_shiftMm += shiftMm;
	_rearShiftMm += rearShiftMm;
	save();
}

//--------------------------------------------------------------
//рисовать сверху или сбоку
void KinectTrackerCalibrator::drawTopSideMm( const vector<KinectTrackerDataPerson> &persons, int topSide )	
{
	ofPushMatrix();
	ofSetLineWidth( 3 );

	//сдвиг
	if ( topSide == 0 ) { ofTranslate( _w/2, 50 ); }
	else { ofTranslate( 50, _h/2 ); }

	//масштаб
	if ( topSide == 0 ) {
		float scl = min( _w / _roomSizeMm.x, _h / _roomSizeMm.z );
		scl *= 0.9;
		ofScale( scl, scl );
	}
	else {
		float scl = min( _w / _roomSizeMm.z, _h / _roomSizeMm.y );	
		scl *= 0.9;
		ofScale( scl, -scl );
	}

	//габариты
	ofNoFill();
	ofSetColor( 128, 128, 128 );
	if ( topSide == 0 ) { ofRect( -_roomSizeMm.x/2, 0, _roomSizeMm.x, _roomSizeMm.z ); }
	else { ofRect( 0, -_roomSizeMm.y/2, _roomSizeMm.z, _roomSizeMm.y ); } 
	
	//экран
	ofSetColor( 255, 255, 255 );
	ofFill();
	if ( topSide == 0 ) { ofRect( -_screenSizeMm.x/2, -100, _screenSizeMm.x, 100 ); }
	else { ofRect( -100, -_screenSizeMm.y/2, 100, _screenSizeMm.y ); }

	//линии каждый метр по z
	ofSetColor( 128, 128, 128 );
	for ( int z=0; z<=_roomSizeMm.z; z+=1000 ) {
		if ( topSide == 0 ) {	ofLine( -_roomSizeMm.x/2, z, _roomSizeMm.x/2, z ); }
		else { ofLine( z, -_roomSizeMm.y/2, z, _roomSizeMm.y/2 ); }
	}
	//другие линии
	if ( topSide == 0 ) {
		for ( int x = 0; x <= _roomSizeMm.x/2; x+= 1000 ) {
			ofLine( x, 0, x, _roomSizeMm.z );
			ofLine( -x, 0, -x, _roomSizeMm.z );
		}
	}
	else {
		for ( int y = 0; y <= _roomSizeMm.y/2; y+= 1000 ) {
			ofLine( 0, y, _roomSizeMm.z, y );
			ofLine( 0, -y, _roomSizeMm.z, -y );
		}
	}

	//if ( topSide == 0 ) { ofLine( 0, 0, 0, _roomSizeMm.z ); }
	//else { ofLine( 0, 0, _roomSizeMm.z, 0 ); }

	//сдвиг камеры
	ofSetColor( 255, 0, 255 );
	if ( topSide == 0 ) {	ofLine( _shiftMm.x, 0, _shiftMm.x + _rearShiftMm.x, _roomSizeMm.z ); }
	else { ofLine( 0, _shiftMm.y, _roomSizeMm.z, _shiftMm.y + _rearShiftMm.y );	}



	//люди
	for (int k=0; k<persons.size(); k++) {
		const KinectTrackerDataPerson &person0 = persons[k];
		KinectTrackerDataPerson person = person0;
		for ( int i=0; i<person.item.size(); i++ ) {
			convertMmToMm( person0.item[i], person.item[i] );
		}
		//отрезки
		ofSetColor( 0, 128, 255 );
		person.drawSegmentTopSideMm( kKinectLFoot, kKinectTorso, topSide );
		person.drawSegmentTopSideMm( kKinectRFoot, kKinectTorso, topSide );
		ofSetColor( 255, 255, 0 );
		person.drawSegmentTopSideMm( kKinectLHand, kKinectLElbow, topSide );
		person.drawSegmentTopSideMm( kKinectRHand, kKinectRElbow, topSide );
		person.drawSegmentTopSideMm( kKinectLElbow, kKinectLShoulder, topSide );
		person.drawSegmentTopSideMm( kKinectRElbow, kKinectRShoulder, topSide );
		
		//точки
		for (int i=0; i<person.item.size(); i++) {
			KinectTrackerDataItem &it = person.item[i];
			if (it.valid && kinectIsBodyPart(i) ) {
				ofFill();
				ofColor c = kinectPartColor( i );
				ofSetColor( c.r, c.g, c.b );
				if ( topSide == 0 ) { ofCircle( it.mm.x, it.mm.z, 50 ); }
				else { ofCircle( it.mm.z, it.mm.y, 50 ); }
			}
		}
	}

	ofSetLineWidth( 1 );
	ofPopMatrix();
}

//--------------------------------------------------------------
//рисовать спереди, калиброванное
void KinectTrackerCalibrator::drawFrontPixels( const vector<KinectTrackerDataPerson> &persons )
{
	//экран
	ofFill();
	ofSetColor( 128, 128, 128 );
	ofRect( 0, 0, _w, _h );
	
	ofSetLineWidth( 2 );
	//люди
	for (int k=0; k<persons.size(); k++) {
		const KinectTrackerDataPerson &person0 = persons[k];
		KinectTrackerDataPerson person = person0;
		for ( int i=0; i<person.item.size(); i++ ) {
			convertMmToPixel( person.item[i] );
		}
		//отрезки
		ofSetColor( 0, 128, 255 );
		person.drawSegmentFrontPixel( kKinectLFoot, kKinectTorso );
		person.drawSegmentFrontPixel( kKinectRFoot, kKinectTorso );
		ofSetColor( 255, 255, 0 );
		person.drawSegmentFrontPixel( kKinectLHand, kKinectLElbow );
		person.drawSegmentFrontPixel( kKinectRHand, kKinectRElbow );
		person.drawSegmentFrontPixel( kKinectLElbow, kKinectLShoulder );
		person.drawSegmentFrontPixel( kKinectRElbow, kKinectRShoulder );
		
		//точки
		for (int i=0; i<person.item.size(); i++) {
			KinectTrackerDataItem &it = person.item[i];
			if (it.valid && kinectIsBodyPart(i) ) {
				ofFill();
				ofColor c = kinectPartColor( i );
				ofSetColor( c.r, c.g, c.b );
				ofCircle( it.pixel.x, it.pixel.y, 10 );
			}
		}
	}

	ofSetLineWidth( 1 );
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
int kinectNameToInt( const string &name ) 
{
	if ( name == "torso" ) return kKinectTorso;
	if ( name == "boundBox0" ) return kKinectBoundBox0;
	if ( name == "boundBox1" ) return kKinectBoundBox1;
	if ( name == "head" ) return kKinectHead;
	if ( name == "neck" ) return kKinectNeck;
	if ( name == "LShoulder" ) return kKinectLShoulder;
	if ( name == "RShoulder" ) return kKinectRShoulder;
	if ( name == "LElbow" ) return kKinectLElbow;
	if ( name == "RElbow" ) return kKinectRElbow;
	if ( name == "LHand" ) return kKinectLHand;
	if ( name == "RHand" ) return kKinectRHand;
	if ( name == "LHip" ) return kKinectLHip;
	if ( name == "RHip" ) return kKinectRHip;
	if ( name == "LKnee" ) return kKinectLKnee;
	if ( name == "RKnee" ) return kKinectRKnee;
	if ( name == "LFoot" ) return kKinectLFoot;
	if ( name == "RFoot" ) return kKinectRFoot;
	return -1;
}

string kinectIntToName( int index )
{
	if ( index == kKinectTorso ) return "torso"; 
	if ( index == kKinectBoundBox0 ) return "boundBox0"; 
	if ( index == kKinectBoundBox1 ) return "boundBox1"; 
	if ( index == kKinectHead ) return "head";  
	if ( index == kKinectNeck ) return "neck";  
	if ( index == kKinectLShoulder ) return "LShoulder"; 
	if ( index == kKinectRShoulder ) return "RShoulder";  
	if ( index == kKinectLElbow ) return "LElbow"; 
	if ( index == kKinectRElbow ) return "RElbow"; 
	if ( index == kKinectLHand ) return "LHand"; 
	if ( index == kKinectRHand ) return "RHand"; 
	if ( index == kKinectLHip ) return "LHip"; 
	if ( index == kKinectRHip )return "RHip";  
	if ( index == kKinectLKnee )return "LKnee";  
	if ( index == kKinectRKnee ) return "RKnee"; 
	if ( index == kKinectLFoot ) return "LFoot"; 
	if ( index == kKinectRFoot )  return "RFoot"; 
	return "";
}

int kinectIsBodyPart( int i )
{
	return ( i != kKinectBoundBox0 && i != kKinectBoundBox1 );
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//------------------  KinectTrackerDataPerson ------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

KinectTrackerDataPerson::KinectTrackerDataPerson()
{
	id = -1;
	state = KinectPersonEmpty;
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::setup( float w, float h, const KinectTrackerDataParam &param )
{
	_param = param;
	_w = w;
	_h = h;
	state = KinectPersonEmpty;
	_time = 0;
	item.resize( kKinectPoints );
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::setState( int state0 )
{
	state = state0;
	if ( state == KinectPersonEmpty ) {
		for (int i=0; i<item.size(); i++) {
			item[i].valid = false;
		}
	}
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::updateReceiver( float dt )
{
	_time += dt;
	if ( !empty() ) {
		if ( _time > _param.receiverWaitNoTracker ) {
			setState( KinectPersonEmpty );
		}
	}
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::boundBoxMm( ofPoint &origin, ofPoint &size ) //возвращает габаритный прямоугольник с данных
{
	if ( item[kKinectBoundBox0].valid && item[kKinectBoundBox1].valid ) {
		//был расчет габаритного прямоугольника по маске
		origin = item[kKinectBoundBox0].mm;
		size = item[kKinectBoundBox1].mm - origin;
	}
	else {
		//считаем по габаритам найденных частей
		ofPoint p0( 10000, 10000, 10000 );
		ofPoint p1( -10000, -10000, -10000 );
		for (int i=0; i<item.size(); i++) {
			KinectTrackerDataItem &it = item[i];
			if (it.valid && kinectIsBodyPart(i)) {
				ofPoint &p = item[i].mm;
				p0.x = min( p0.x, p.x );
				p0.y = min( p0.y, p.y );
				p0.z = min( p0.z, p.z );
				p1.x = max( p1.x, p.x );
				p1.y = max( p1.y, p.y );
				p1.z = max( p1.z, p.z );
			}
		}
		origin = p0;
		size = p1 - p0;
	}
}

//--------------------------------------------------------------
ofColor kinectPartColor( int i )
{
	ofColor color;
	color.r = int((i+5) * 103) % 255;
	color.g = int((i+3) * 33) % 255;
	color.b = int((i+1) * 5581) % 255;
	return color;
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::drawSegment( float w, float h, int index0, int index1 )
{
	KinectTrackerDataItem &item0 = item[ index0 ];
	KinectTrackerDataItem &item1 = item[ index1 ];

	if ( item0.valid && item1.valid ) {
		float sclX = w / _w;
		float sclY = h / _h;
		ofLine( item0.pixel.x, item0.pixel.y, item1.pixel.x, item1.pixel.y ); 
	}
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::draw( float w, float h )			//рисовать вид спереди, в пикселах
{
	if ( !empty() ) {
		float sclX = w / _w;
		float sclY = h / _h;

		//отрезки
		ofSetLineWidth( 3 );
		ofSetColor( 0, 128, 255 );
		drawSegment( w, h, kKinectLFoot, kKinectTorso );
		drawSegment( w, h, kKinectRFoot, kKinectTorso );
		ofSetColor( 255, 255, 0 );
		drawSegment( w, h, kKinectLHand, kKinectLElbow );
		drawSegment( w, h, kKinectRHand, kKinectRElbow );
		drawSegment( w, h, kKinectRElbow, kKinectRShoulder );
		drawSegment( w, h, kKinectLElbow, kKinectLShoulder );
		ofSetLineWidth( 1 );

		//точки
		for (int i=0; i<item.size(); i++) {
			KinectTrackerDataItem &it = item[i];
			if (it.valid && kinectIsBodyPart(i) ) {
				ofFill();
				ofColor c = kinectPartColor( i );
				ofSetColor( c.r, c.g, c.b );
				ofCircle( it.pixel.x * sclX, it.pixel.y * sclY, 10 );
			}
		}
		//габаритный прямоугольник - возможно, работает только в рендере
		if ( item[kKinectBoundBox0].valid && item[kKinectBoundBox1].valid ) {
			ofPoint p0 = item[kKinectBoundBox0].pixel;
			ofPoint p1 = item[kKinectBoundBox1].pixel;
			ofPoint size = p1 - p0;
			if ( size.x >= 0 ) { // && size.y >= 0 ) {
				ofNoFill();
				ofSetColor( 128, 128, 128 );
				ofRect( p0.x * sclX, p0.y * sclY, size.x * sclX, size.y * sclY );
			}
		}
	}
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::drawSegmentTopSideMm( int index0, int index1, int topSide )	//не ставит цвет
{
	KinectTrackerDataItem &item0 = item[ index0 ];
	KinectTrackerDataItem &item1 = item[ index1 ];

	if ( item0.valid && item1.valid ) {
		if ( topSide == 0 ) {
			ofLine( item0.mm.x, item0.mm.z, item1.mm.x, item1.mm.z );	//сверху
		}
		else {
			ofLine( item0.mm.z, item0.mm.y, item1.mm.z, item1.mm.y );	//сбоку
		}
	}
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::drawSegmentFrontPixel( int index0, int index1 )		//не ставит цвет
{
	KinectTrackerDataItem &item0 = item[ index0 ];
	KinectTrackerDataItem &item1 = item[ index1 ];

	if ( item0.valid && item1.valid ) {
		ofLine( item0.pixel.x, item0.pixel.y, item1.pixel.x, item1.pixel.y );	
	}
}

//--------------------------------------------------------------
/*void KinectTrackerDataPerson::drawFrontMm( float w, float h )	//рисовать вид спереди, мм
{
	if ( !empty() ) {
		float scl = w / 5000.0;
		for (int i=0; i<item.size(); i++) {
			KinectTrackerDataItem &it = item[i];
			if (it.valid && kinectIsBodyPart(i)) {
				ofFill();
				ofColor c = kinectPartColor( i );
				ofSetColor( c.r, c.g, c.b );
				ofPoint p = it.mm;
				p.x = w/2 + p.x * scl;
				p.y = h/2 - p.y * scl;
				ofCircle( p.x, p.y, 7 );
			}
		}
		//габаритный прямоугольник
		ofPoint origin, size;
		boundBoxMm( origin, size );
		if ( size.x >= 0 && size.y >= 0 ) {
			ofNoFill();
			ofSetColor( 128, 128, 128 );
			ofRect( origin.x * scl + w/2, -origin.y * scl + h/2, size.x * scl, -size.y * scl );
		}
	}
}
*/

//--------------------------------------------------------------
/*string intToStringFixed( int value, int len )
{
	string s;
	if (len == 4) {
		value = ( value >= 0 ) ? value : 0;
		value = ( value <= 9999 ) ? value : 9999;
		s = ofToString( value );
		while ( s.length() < len ) { s = "0" + s; }
	}
	return s;

}*/

//--------------------------------------------------------------
//для передачи по сети
string KinectTrackerDataPerson::toString( bool pixels, bool mm, float w, float h,
										 const vector<int> &enabledParts)		
{
	string s = "";
	if ( !empty() ) {
		s += ofToString( id );
		for (int i=0; i<item.size(); i++) {
			KinectTrackerDataItem &it = item[i];
			if ( enabledParts[i] ) {
				if ( it.valid ) { 
					if ( pixels ) {
						s += ";" + ofToString( it.pixel.x / w, 4 )			//внимание, этот режим очень расходен по длине строки!			
							+ ";" + ofToString( it.pixel.y / h, 4 )
							+ ";" + ofToString( it.pixel.z, 1 );
					}
					if ( mm ) {
						s += ";" + ofToString( int(it.mm.x) )
							+ ";" + ofToString( int(it.mm.y) )
							+ ";" + ofToString( int(it.mm.z) );
						//s += ";" + intToStringFixed( it.mm.x + 5000, 4 ) 
						//	+ intToStringFixed( it.mm.y + 5000, 4 ) 
						//	+ intToStringFixed( it.mm.y + 5000, 4 );
					}
				}
				else {
					s += ";-;-;-";
				}
			}
		}
	}
	return s;
}

//--------------------------------------------------------------
void KinectTrackerDataPerson::updateCameraReceiver( bool signal )			//просто знак что пришел сигнал с камеры
{
	if ( signal ) {
		_time = 0;
		setState( KinectPersonTracking );
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//------------------  KinectTrackerDataParam -------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void KinectTrackerDataParam::load( ofxIniSettings &ini )
{ 
	string section = "Tracking.";
	receiverWaitNoTracker = ini.get( section + "receiverWaitNoTracker", 0.0f );
	
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//------------------  KinectTrackerData ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
KinectTrackerData::KinectTrackerData()
{

}

//--------------------------------------------------------------
void KinectTrackerData::setup( float w, float h, const string &fileName, const string &calibrFileName,
							  int autobots
						)
{
	ofxIniSettings ini;
	ini.load( ofToDataPath( fileName ) );

	_param.load( ini );

	_w = w;
	_h = h;
	_N = KinectMaxPersons + autobots;				
	_person.resize( _N );
	for (int i=0; i<_N; i++) {
		_person[i].setup( _w, _h, _param );
		_person[i].id = i;
	}

	//какие части использовать
	_enabledParts.resize( kKinectPoints );

	cout << "Tracking: " << endl;
	for (int i=0; i<kKinectPoints; i++) {
		string section = "Tracking.";
		string name = kinectIntToName( i );
		_enabledParts[i] = ini.get( section + name, 0 );
		if (_enabledParts[i]) {
			cout << "\t" << name << endl;
		}
	}

	_calibrator.setup( calibrFileName, _w, _h );
}

//--------------------------------------------------------------
void KinectTrackerData::updateReceiver( float dt )
{
	for (int i=0; i<_N; i++) {
		_person[i].updateReceiver( dt );
	}
}

//--------------------------------------------------------------
void KinectTrackerData::clear()
{
	for (int k=0; k<_person.size(); k++) {
		_person[k].setState( KinectPersonEmpty );
	}

}

//--------------------------------------------------------------
void KinectTrackerData::updateFromString( const string &s )
{
	//mm|1;100;-73;1019;-272;-437;781;470;439;1307;-;-;-;-;-;-;-;-;-;-;-;-;-;-;-;-;-;-

	//clear();

	vector<string> s1 = ofSplitString( s, "|" );
	if ( s1.size() >= 2 ) {
		if ( s1[0] == "mm" ) {	//данные в мм
			for (int j=1; j<s1.size(); j++) {
				vector<string> data = ofSplitString( s1[j], ";" );
				if ( data.size() > 0 ) {
					int k=0;
					int id = ofToInt( data[k++] );
					if ( id >= 0 && id < _N ) {
						KinectTrackerDataPerson &person = _person[id];
						person.updateCameraReceiver( true );
						//person.setState( KinectPersonTracking );
						vector<KinectTrackerDataItem> &item = person.item;
						for (int i=0; i<kKinectPoints; i++) {
							if ( _enabledParts[i] ) {
								if ( k + 2 < data.size() ) {
									string x = data[ k++ ];
									string y = data[ k++ ];
									string z = data[ k++ ];
									if ( x == "-" || y == "-" || z == "-" ) {
										item[i].update( false, ofPoint(), ofPoint() );
									}
									else {
										ofPoint mm( ofToFloat( x ), ofToFloat( y ), ofToFloat( z ) );
										item[i].update( true, ofPoint(), mm );
									}
								}
								else {
									cout << "ERROR parsing " << s << endl;
									cout << "Check tracker.ini in Tracker and Render!" << endl;
									return;
								}
							}
						}
					}
					else {
						cout << "ERROR parsing " << s << endl;
						cout << "Check tracker.ini in Tracker and Render!" << endl;
						return;
					}
				}
			}
		}
	}


	//Получаем писельные значения
	if ( _calibrator.enabled() ) {
		for (int k=0; k<_person.size(); k++) {
			KinectTrackerDataPerson &person = _person[k];
			if ( !person.empty() ) {
				vector<KinectTrackerDataItem> &item = person.item;
				for (int i=0; i<item.size(); i++) {
					_calibrator.convertMmToPixel( item[i] );
				}
			}
		}
	}
}

//--------------------------------------------------------------
string KinectTrackerData::toString( bool pixels, bool mm, float wPix, float hPix )		//для передачи по сети
{
	string s = "";
	if ( pixels ) { s += "pix"; }
	if ( mm ) { s += "mm"; }

	for (int i=0; i<_N; i++) {
		KinectTrackerDataPerson &person = _person[i];
		string s1 = person.toString( pixels, mm, wPix, hPix, enabledParts() );
		if ( s1 != "" ) {
			s += "|" + s1;
		}
	}
	return s;
}	

//--------------------------------------------------------------
void KinectTrackerData::draw( float w, float h )			//рисовать вид спереди, в пикселах
{
	for (int k=0; k<_person.size(); k++) {
		_person[k].draw( w, h );
	}
}

//---------------------------------------------------------------------
void KinectTrackerData::drawFrontPixels()					//рисовать вид спереди, калиброванный, в пикселах
{
	_calibrator.drawFrontPixels( _person );
}

//---------------------------------------------------------------------
void KinectTrackerData::drawTopMm()	//рисовать вид сверху, мм
{
	_calibrator.drawTopSideMm( _person, 0 );
}

//---------------------------------------------------------------------
void KinectTrackerData::drawSideMm()	//рисовать вид спереди, мм
{
	_calibrator.drawTopSideMm( _person, 1 );
}

//---------------------------------------------------------------------
