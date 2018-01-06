#include "PersonType.h"

//----------------------------------------------
int kRecognitionFaceCount = 5;		//����� ������������ ��������� ���, ����� �������� - �������� gender
int kRecognitionFrameCount = 60;	//����� ������. ���� ��������� � ��� ����� ���� ������������ ���� - �������� gender
//----------------------------------------------


template <class T>
string xmlAttrNumber(string attrName, T value)
{
    return attrName + "=\"" + ofToString(value) + "\"";
}

string xmlAttrString(string attrName, string value)
{
    return attrName + "=\"" + value + "\"";
}

string Person::str() const
{
    string s = string("<person")
               + " " + xmlAttrNumber("id", id)
               + " " + xmlAttrNumber("x", x)
               + " " + xmlAttrNumber("y", y)
               + " " + xmlAttrNumber("distance", distance)
               + " " + xmlAttrNumber("faceDetected", (faceDetected) ? 1 : 0);
    if (gender != "") {
        s += " " + xmlAttrString("gender", gender);
    }
    //s += " " + xmlAttrNumber( "age", age );
    s +=		"/>";

    return s;
}