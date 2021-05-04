#include "globals.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool aura = false;
double confidenceStep = 1.6;
int MinPts = 10;
double EPSILON = 0.35;
char inputFile[500] = "";
char outputDir[500] = "results";

std::vector< std::string > inputFiles;

int numberOfThreads = 10;
int numberOfSplits  = 10;

int localClusteringAlg = 1; //EUCLIDEAN=1,DBSCAN=2

bool ellipsoidalModeling = false;

void parseTheArguments_MADC(int argc, char* argv[]){
  int opt;
  
  while ( (opt=getopt(argc, argv, "a:c:m:e:f:")) != -1  ){

    switch(opt){

    case 'a':
      aura = atoi(optarg);
      break;
    case 'c':
      confidenceStep = atof(optarg);
      break;
    case 'm':
      MinPts = atoi(optarg);
      break;
    case 'e':
      EPSILON = atof(optarg);
      break;
    case 'f':
      strcpy(inputFile, optarg);
      inputFiles.push_back(std::string(inputFile));
      break;
    default:
      fprintf(stderr,
	      "Usage: %s [-a aura] [-c confidenceStep] [-m MinPts] [-e Epsilon] [-f inputFile]\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  printf("aura: %s, confidecenStep: %f, MinPts: %d, Epsilon: %f\n",
	 aura ? "true":"false",
	 confidenceStep,
	 MinPts,
	 EPSILON); 

  
  printf("InputFiles: ");
  for (int i = 0; i < inputFiles.size(); i++){
    printf("[%s] ", inputFiles[i].c_str());
  }
  printf("\n");

}

void parseTheArguments_PARMACC(int argc, char* argv[]){
  int opt;
  
  while ( (opt=getopt(argc, argv, "a:c:m:e:f:K:S:O:")) != -1  ){

    switch(opt){

    case 'a':
      aura = atoi(optarg);
      break;
    case 'c':
      confidenceStep = atof(optarg);
      break;
    case 'm':
      MinPts = atoi(optarg);
      break;
    case 'e':
      EPSILON = atof(optarg);
      break;
    case 'f':
      strcpy(inputFile, optarg);
      //inputFiles.push_back(std::string(inputFile));
      break;
    case 'K':
      numberOfThreads = atoi(optarg);
      break;
    case 'S':
      numberOfSplits = atoi(optarg);
      break;
    case 'O':
      strcpy(outputDir, optarg);
      break;
      
    default:
      fprintf(stderr,
	      "Usage: %s [-a aura] [-c confidenceStep] [-m MinPts] [-e Epsilon] [-f inputFile] [-K numberOfThreads] [-S numberOfSplits]\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  printf("aura: %s, confidecenStep: %f, MinPts: %d, Epsilon: %f, inputFile: %s, numberOfThreads: %d, numberOfSplits: %d\n",
	 aura ? "true":"false",
	 confidenceStep,
	 MinPts,
	 EPSILON,
	 inputFile,
	 numberOfThreads,
	 numberOfSplits); 

}

