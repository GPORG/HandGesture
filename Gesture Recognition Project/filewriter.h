#ifndef FILEWRITER_H
#define FILEWRITER_H
#include<string>
#include <iostream>
#include <fstream>
using namespace std;
class FileWriter {
public:
	FileWriter(char* path);
	// Constructor
	~FileWriter();
	// Destructor
	ofstream outFile;
	void writeFile(string line);
};

#endif // FILEWRITER_H
