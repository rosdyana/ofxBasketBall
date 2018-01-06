#pragma once

//Анализ сцены - сбор списка людей в кадре

#include "cv.h"
#include "ofMain.h"
#include "PersonType.h"

using namespace cv;

class SceneAnalys
{
public:
	SceneAnalys(void);
	~SceneAnalys(void);

	void setup( bool increaseId = false,	//увеличивать ли id при появлении нового человека
				int persistentFrames = 0,	//число кадров, на сколько хранится пропавший человек
				int recognitionFaceCount = 5,	//число срабатываний детектора лиц, когда превысит - ставится gender
				int recognitionFrameCount = 60	////число кадров. Если превысили и ест ьхоть одно срабатывание лица - ставится gender
				);

	//фильтр людей по расстоянию. если <= 0, то не фильтровать
	void setFilterZ( float filterZ ) { _filterZ = filterZ; }

	void update( int w, int h, unsigned short *depth, unsigned short *labels );

	void applyFaces( const vector<cv::Rect> &faces,
		const vector<string> &genders,
		const Mat &labelsMat		
		);		//применить к текущей сцене поступившие знания о лицах


	const Persons &persons() const { return _persOut; }

	vector<Person> &persist() { return _persist; } //список людей по id кинекта

	const Mat labelsMat();			
	const Mat depthMat();

	//для посылки серверу
	string message() { return _persOut.message(); }
private:
	int _w, _h;
	unsigned short *_depth;	//временный указатель
	unsigned short *_labels;	//временный указатель
		
	int _frame;
	
	
	Persons _pers;					//для расчета
	Persons _persOut;				//для вывода

	bool _kIncreaseId;			//увеличивать ли id
	int _kPersistentFrames;		//сколько кадров хранить пропавших людей

	void persistenceUpdate();	//обновление с учетом _kIncreaseId и _kPersistentFrames

	vector<Person> _persist;	//хранение людей с предыдущих кадров
	int _totalId;				//максимальный id

	float _filterZ;				//фильтр людей по расстоянию. если <= 0, то не фильтровать

};

extern SceneAnalys shared_scene;

