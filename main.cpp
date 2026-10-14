#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

using namespace std;

void openFile(string fileName, fstream& file, ios::openmode mode);

void copyField(char* dst, const string& src){
    memcpy(dst, src.c_str(), src.size() + 1);
}

struct PhoneOwner{
    int id;
    char phoneNumber[13];
    char address[50];
    char fio[40];

    PhoneOwner(int id, const string phone, const string address, const string fio){
        this->id = id;
        copyField(this->phoneNumber, phone);
        copyField(this->address, address);
        copyField(this->fio, fio);
    }
};



void textToBinary(string textFileName, string binaryFileName){
    fstream textFile;

    openFile(textFileName, textFile, ios::in);
    ofstream binaryFile(binaryFileName, ios::binary);

    string line;

    while(getline(textFile, line)) {
        stringstream ss(line);
        string id, phone, address, fio;

        getline(ss, id, ';');
        getline(ss, phone, ';');
        getline(ss, address, ';');
        getline(ss, fio, ';');
    
        PhoneOwner rec(stoi(id), phone, address, fio);

        binaryFile.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    }
}


void openFile(string fileName, fstream& file, ios::openmode mode){

        while(true){
            cout << "Введите имя файла:" << endl;
            if (!getline(cin >> ws, fileName));

            if (!filesystem::exists(fileName)){
                cout << "Файл не существует, попробуйте еще раз" << endl;
                continue;
            }

            file.open(fileName, mode);

            if(!file.is_open()){
                cout << "Файл существует, но не удалось открыть" << endl;
                continue;
            }

            break;
    }
    cout << "Файл " << fileName << " открыт" << endl;

}


int main(){
    string textfileName;
    fstream file;

    openFile(textfileName, file, ios::in);

    string binaryFileName = "binary" + textfileName;

    textToBinary(textfileName, binaryFileName);

    return 0;
}


