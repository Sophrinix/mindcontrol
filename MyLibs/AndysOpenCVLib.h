/*
 * AndysOpenCVLib.h
 *
 *  Created on: Jul 13, 2009
 *      Author: Andy
 */

#ifndef ANDYSOPENCVLIB_H_
#define ANDYSOPENCVLIB_H_


/*
 *
 * This is a pair of cvPoints
 *
 */
typedef struct PairOfPtsStruct {
	CvPoint alpha;
	CvPoint beta;
} PairOfPoints;

typedef struct MemoryManagementStruct{
	CvMemStorage longTerm;
	CvMemStorage scratch;
	CvMemStorage meso;
} MemMan;

/***************************************************************
 * Working with the Frame Data Type
 ***************************************************************
 */


/*
 * Frame contains a binary representation and an IplImage representation of a frame.
 * This is useful in conjuncture with the TransformLib.h library that converts
 * from CCD to DLP space.
 *
 */
typedef struct FrameStruct{
	unsigned char * binary;
	IplImage* iplimg;
	CvSize size;
}Frame;



/*
 * Creates a frame. Allocates memory for frame structure.
 * Allocates memory for binary image.
 * Allocates memory for IplImage
 *
 */
Frame* CreateFrame(CvSize size);

/*
 * Destroys a frame.
 * Deallocates memory for binary image.
 * Deallocates memory for IplImage
 * Deallocates memory for Frame structure
 * Set's myFrame pointer to null.
 */
void DestroyFrame(Frame** myFrame);


/*
 * Load the Frame with a Binary Image
 *
 * copies the binary image into the frame's memory and
 * then copies the charArray to an iplImage and stores that
 * in the frame also
 *
 * NOTE: the binary image must have size myFrame->size
 *
 */
void LoadFrameWithBin(unsigned char* binsrc, Frame* myFrame);

/*
 * Load the Frame with a IplImage
 *
 * copies the IplImage into the frame's memory and
 * then converts it to a binary image and stores that
 * in the frame also
 *
 * NOTE: the image must have size myFrame->size
 *
 */
void LoadFrameWithImage(IplImage* imgsrc, Frame* myFrame);

/*
 * copies the 8 bit image data in src to the character array arr
 * arr must be preallocated and be src->width*src->height in size
 */
void copyIplImageToCharArray (const IplImage *src, unsigned char *arr);

/*
 * Creates a single channel 8bit IplImage and copies the 8 bit image data
 * in arr to the IplImage dest of size nsizex, nsizey.
 *
 */


int CopyCharArrayToIplImage(const unsigned char *arr, IplImage *dst,  int nsizex,
		int nsizey);


/*
 * Prints out some input, like whether the Intel Performance Primitives are installed
 *
 */
void DisplayOpenCVInstall();

/*
 *
 * Given a sequence of x,y CvPoints, this function returns a point containing the mean x and mean y values of the points.
 *
 */
CvPoint  GetMedianOfPoints(CvSeq* seq);



/*
 * Draws a square on "image" with radius "radius" centered on point "pt"
 * If any part of the square would fall outside of the image, than the function doesn't do anything.
 */
void SafeDrawSquare(IplImage** image, CvPoint* pt, int radius);

/*
 * Given a list of contours, this finds the contour with the longest perimiter and points ContourOfInterst to this contour.
 */
void LongestContour(CvSeq* contours, CvSeq** ContourOfInterest);


/*
 * Takes the cross product of two vectors representated in cartesian coordinates as CvPoint (x,y)
 *
 */
int PointCross(CvPoint* VecA, CvPoint* VecB);


/*
 * Takes the dot product of two vectors representated in cartesian coordinates as CvPoint (x,y)
 *
 */
int PointDot(CvPoint* VecA, CvPoint* VecB);


/*
 * Normalizes vectors representated in cartesian coordinates as CvPoint (x,y) and takes the cross product.
 *
 */
float NormPointCross(CvPoint* VecA, CvPoint* VecB);


/*
 * Normalizes two vectors representated in cartesian coordinates as CvPoint (x,y) and returns their dot ptorduct.
 *
 */
float NormPointDot(CvPoint* VecA, CvPoint* VecB);


/* Function Draws a Sequence of CvPoint's with little cirlces.
 * This function uses only a sequence and an image.
 *
 */

void DrawSequence(IplImage** image, CvSeq* Seq);


/*
 *
 * Resamples a boundary and stores it by omitting points. There is no interpolation.
 * This means that, if the points of an original were evenly spaced (a big if)
 * then if the number of original points is not an even multiple of Numsegments than the
 * distance between the last two points will be the same as all the other points.
 *
 * Basically because there is no interpolation, there is no way to ensure that all the points
 * are evenly spaced. I think this will good enough though. The other alternative would
 * be to use interpolation and that would require square roots and floats which I think
 * would take too long.
 *
 */
