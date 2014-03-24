/*
 * File:   FileReader.cpp
 * Author: Mohamed
 *
 * Created on April 7, 2013, 5:40 PM
 */

#include "FileReader.h"

FileReader::FileReader(char* name) {
    inFile.open(name,ios::in);
    if(!inFile){
    cerr <<"can't open nput file"<<endl;
    }
}

string FileReader::readFile(){
    string line;
    if (!inFile.eof()){
        getline(inFile,line,'\n');
    }
    return line;
}

FileReader::~FileReader() {
    inFile.close();
}

