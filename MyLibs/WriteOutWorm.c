/*
 * WriteOutWorm.c
 *
 *  Created on: Nov 3, 2009
 *      Author: Andy
 */
#include <stdio.h>
#include <time.h>

//OpenCV Headers
#include <cxcore.h>
#include <highgui.h>
#include <cv.h>

// Andy's Libraries

#include "AndysOpenCVLib.h"
#include "WormAnalysis.h"
#include "WriteOutWorm.h"



/*
 * Shortcut function to create a filename.
 * Basically just concatenates three strings together.
 *
 */
char* CreateFileName(const char* dir, const char* core, const char* suffix){
	/* Prepare String with Time Stamp */
	char timestamp[30];
	time_t curtime;
	struct tm *loctime;

	/* Get the current time. */
	curtime= time (NULL);
	strftime(timestamp,30,"%Y%m%d_%H%M",loctime);

	/* Allocate memory for filename */
	char* filename= (char*) malloc(strlen(timestamp)+strlen(dir)+strlen(core)+strlen(suffix)+1);

	/*Concatenate Strings */
	strcpy(filename,dir);
	strcat(filename,timestamp);
	strcat(filename,core);
	strcat(filename,suffix);

	free(&timestamp);
	return filename;
}

/*
 * Destroy's a filename and deallocates the poitner and such.
 */
void DestroyFilename(char** filename){
	free(*filename);
	*filename=NULL;
}


/*
 * Sets up the WriteToDisk  given the base of a filname.
 * Creates a WriteOut Object.
 *
 * e.g. if you want to create a files named myexperiment.yaml and myexperiment.mov
 * pass in the string "myexperiment"
 *
 */
WriteOut* SetUpWriteToDisk(const char* filename, CvMemStorage* Mem){
	/** Create new instance of WriteOut object **/
	WriteOut* DataWriter =(WriteOut*) malloc(sizeof(WriteOut));

	/** Append .yaml to the end of filename **/
	char* datafile = (char* ) malloc (strlen(filename)+1+strlen(".yaml"));
	strcpy(datafile,filename);
	strcat(datafile,".yaml");
	printf("About to initialize cvopenFileStorage(): %s\n", datafile);
	DataWriter->fs=cvOpenFileStorage(datafile,Mem,CV_STORAGE_WRITE);

	//free(&datafile);

	//cvWriteComment(Files->fs, "Worm experiment data made from mindcontrol project.\nleifer@fas.harvard.edu");
	printf("About to start cvSTartWriteStruc()\n");
	cvStartWriteStruct(DataWriter->fs,"Frames",CV_NODE_SEQ,NULL);
	return DataWriter;
}


/*
 * Writes Out information of one frame of the worm to a disk
 * in YAML format.
 *
 * Note the Worm object must have the following fields
 * Worm->frameNum
 * Worm->Segmented->Head
 * Worm->Segmented->Tail
 * Worm->Segmented->LeftBound
 * Worm->Segmented->RightBound
 * Worm->Segmented->Centerline
 */
int AppendWormFrameToDisk(WormAnalysisData* Worm, WormAnalysisParam* Params, WriteOut* DataWriter){

	CvFileStorage* fs=DataWriter->fs;

	cvStartWriteStruct(fs,NULL,CV_NODE_MAP,NULL);
		/** Frame Number Info **/
		if (IntExists(Worm->frameNum)) cvWriteInt(fs,"FrameNumber",Worm->frameNum);

		/** Segmentation Info **/
		if(cvPointExists(Worm->Segmented->Head)){
		cvStartWriteStruct(fs,"Head",CV_NODE_MAP,NULL);
			cvWriteInt(fs,"x",Worm->Segmented->Head->x);
			cvWriteInt(fs,"y",Worm->Segmented->Head->y);
		cvEndWriteStruct(fs);
		}

		if(cvPointExists(Worm->Segmented->Tail)){
		cvStartWriteStruct(fs,"Tail",CV_NODE_MAP,NULL);
			cvWriteInt(fs,"x",Worm->Segmented->Tail->x);
			cvWriteInt(fs,"y",Worm->Segmented->Tail->y);
		cvEndWriteStruct(fs);
		}

		if(cvSeqExists(Worm->Segmented->LeftBound)) cvWrite(fs,"BoundaryA",Worm->Segmented->LeftBound);
		if(cvSeqExists(Worm->Segmented->RightBound)) cvWrite(fs,"BoundaryB",Worm->Segmented->RightBound);
		if(cvSeqExists(Worm->Segmented->Centerline)) cvWrite(fs,"SegmentedCenterline",Worm->Segmented->Centerline);

		/** Illumination Information **/
		if (IntExists(Params->IllumSegCenter)) cvWriteInt(fs,"IllumSegCenter",Params->IllumSegCenter);
		if (IntExists(Params->IllumSegRadius)) cvWriteInt(fs,"IllumSegRadius",Params->IllumSegCenter);

		if (IntExists(Params->IllumLRC)) cvWriteInt(fs,"IllumNullLeftRightBoth",Params->IllumLRC);
	cvEndWriteStruct(fs);

	return 0;
}

/*
 * Finish writing to disk and close the file and such.
 * Destroys the Data Writer
 *
 */
int FinishWriteToDisk(WriteOut** DataWriter){
	CvFileStorage* fs=(*DataWriter)->fs;
	/** Finish writing this structure **/
	cvEndWriteStruct(fs);
	/** Close File Storage and Finish Writing Out to File **/
	cvReleaseFileStorage(&fs);
	free(*DataWriter);
	*DataWriter=NULL;
	return 0;
}
