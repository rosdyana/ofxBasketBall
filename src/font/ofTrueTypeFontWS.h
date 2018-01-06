#ifndef _OF_TTF_FONT_WS_H_
#define _OF_TTF_FONT_WS_H_


#include "ofTrueTypeFont.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;

class ofTrueTypeFontWS : public ofTrueTypeFont
{
public:
    ofTrueTypeFontWS() {};
    virtual ~ofTrueTypeFontWS();

    void loadFont(string filename, int fontsize,
                  bool _bAntiAliased = true,
                  bool _bFullCharacterSet = true,
                  bool makeContours = false);

    float stringWidth(wstring s);
    float stringHeight(wstring s);

    ofRectangle getStringBoundingBox(wstring s, float x, float y);

    void drawString(wstring s, float x, float y);
    void drawStringAsShapes(wstring s, float x, float y);

private:
    typedef struct FT_LibraryRec_  *FT_Library;
    typedef struct FT_FaceRec_  *FT_Face;
    FT_Library library;
    FT_Face face;
    static const int TYPEFACE_UNLOADED;
    void loadEachChar(int char_id, int fontsize,
                      bool _bAntiAliased, bool _bFullCharacterSet);
};

#endif

