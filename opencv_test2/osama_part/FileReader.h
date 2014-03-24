/*
 * File:   FileReader.h
 * Author: Mohamed
 *
 * Created on April 7, 2013, 5:40 PM
 */

#ifndef FILEREADER_H
#define	FILEREADER_H
#include <fstream>
#include <iostream>

using namespace std;
class FileReader {
public:
    FileReader(char*name);
    virtual ~FileReader();
    string readFile();
    ifstream inFile;

};

#endif	/* FILEREADER_H */

