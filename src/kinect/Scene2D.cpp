#include "Scene2D.h"
#include "highgui.h"

//-----------------------------------------------------------------
void Scene2D::setup( float kTruncDepthMM0	//����������, �� ������� ��������
					)
{
	kTruncDepthMM = kTruncDepthMM0;

}


//-----------------------------------------------------------------
const int maxN = 100;		//������������ ����� ����� � �����
bool idTable[ maxN ];

void Scene2D::updateCamera( const Mat &depth, const Mat &labels, const Persons &persons, vector<int> &nearPersonIdLabel )
{
	nearPersonIdLabel.clear();

	int w = depth.size().width;
	int h = depth.size().height;
	if ( w > 0 && h > 0 ) {
		//imshow("depth", depth);
		//��������� ������ ������� �����


		//vector<bool> idTable( maxN, false );	//������� �����
		//vector<int> idList;	//������ �����
		for (int i=0; i<maxN; i++) {
			idTable[i] = false;
		}

		for (int i=0; i<persons.p.size(); i++) {
			const Person &p = persons.p[i];			
			if ( !p.empty() && (p.distance < kTruncDepthMM || kTruncDepthMM == 0 ) ) {
				int id = p.idLabel;
				if ( id >= 0 && id < maxN ) {
					idTable[ id ] = true;
					nearPersonIdLabel.push_back( id );
					//idList.push_back( id );
					//cout << "good " << id << endl;
				}
			}
			//else cout << "bad " << p.idLabel << endl;

		}

		//��������� ����� �� �����, � �����������
		if ( _mask.empty() ) {
			_mask = depth.clone(); 
		}
		if ( _mask8.empty() ) {
			_mask8 = Mat( _mask.size(), CV_8UC1 );
		}

		//_mask.setTo( cv::Scalar(0) );
		_mask8.setTo( cv::Scalar(0) );


		//TODO !!! ��������� ��������
		for(int y = 0; y < h; y++) {
			const unsigned short* pLabel  = labels.ptr<unsigned short>(y);
			const unsigned short* pDepth  = depth.ptr<unsigned short>(y);
			//unsigned short* pMask  = _mask.ptr<unsigned short>(y);
			unsigned char* pMask8  = _mask8.ptr<unsigned char>(y);
			for (int x = 0; x < w; x++) {
				unsigned short label = *pLabel;
				if ( label >= 0 && label < maxN && idTable[ label ] ) {				
					//*pMask = *pDepth;
					*pMask8 = 255 - min( 255 * (*pDepth) / kTruncDepthMM, 255 );					
				}

				pLabel++;
				pDepth++;
				//pMask++;
				pMask8++;
			}
		}

		//������

		vector<ofPoint> headsLast = _heads;
		vector<int> headsLabelLast = _headsLabel;

		_heads.clear();		
		_headsLabel.clear();
		for (int i=0; i<persons.p.size(); i++) {
			const Person &p = persons.p[i];	
			if ( idTable[ p.idLabel ] ) {

				//���� ������ � ���� id � ����������� �����
				int indexLast = -1;
				for (int k=0; k<headsLast.size(); k++) {
					if ( headsLabelLast[k] == p.id ) {
						indexLast = k; 
						break;
					}
				}

				bool isLastHead = (indexLast > -1 );
				ofPoint lastHead;
				if ( isLastHead ) lastHead = headsLast[ indexLast ];


				//TODO �������������� ��� �������������� ������ ���� ������

				ofPoint head = headSearch( p.idLabel, p.x * w, p.y * h, labels,
					isLastHead, lastHead
					);
				if ( head.x >= 0 ) {
					//cout << head.x << " " << head.y << endl;
					//circle( _mask, cv::Point( head.x, head.y ), 30, cv::Scalar( 30000 ), 3 );


					//������ z-���������� - ������, �� ��������� ��������
					head.z = ( kTruncDepthMM > 0 ) ? ( 1 - p.distance / kTruncDepthMM ) 
						: ( 1.0 - min( p.distance / 5000.0, 1.0 ) );	//�������� 5000 - ������������ ����������
					
					//cout << head.z << "  ";

					_heads.push_back( head );
					_headsLabel.push_back( p.id );
				}
			}
		}

		//imshow( "mask", _mask8 );


		//������������ ��������� �����, �� �� ����� 10, ����� persistence
		//� �������� - ���� ������������ ��� �������


	}

}


//-----------------------------------------------------------------
void Scene2D::update( float dt )
{
}

//-----------------------------------------------------------------
void Scene2D::getHeadData( vector<ofPoint> &pos, vector<int> &label )
{
	pos = _heads;
	label = _headsLabel;
}

//-----------------------------------------------------------------
void Scene2D::draw()
{

}

//-----------------------------------------------------------------
struct ScanStat {
	int x0, x1, y;		//������� �������
	int xb0, xb1, yb;	//������ �������
	int widthSum;	//����� �����
	int n;			//������, �.�. widthSum / n - ������� ������
	bool finished;
	ScanStat() {
		x0 = 0; x1 = 0;
		xb0 = 0; xb1 = 0;
		widthSum = 0;
		n = 0;
		finished = false;
	}
};

