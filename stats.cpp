
#include "stats.h"

stats::stats(PNG & im){
    sumHueX.clear();
    sumHueY.clear();
    sumSat.clear();
    sumLum.clear();

    sumHueX.resize(im.width()+1, vector<double>( im.height()+1 , 0 ) );
    sumHueY.resize(im.width()+1, vector<double>( im.height()+1 , 0 ) );
    sumSat.resize(im.width()+1, vector<double>( im.height()+1 , 0 ) );
    sumLum.resize(im.width()+1, vector<double>( im.height()+1 , 0 ) );

    hist.clear();
    hist.resize(im.width()+1,vector<vector<int> >(im.height()+1,vector<int>(36, 0)));

    //std::cout << "\tfinished resizing" << '\n';

    for (size_t x = 1; x <= im.width(); x++) {
        for (size_t y = 1; y <= im.height(); y++) {
            vector<int> thisHist = hist[x][y];
            vector<int> aboveHist;
            vector<int> leftHist;
            vector<int> aboveLeftHist;
            HSLAPixel * thisPixel = im.getPixel(x-1, y-1);

            double thisHue = thisPixel->h;
            int thisHistVal = ((thisHue == 360) ? 0 : thisHue/10);

            sumHueX[x][y] = cos(thisHue * PI/180)
            + sumHueX[x][y-1]
            + sumHueX[x-1][y]
            - sumHueX[x-1][y-1];
            sumHueY[x][y] = sin(thisHue * PI/180)
            + sumHueY[x][y-1]
            + sumHueY[x-1][y]
            - sumHueY[x-1][y-1];

            sumSat[x][y] = thisPixel->s
            + sumSat[x][y-1]
            + sumSat[x-1][y]
            - sumSat[x-1][y-1];
            sumLum[x][y] = thisPixel->l
            + sumLum[x][y-1]
            + sumLum[x-1][y]
            - sumLum[x-1][y-1];

            aboveHist = hist[x][y-1];
            leftHist = hist[x-1][y];
            aboveLeftHist = hist[x-1][y-1];

            //above + left - aboveLeft
            for (size_t k = 0; k < 36; k++) {
                hist[x][y][k] = aboveHist[k] + leftHist[k] - aboveLeftHist[k];
            }
            //add this own pixel's hue to hist
            hist[x][y][thisHistVal]++;
            //std::cout << "hist update" << '\n'; //IO makes code slow :(
            // }
        } //end for y
    } //end for x
} //end const

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
    long width = lr.first - ul.first + 1;
    long height = lr.second - ul.second + 1;

    return width * height;
}

