#include "pbScreenLog.h"

//-----------------------------------------------------------------------------------------
void pbScreenLog::setup(const string &fontFileName, int fontSize, float secToAddEmptyLines, int capacity)
{
    _font.loadFont(fontFileName, fontSize);

    _secToAddEmptyLines = secToAddEmptyLines;
    _capacity = capacity;

    clear();
}

//-----------------------------------------------------------------------------------------
void pbScreenLog::update()		//надо вызывать чтоб добавлял пустые строки
{
    if (_secToAddEmptyLines > 0) {
        float time = ofGetElapsedTimef();
        if (time > _secToAddEmptyLines + _lastAddTime) {
            add("...", 0x00000000);
        }
    }
}


//-----------------------------------------------------------------------------------------
void pbScreenLog::clear()
{
    _messageText.clear();
    _messageColor.clear();
    _lastAddTime = ofGetElapsedTimef();
}

//-----------------------------------------------------------------------------------------
void pbScreenLog::add(const string &message, int color)
{
    _lastAddTime = ofGetElapsedTimef();

    _messageText.push_back(message);
    _messageColor.push_back(color);
    if (_messageText.size() >= _capacity) {
        _messageText.erase(_messageText.begin());
        _messageColor.erase(_messageColor.begin());
    }
}

//-----------------------------------------------------------------------------------------
void pbScreenLog::draw(ofRectangle rect, int bR, int bG, int bB, int bA)
{
    ofSetColor(bR, bG, bB, bA);
    ofFill();
    ofRect(rect.x, rect.y, rect.width, rect.height);
    int y = rect.y + rect.height;
    for (int i = _messageText.size() - 1; i >= 0 && y > rect.y; i--) {
        string text = _messageText[i];
        ofRectangle r = _font.getStringBoundingBox(text, 0, 0);
        ofSetColor(_messageColor[i]);
        y -= r.height * 1.2;
        if (y - r.height >= rect.y) {
            _font.drawString(text, rect.x, y);
        } else {
            break;
        }
    }

}

//-----------------------------------------------------------------------------------------