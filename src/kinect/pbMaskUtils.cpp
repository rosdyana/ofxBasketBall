#include "pbMaskUtils.h"

//------------------------------------------------------------------------
//ближайшая точка к 8-битной маске. Если маска пустая - то (-1,-1)
ofPoint pbMaskUtils::nearestPointToMask(const Mat &mask, const ofPoint &p, float pW, float pH, ofRectangle rect)
{
    int w = mask.size().width;
    int h = mask.size().height;
    if (rect.x < 0) {
        rect = ofRectangle(0, 0, w, h);
    }
    cv::Rect r = cv::Rect(rect.x, rect.y, rect.width, rect.height) & cv::Rect(0, 0, w, h);

    ofPoint res(-1, 1);
    float bestDist = -1;
    float scaleX = (pW < 0) ? w : pW;
    scaleX = w / scaleX;
    float scaleY = (pH < 0) ? h : pH;
    scaleY = h / scaleY;
    float userX = p.x * scaleX;
    float userY = p.y * scaleY;
    for (int y = r.y; y < r.y + r.height; y++) {
        for (int x = r.x; x < r.x + r.width; x++) {
            int value = mask.at<unsigned char>(y, x);
            if (value > 0) {
                float dist = ofDistSquared(userX, userY, x, y);
                if (bestDist < 0 || dist < bestDist) {
                    bestDist = dist;
                    res.x = x;
                    res.y = y;
                }
            }
        }
    }
    if (bestDist >= 0) {
        res.x /= scaleX;
        res.y /= scaleY;
    }

    return res;
}

//------------------------------------------------------------------------