/**
this can go fuck itself
*/
HSLAPixel stats::getAvg(pair<int, int> ul, pair<int, int> lr)
{
    /**
    End Goal: return a pixel where its h, l, s, a components are the corresponding
    average values in a rectangle.

    NEED if statements for each of the cases. So that you can avoid segfaults
    */

    ul = make_pair(ul.first+1, ul.second+1);
    lr = make_pair(lr.first+1, lr.second+1);
    //initialize a local pixel variable for return
    HSLAPixel ansPixel;

    // average values for alpha

    ansPixel.a = 1.0;

    //----------------------------------------------------------------------------

    double ansSat = 0;
    double ansLum = 0;
    double sumX = 0;
    double sumY = 0;

    //when ul is at (0,0)
    // if (ul.first == 0 && ul.second == 0) { //ul.first == 0 && lr.second == 0
    //     //the average values should be the HSLAPixel values itself
    //     ansSat = sumSat[lr.first][lr.second];
    //     ansLum = sumLum[lr.first][lr.second];
    //     sumX = sumHueX[lr.first][lr.second];
    //     sumY = sumHueY[lr.first][lr.second];
    // }
    //
    // //case 2
    // // UL is top row but not at x = 0
    // else if (ul.first != 0 && ul.second == 0) {
    //     ansSat = sumSat[lr.first][lr.second] - sumSat[ul.first - 1][lr.second]; //first error "invalid read"
    //     ansLum = sumLum[lr.first][lr.second] - sumLum[ul.first - 1][lr.second];
    //     sumX = sumHueX[lr.first][lr.second] - sumHueX[ul.first - 1][lr.second];
    //     sumY = sumHueY[lr.first][lr.second] - sumHueY[ul.first - 1][lr.second];
    // }
    //
    // // case 3
    // // UL is leftmost but not y = 0
    // else if (ul.first == 0 && ul.second != 0) {
    //     ansSat = sumSat[lr.first][lr.second] - sumSat[lr.first][ul.second - 1];
    //     ansLum = sumLum[lr.first][lr.second] - sumLum[lr.first][ul.second - 1];
    //     sumX = sumHueX[lr.first][lr.second] - sumHueX[lr.first][ul.second - 1];
    //     sumY = sumHueY[lr.first][lr.second] - sumHueY[lr.first][ul.second - 1];
    // }

    // case 4
    // UL is not (0,0), (0,y), (x,0)
    // else {
    ansSat = sumSat[lr.first][lr.second] //bottom right
    - sumSat[lr.first][ul.second - 1] //minus above
    - sumSat[ul.first - 1][lr.second] //minus left
    + sumSat[ul.first - 1][ul.second - 1]; //add aboveLeft

    ansLum = sumLum[lr.first][lr.second] //bottom right
    - sumLum[lr.first][ul.second - 1] //minus above
    - sumLum[ul.first - 1][lr.second] //minus left
    + sumLum[ul.first - 1][ul.second - 1]; //add aboveLeft

    sumX = sumHueX[lr.first][lr.second] //bottom right
    - sumHueX[lr.first][ul.second - 1] //minus above
    - sumHueX[ul.first - 1][lr.second] //minus left
    + sumHueX[ul.first - 1][ul.second - 1]; //add aboveLeft

    sumY = sumHueY[lr.first][lr.second] //bottom right
    - sumHueY[lr.first][ul.second - 1] //minus above
    - sumHueY[ul.first - 1][lr.second] //minus left
    + sumHueY[ul.first - 1][ul.second - 1]; //add aboveLeft
    // }

    //---------------------

    // calculating the average of the saturation over a rectangle area

    ansPixel.s = ansSat / rectArea(ul, lr);

    // calculating the average of the luminence over a rectangle area

    ansPixel.l = ansLum / rectArea(ul, lr);

    // calculating the average of the hue over a rectangle area

    double theta = atan2(sumY, sumX) * 180 / PI; //compute theta: the arctan of sumX and sumY

    //converting theta into its appropriate value based on the "coordinates" of X and Y

    //when theta is a negative value <- 3rd and 4th quadrants
    if (theta < 0) theta += 360;

    //finally store the average hue
    ansPixel.h = theta;


    // std::cout << "lr.first/second  "<<lr.first<<"\t"<<lr.second<< '\n';
    // std::cout << "ul.first/second  "<<ul.first<<"\t"<<ul.second<< '\n';
    // std::cout << "Area: "<< rectArea(ul, lr) << '\n';
    // cout<<ansPixel.h<<", "<<
    //     ansPixel.s<<", "<<
    //     ansPixel.l<<", "<<
    //     ansPixel.a<<"\n";


    return ansPixel;
}
double stats::entropy(pair<int,int> ul, pair<int,int> lr){

    vector<int> distn;
    distn.resize(36, 0);
    ul = make_pair(ul.first+1, ul.second+1);
    lr = make_pair(lr.first+1, lr.second+1);

    /* using private member hist, assemble the distribution over the
    *  given rectangle defined by points ul, and lr into variable distn.
    *  You will use distn to compute the entropy over the rectangle.
    *  if any bin in the distn has frequency 0, then do not add that
    *  term to the entropy total. see .h file for more details.
    */

    /* my code includes the following lines:
    if (distn[i] > 0 )
    entropy += ((double) distn[i]/(double) area)
    * log2((double) distn[i]/(double) area);
    */
    // std::cout << "lr.first/second  "<<lr.first<<"\t"<<lr.second<< '\n';
    // std::cout << "ul.first/second  "<<ul.first<<"\t"<<ul.second<< '\n';

    for (size_t k = 0; k < 36; k++) {
        if (ul.first == 0 && ul.second == 0) { //ul is top left 0,0
            distn[k] = hist[lr.first][lr.second][k];
        } else if (ul.second == 0) { //ul on top row
            distn[k] = hist[lr.first][lr.second][k]
            -hist[ul.first-1][lr.second][k];
        } else if (ul.first == 0) { //ul on leftmost column
            distn[k] = hist[lr.first][lr.second][k]
            -hist[lr.first][ul.second-1][k];
        } else { //upper left is not on an edge
            distn[k] = hist[lr.first][lr.second][k] //bottom right
            - hist[lr.first][ul.second - 1][k] //above --ul.first ->ul.second
            - hist[ul.first - 1][lr.second][k] //left
            + hist[ul.first - 1][ul.second - 1][k]; //add aboveLeft
        }
        // cout<<distn[k]<<", ";
    }
    // cout<<endl;
    long area = rectArea(ul, lr);

    // int totalFreq = 0;
    double entropy = 0;
    // for (size_t k = 0; k < 36; k++) {
    //     totalFreq += distn[k];
    // }
    // cout<<"calc entropy:\n";
    for (size_t i = 0; i < 36; i++) {
        if (distn[i] > 0 ) {
            entropy += ((double) distn[i]/(double) area) //ASK ABOUT THIS
            * log2((double) distn[i]/(double) area);
            // cout<<i<<" : "<<entropy<<", ";
        }
    }
    // cout<<endl;

    //cout<<"Entropy : "<<entropy<<"\n";
    return  -1.0 * entropy;
}
