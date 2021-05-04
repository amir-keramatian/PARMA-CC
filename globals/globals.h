#ifndef GLOBALS_FILE_H_
#define GLOBALS_FILE_H_

#include <vector>
#include <iostream>


extern char inputFile[];
extern char outputDir[];

extern bool aura;
extern double confidenceStep;
extern int MinPts;
extern double EPSILON;

extern std::vector< std::string > inputFiles;

extern int numberOfThreads;
extern int numberOfSplits;

extern int localClusteringAlg;

extern bool ellipsoidalModeling;

void parseTheArguments_MADC(int, char* argv[]);
void parseTheArguments_PARMACC(int, char* argv[]);


#endif