//-----------------------------------------------------------------
//����� ���������� ������, x,y - ��������, z - ����������
ofPoint Scene2D::headSearch( unsigned short label0, int x0, int y0, const Mat &labels,
							bool isLastHead, ofPoint lastHead
							)
{	
	const float kScale = 0.3;		//��������� ���������
	const int kBlur = 13; //21;		//������ �����, ������ ���� ��������
	const float kThresh = 150;		//����� ����������� �����

	//const int scanX = 100;						//������ ������������ ��� ������� ��������

	int w = labels.size().width;
	int h = labels.size().height;

	//�������� �����
	cv::inRange( labels, cv::Scalar( label0 ), cv::Scalar( label0 + 1), _headMask );
	//imshow( "headmask", _headMask );

	Mat small;
	resize( _headMask, small, cv::Size(), kScale, kScale );

	Mat blurred;
	GaussianBlur( small, blurred, cv::Size( kBlur, kBlur ), 0 );

	//imshow( "blur", blurred );

	Mat bin;
	threshold( blurred, bin, kThresh, 255, CV_THRESH_BINARY );

	//imshow( "bin", bin );

	//Mat large;
	//resize( bin, large, cv::Size(), 1.0 / kScale, 1.0 / kScale, INTER_NEAREST );
	//imshow( "bin", large );
	//large /= 2;


	//Mat vis;	//������������
	//cvtColor( bin, vis, CV_GRAY2RGB );


	//�������� �� ����� �����
	x0 *= kScale;
	y0 *= kScale;
	int x = x0;
	int y = y0-1;

	vector<ScanStat> list;
	vector<ScanStat> list1;

	ScanStat ss;
	ss.xb0 = x0;
	ss.xb1 = x0;
	ss.yb = y0;
	ss.x0 = x0;
	ss.x1 = x0;
	ss.y  = y;
	list.push_back( ss );

	while ( y > 0 ) {
		const unsigned char* pMask  = bin.ptr<unsigned char>(y);
		list1.clear();
		for (int i=0; i<list.size(); i++) {
			//��������� ����
			ScanStat &ss = list[i];
			if ( !ss.finished ) {
				//������ �������
				//�������� �����
				int left = ss.x0;
				int leftGood = left;
				while ( left >= 0 && ( pMask[ left ] || left >= ss.x0 ) ) {
					if ( pMask[left] ) leftGood = left;
					left--;
				}	

				//�������� ������
				int right = ss.x1;
				int rightGood = right;
				while ( right < w && ( pMask[ right ] || right <= ss.x1 ) ) {
					if ( pMask[right] ) rightGood = right;
					right++;
				}

				//���� �������
				int count = 0;				
				int x = leftGood;
				while ( x < rightGood ) {
					while ( x < rightGood && !pMask[x] ) x++;
					int x1 = x + 1;
					while ( x1 < rightGood && pMask[x1] ) x1++;
					if ( x < rightGood ) {
						//��� ����� �������
						ScanStat s = ss;
						s.n++;
						s.x0 = x;
						s.x1 = x1-1;
						s.y = y;
						list1.push_back( s );
						count++;						
					}
					x = x1 + 1;
				}
				if ( count == 0 ) {
					//��������� ����������
					ss.finished = true;
				}
			}

			if ( ss.finished ) {//��� ����� ����� �����
				list1.push_back( ss );
			}

		}
		y--;
		list = list1;
	}


	/*for (int i=0; i<list.size(); i++) {
	const ScanStat &ss = list[i];			
	line( vis, cv::Point( ss.x0, ss.y ), cv::Point( ss.x1, ss.y ), cv::Scalar( 0, 0, 255 ), 5 );
	}*/


	//���� ����� ������� � ������ � �������
	float best = -1;
	int bestIndex = -1;


	for (int i=0; i<list.size(); i++) {
		const ScanStat &ss = list[i];		
		float x = (ss.x0 + ss.x1) / 2.0;
		float y = ss.y;
		if ( y != y0 ) {
			//���������� "������ ������"
			float val = //fabs( ( x - x0 ) / ( y - y0 ) );
				//fabs( ( x - x0 ) / (( y - y0 )*( y - y0 )) );	
				fabs( ( x - x0 ) / (( y - y0 )*( y - y0 )) ) - fabs( (y - y0) / 300000 );
			if ( bestIndex == -1 || val < best ) {
				best = val;
				bestIndex = i;
			}
		}
	}

	//circle( vis, cv::Point( x0, y0 ), 10, cv::Scalar( 0, 0, 255 ), -1 );

	ofPoint head( -1, -1 );


	if ( bestIndex != - 1 ) {
		const ScanStat &ss = list[bestIndex];	
		head.x = (ss.x0 + ss.x1 ) / 2;
		head.y = ss.y;

		//line( vis, cv::Point( x0, y0 ), cv::Point( head.x, head.y ), cv::Scalar( 0, 0, 255 ), 3 );
		head.x /= kScale;
		head.y /= kScale;
	}

	//imshow( "bin", vis );



	return head;
}


//-----------------------------------------------------------------


