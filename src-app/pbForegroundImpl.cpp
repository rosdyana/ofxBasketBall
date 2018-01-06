#include "pbForegroundImpl.h"


//------------------------------------------------------------------------------
void init_codebook(codeBook &c)
{
    c.cb = 0;
    c.numEntries = 0;
    c.t = 0;
}

void free_codebook(codeBook &c)
{
    for (int i = 0; i < c.numEntries; i++) {
        delete c.cb[i];
    }
    if (c.cb)	{
        delete[] c.cb;
        c.cb = 0;
    }
    c.numEntries = 0;
    c.t = 0;
}

//------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////
// int update_codebook(uchar *p, codeBook &c, unsigned cbBounds)
// Updates the codebook entry with a new data point
//
// p Pointer to a YUV pixel
// c Codebook for this pixel
// cbBounds Learning bounds for codebook (Rule of thumb: 10)
// numChannels Number of color channels we’re learning
//
// NOTES:
// cvBounds must be of length equal to numChannels
//
// RETURN
// codebook index
//
int update_codebook(
    uchar *p,
    codeBook &c,
    unsigned *cbBounds,
    int numChannels,
    float time
)
{
    c.t = time;
    int high[3], low[3];
    for (int n = 0; n < numChannels; n++) {
        high[n] = *(p + n) + *(cbBounds + n);
        if (high[n] > 255) {
            high[n] = 255;
        }
        low[n] = *(p + n) - *(cbBounds + n);
        if (low[n] < 0) {
            low[n] = 0;
        }
    }
    int matchChannel;
    // SEE IF THIS FITS AN EXISTING CODEWORD
    //
    int i;
    for (i = 0; i < c.numEntries; i++) {
        matchChannel = 0;
        for (int n = 0; n < numChannels; n++) {
            if ((c.cb[i]->learnLow[n] <= *(p + n)) &&
                    //Found an entry for this channel
                    (*(p + n) <= c.cb[i]->learnHigh[n])) {
                matchChannel++;
            }
        }
        if (matchChannel == numChannels) { //If an entry was found
            c.cb[i]->t_last_update = c.t;
            //adjust this codeword for the first channel
            for (int n = 0; n < numChannels; n++) {
                if (c.cb[i]->max[n] < * (p + n)) {
                    c.cb[i]->max[n] = *(p + n);
                } else if (c.cb[i]->min[n] > *(p + n)) {
                    c.cb[i]->min[n] = *(p + n);
                }
            }
            break;
        }
    }
    // OVERHEAD TO TRACK POTENTIAL STALE ENTRIES
    //
    for (int s = 0; s < c.numEntries; s++) {
        // Track which codebook entries are going stale:
        //
        int negRun = c.t - c.cb[s]->t_last_update;
        if (c.cb[s]->stale < negRun) {
            c.cb[s]->stale = negRun;
        }
    }

    // ENTER A NEW CODEWORD IF NEEDED
    //
    if (i == c.numEntries) { //if no existing codeword found, make one
        code_element **foo = new code_element* [c.numEntries + 1];
        for (int ii = 0; ii < c.numEntries; ii++) {
            foo[ii] = c.cb[ii];
        }
        foo[c.numEntries] = new code_element;
        if (c.numEntries) {
            delete [] c.cb;    //так там указатели... память похоже не чистится.
        }
        c.cb = foo;
        for (int n = 0; n < numChannels; n++) {
            c.cb[c.numEntries]->learnHigh[n] = high[n];
            c.cb[c.numEntries]->learnLow[n] = low[n];
            c.cb[c.numEntries]->max[n] = *(p + n);
            c.cb[c.numEntries]->min[n] = *(p + n);
        }
        c.cb[c.numEntries]->t_last_update = c.t;
        c.cb[c.numEntries]->stale = 0;
        c.numEntries += 1;
    }
    // SLOWLY ADJUST LEARNING BOUNDS
    //
    for (int n = 0; n < numChannels; n++) {
        if (c.cb[i]->learnHigh[n] < high[n]) {
            c.cb[i]->learnHigh[n] += 1;
        }
        if (c.cb[i]->learnLow[n] > low[n]) {
            c.cb[i]->learnLow[n] -= 1;
        }
    }
    return (i);
}

//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////
//int clear_stale_entries(codeBook &c)
// During learning, after you’ve learned for some period of time,
// periodically call this to clear out stale codebook entries
//
// c Codebook to clean up
//
// Return
// number of entries cleared
//
int clear_stale_entries(codeBook &c, float staleDuration)
{
    float staleThresh = staleDuration;
    int *keep = new int [c.numEntries];
    int keepCnt = 0;
    // SEE WHICH CODEBOOK ENTRIES ARE TOO STALE
    //
    for (int i = 0; i < c.numEntries; i++) {
        if (c.cb[i]->stale > staleThresh) {
            keep[i] = 0;    //Mark for destruction
        } else {
            keep[i] = 1; //Mark to keep
            keepCnt += 1;
        }
    }
    // KEEP ONLY THE GOOD
    //
    c.t = 0; //Full reset on stale tracking
    code_element **foo = new code_element* [keepCnt];
    int k = 0;
    for (int ii = 0; ii < c.numEntries; ii++) {
        if (keep[ii]) {
            foo[k] = c.cb[ii];
            //We have to refresh these entries for next clearStale
            foo[k]->t_last_update = 0;
            k++;
        }
    }
    // CLEAN UP
    //
    delete [] keep;
    delete [] c.cb;
    c.cb = foo;
    int numCleared = c.numEntries - keepCnt;
    c.numEntries = keepCnt;
    return (numCleared);
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////
// uchar background_diff( uchar *p, codeBook &c,
// int minMod, int maxMod)
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
uchar background_diff(
    uchar *p,
    codeBook &c,
    int numChannels,
    int *minMod,
    int *maxMod
)
{
    int matchChannel;
    // SEE IF THIS FITS AN EXISTING CODEWORD
    //
    int i;
    for (i = 0; i < c.numEntries; i++) {
        matchChannel = 0;
        for (int n = 0; n < numChannels; n++) {
            if ((c.cb[i]->min[n] - minMod[n] <= *(p + n)) &&
                    (*(p + n) <= c.cb[i]->max[n] + maxMod[n])) {
                matchChannel++; //Found an entry for this channel
            } else {
                break;
            }
        }
        if (matchChannel == numChannels) {
            break; //Found an entry that matched all channels
        }
    }
    if (i >= c.numEntries) {
        return (255);
    }
    return (0);
}

//------------------------------------------------------------------------------





