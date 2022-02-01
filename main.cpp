#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <vector>
typedef unsigned char BYTE;
using namespace std;

/**
 * Read the binary file content into vector
 */
vector<char> readBinary(string filename) {
    // open the file:
    streampos fileSize;
    ifstream file(filename, ios::binary);

    // file size
    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    istreambuf_iterator<char> start(file), end;
    vector<char> buffer;
    buffer.reserve(fileSize);
    buffer.assign(start,end);
    file.close();
    return buffer;
}

/**
 * Check to see if file is binary type
 */
bool translateBinary(vector <char> preFiltered, bool sFlag, bool rFlag) {
    vector <string> filteredData;
    vector <string> reversedData;
    string runningLine;
    string temp;
    // go through and start changing ascii into their filtered form
    for (int i = 0; i < preFiltered.size(); i++) {
        temp = preFiltered[i];
        int charNum = static_cast<int>(static_cast<unsigned char>(preFiltered[i]));
        unsigned char modifier = (static_cast<unsigned char>(preFiltered[i]));
        if ((charNum > -1 && charNum < 32)){
            temp = (char) (static_cast<int> (modifier + 64));
            if (temp == "J"){
                filteredData.push_back("\n");
                runningLine += "\n";
                reversedData.push_back(runningLine);
                runningLine = "";
            }
            else if(temp == "I"){
                filteredData.push_back("\t");
                runningLine += "\t";
            }
            else {
                filteredData.push_back("^" + temp);
                runningLine += "^" + temp;
            }
        }
        else if (charNum == 127) {
            temp = (char) (static_cast<int>(modifier - 64));
            filteredData.push_back("^" + temp);
            runningLine += "^" + temp;
        }
        else if (charNum > 127 && charNum < 160) {
            // extra printable char at end is - 64
            temp = (char) (static_cast<int>(modifier - 64));
            filteredData.push_back("M-^" + temp);
            runningLine += "M-^" + temp;
        }
        else if (charNum == 255){
            temp = (char) (static_cast<int>(modifier - 128));
            filteredData.push_back("M-^" + temp);
            runningLine += "M-^" + temp;
        }
        else if ( charNum >= 160) {
            temp = (char) (static_cast<int>(static_cast<unsigned char>(preFiltered[i])) - 128);
            filteredData.push_back("M-" + temp);
            runningLine += "M-" + temp;
        }
        else {
            filteredData.push_back(temp);
            runningLine += temp;
        }
        if (i == preFiltered.size() - 1){
            reversedData.push_back("\n");
            reversedData.push_back(runningLine);
        }
    }

    if (sFlag){
        string peakChar;
        for (int i = 0; i < filteredData.size(); i++){
            if (filteredData[i] == "\n" && (i + 1 < filteredData.size()) ){
                peakChar = filteredData[i + 1];
                if (peakChar == "\n"){
                    for(int x = i + 1; x < filteredData.size(); ++x){
                        if(filteredData[x] != "\n"){
                            // delete the subsequent \n's
                            filteredData.erase(filteredData.begin() + i + 1, filteredData.begin() + x - 1);
                            break;
                        }
                    }
                }
            }
        }
    }

    if(rFlag){
        for (int i = reversedData.size() - 1; i >= 0; i--){
            cout << reversedData[i];
        }
    }

    else{
        for (int i = 0; i < filteredData.size(); i++){
            cout << filteredData[i];
        }
    }

    return false;
}

int main(int argc, char *argv[])
{
    int offset = 0;
    // make sure there is input going in
    if (argc < 2) {
    cout << argc << endl;
    cout << argv[1] << endl;
    }

    // create flags
    bool vFlag = false;
    bool eFlag = false;
    bool nFlag= false;
    bool sFlag = false;
    bool rFlag = false;

    // parse arguments to set flag validity
    for (int i = 1; i < argc; ++i){
        if (argv[i][0] == '-'){
            offset = i + 1;
            string argumentFlag = argv[i];
            if(argumentFlag.find('E') != string::npos){
                eFlag = true;
            }
            if(argumentFlag.find("n") != string::npos){
                nFlag = true;
            }
            if(argumentFlag.find("v") != string::npos){
                vFlag = true;
            }
            if(argumentFlag.find("s") != string::npos){
                sFlag = true;
            }
            if(argumentFlag.find("r") != string::npos){
                rFlag = true;
            }
        }
    }
    // find position of files and modify offset
    int fileCount = 1;
    if (offset != 0){
        fileCount = offset;
    }
    // iterate through all possible files fed
    while (fileCount != argc){
    string fileInput = argv[fileCount];
    fileCount++;
    // if binary flags are enabled, use binary
    if (vFlag || eFlag) {
        translateBinary(readBinary(fileInput), sFlag, rFlag);
    }
    else {
        int lineCount = 1;
        string nFlagModifier = "";
        ifstream myFile (fileInput);
        string line;
        if (myFile.is_open()){
            // feed line from file into line variable
            while (getline(myFile, line)) {
                if (nFlag){
                    nFlagModifier = "\t" + to_string(lineCount) + "  ";
                    }
                cout << nFlagModifier + line;
                lineCount++;
            }
            myFile.close();
        }
     else {
         cout << "COULDNT FIND FILE";
          }
         }
    }
    return 0;
}