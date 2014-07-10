#include "filewriter.h"
#include<fstream>
#include<iostream>

FileWriter::FileWriter(char* path) {
	outFile.open(path, ios::out);
	if (!outFile) {
		cerr << "Can't open output file " << endl;
	}
}

FileWriter::~FileWriter() {
	outFile.close();
}
void FileWriter::writeFile(string line) {
	outFile << line << endl;
}