void resampleSeq(CvSeq* sequence,CvSeq* ResampledSeq, int Numsegments);

/*
 *
 * Returns the squared distance between two points
 *
 */
int sqDist(CvPoint pta, CvPoint ptb);



/*
 * Given two CvSeq containing CvPoint's this function returns the midpoint.
 *
 */
void FindCenterline(CvSeq* NBoundA,CvSeq* NBoundB,CvSeq* centerline);

/*
 * Given a point, and a boundary, this function returns the coordinates of the closest point on the boundary.
 */
CvPoint FindNearestPt(CvPoint CenterLinePt,CvSeq* Boundary);

/*
 * Given a point and a corresponding vector,  find the point along a contour that is perpendicular
 * to the specified point.
 */
CvPoint FindNormalPt(CvPoint* Centerline, CvPoint* CenterVec, CvSeq* Contour);




/*
 *
 * Marc's Functions
 *
 */

/*void SegmentSides (const CvSeq *contourA, const CvSeq *contourB, const CvSeq *centerline, CvSeq *segmentedA, CvSeq *segmentedB) {
 * all sequences are allocated sequences of CvPoint
 * const sequences are input
 * non const sequences are output and should be empty
 * contourA and contourB should be oriented so that they run from the nearest point to c0 (e.g. the head
 * to the nearest point to cN
 *
 * given a centerline c0 thru cN and contours on either side of the center line,
 * fills two new sequences with points according to these rules:
 *
 * a0/b0, aN/bN are the first (and last) points in A/B not equal to c0 (cN)
 * aj/bj is the intersection of the perpendicular to the centerline at point cj and the contour A,B
 *
 * aj is found by:  t(j) = c(j+1)-c(j-1);  x(k) = A(k)-c(j);  find k that minimizes abs(t(j)*x(k));  aj = A(k)
 *
 * finally, we enforce the rule that points in the segmented contour must have the same order as the original;
 * i.e. if index k > j, then segmentedA(k) comes later in contourA than segmentedA(j)
 *
 * MHG 9/16/09
 */
void SegmentSides (const CvSeq *contourA, const CvSeq *contourB, const CvSeq *centerline, CvSeq *segmentedA, CvSeq *segmentedB);



/*int FirstDoesNotMatch (CvPoint a, const CvSeq *b, int startInd, int dir)
 *
 *given a sequence of CvPoints b, starting at index startInd and proceeding in
 *direction dir (dir = +1 or -1, no error checking), finds the first point in b
 *that does not equal a, and returns its index
 *returns -1 in case of failure
 *
 ** MHG 9/16/09
 */
int FirstDoesNotMatch (CvPoint a, const CvSeq *b, int startInd, int dir);


/*int FindPerpPoint (CvPoint x, CvPoint t, const CvSeq *a, int startInd, int endInd) {
 *
 * a is sequence of CvPoint
 * finds the point in a that minimizes abs(dot (a(k)-x, t)) k in [startInd,endInd)
 * note that endInd is not included in search
 *
 * minimum point is the absolute minimum on the interval found by computing the quantity at every point
 *
 ** MHG 9/16/09
 */
int FindPerpPoint (CvPoint x, CvPoint t, const CvSeq *a, int startInd, int endInd);


/* void RemoveSequentialDuplicatePoints (CvSeq *seq)
 *
 * seq is a sequence of CvPoint
 * removes any duplicated points that occur in sequence;  e.g. (1,1), (1,1), (1,2) --> (1,1), (1,2)
 * but (1,1),(1,2),(1,1) --> (1,1),(1,2),(1,1)
 *
 *MHG 9/16/09
 */
void RemoveSequentialDuplicatePoints (CvSeq *seq);




//Marc's functions for convolution
void ConvolveInt1D (const int *src, int *dst, int length, int *kernel, int klength, int normfactor);

void ConvolveCvPtSeq (const CvSeq *src, CvSeq *dst, int *kernel, int klength, int normfactor);

void CreateGaussianKernel (double sigma, int **kernel, int *klength, int *normfactor);


CvSeq *smoothPtSequence (const CvSeq *src, double sigma, CvMemStorage *mem);



/**** Testing Functions ****/

/*
 * Check's to see if sequence exists
 * Exists=nonzero
 * False = 0
 */
bool cvSeqExists(CvSeq* MySeq);

/*
 * Check's to see if point exists
 * Exists=nonzero
 * False = 0
 */
bool cvPointExists(CvPoint* MyPt);

/*
 * Check's to see if int exists
 * Exists=nonzero
 * False = 0
 */
bool IntExists(int MyInt);


#endif /* ANDYSOPENCVLIB_H_ */


