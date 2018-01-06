#pragma once

#include <cv.h>

//using namespace std;
//using namespace cv;

#define pbForeground_CHANNELS ( 3 )

typedef struct ce {
    uchar learnHigh[pbForeground_CHANNELS]; //High side threshold for learning
    uchar learnLow[pbForeground_CHANNELS]; //Low side threshold for learning
    uchar max[pbForeground_CHANNELS]; //High side of box boundary
    uchar min[pbForeground_CHANNELS]; //Low side of box boundary
    float t_last_update; //Allow us to kill stale entries
    float stale; //max negative run (longest period of inactivity)
} code_element;


typedef struct code_book {
    code_element **cb;
    int numEntries;
    float t; //count every access
} codeBook;


void init_codebook(codeBook &c);
//начальна€ очистка при создании

void free_codebook(codeBook &c);


int update_codebook(
    uchar *p,
    codeBook &c,
    unsigned *cbBounds,
    int numChannels,
    float time
);
// Updates the codebook entry with a new data point
//
// p Pointer to a YUV pixel
// c Codebook for this pixel
// cbBounds Learning bounds for codebook (Rule of thumb: 10)
// numChannels Number of color channels weТre learning
//
// NOTES:
// cvBounds must be of length equal to numChannels
//
// RETURN
// codebook index


int clear_stale_entries(codeBook &c, float staleDuration);
// During learning, after youТve learned for some period of time,
// periodically call this to clear out stale codebook entries
//
// c Codebook to clean up
//
// Return
// number of entries cleared
//

uchar background_diff(
    uchar *p,
    codeBook &c,
    int numChannels,
    int *minMod,
    int *maxMod
);

// Given a pixel and a codebook, determine if the pixel is
// covered by the codebook
//
// p Pixel pointer (YUV interleaved)
// c Codebook reference
// numChannels Number of channels we are testing
// maxMod Add this (possibly negative) number onto
// max level when determining if new pixel is foreground
// minMod Subract this (possibly negative) number from
// min level when determining if new pixel is foreground
//
// NOTES:
// minMod and maxMod must have length numChannels,
// e.g. 3 channels => minMod[3], maxMod[3]. There is one min and
// one max threshold per channel.
//
// Return
// 0 => background, 255 => foreground
//