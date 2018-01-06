/*
 *  ofxFBOTexture.cpp
 *  openFrameworks
 *
 *  Created by Zach Gage on 3/28/08.
 *  Copyright 2008 STFJ.NET. All rights reserved.
 *
 *  http://addons.openframeworks.cc/projects/show/ofxfbotexture
 *
 *  Updated 22nd July 2009
 *
 */

#ifndef _FBO_TEX
#define _FBO_TEX


#include "ofMain.h"

#include <iostream>


class ofxFBOTexture : public ofTexture {
public:

        void allocate(int w, int h, bool autoClear);

		//возвращает изображение
		void getImage( ofImage &image );


        void swapIn();
        void swapOut();

        void setupScreenForMe();
        void setupScreenForThem();

        void begin() {
				glMatrixMode(GL_PROJECTION);		//DENIS
				glPushMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

                swapIn();
                setupScreenForMe();
        }

        void end() {
                swapOut();
                setupScreenForThem();

				glMatrixMode(GL_MODELVIEW);			//DENIS
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
        }

        void clear();
        void clear(float r, float g, float b, float a);

        void bindAsTexture();


protected:
        bool            _isActive;
        GLuint      fbo;                                 // Our handle to the FBO
        GLuint      depthBuffer;                        // Our handle to the depth render buffer
        bool        autoClear;
        void        clean();
        float       clearColor[4];

};

#endif
