#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>

using namespace std;

void openFile(string& fileName, fstream& file, ios::openmode mode);

void copyField(char* dst, const string& src){
    memcpy(dst, src.c_str(), src.size() + 1);
}

struct PhoneOwner{
    int id;
    char phoneNumber[13];
    char address[50];
    char fio[40];

    PhoneOwner(){
        memset(this, 0, sizeof(*this));
    }

    PhoneOwner(int id, const string phone, const string address, const string fio){
        memset(this, 0, sizeof(*this));
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

    textFile.close();
    binaryFile.close();
}

void binaryToText(string textFileName, string binaryFileName){
    fstream binaryFile;

    openFile(binaryFileName, binaryFile, ios::in | ios::binary);
    fstream textFile(textFileName, ios::out);

    PhoneOwner p;

    while(binaryFile.read(reinterpret_cast<char*>(&p), sizeof(p) )) {
        textFile << p.id << ';' << p.phoneNumber << ';' << p.address << ';' << p.fio << '\n';
    }

    binaryFile.close();
    textFile.close();

}


void openFile(string& fileName, fstream& file, ios::openmode mode){

        while(true){
            cout << "Введите имя файла:" << endl;
            cin >> fileName;

            if (!std::filesystem::exists(fileName)){
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


void printAllRecords(string& binaryFileName) {
    fstream binaryFile;
    openFile(binaryFileName, binaryFile, ios::binary | ios::in);
    if (!binaryFile) {
        cerr << "Не удалось открыть " << binaryFileName << endl;
        return;
    }

    PhoneOwner p;
    while (binaryFile.read(reinterpret_cast<char*>(&p), sizeof(p))) {
        cout << p.id << " | " << p.phoneNumber << " | " << p.address << " | " << p.fio << '\n';
    }

    binaryFile.close();
}


void findByPosition(string& binaryFileName, const int position){
    fstream binaryFile;
    openFile(binaryFileName, binaryFile, ios::in | ios::binary);
    PhoneOwner p;

    binaryFile.seekg((position - 1) * sizeof(PhoneOwner)); 
    binaryFile.read(reinterpret_cast<char*>(&p), sizeof(p));
    cout << p.id << " | " << p.phoneNumber << " | " << p.address << " | " << p.fio << '\n';

    binaryFile.close();
}

void deleteByPosition(string& binaryFileName, const int position){
    fstream file;
    openFile(binaryFileName, file, ios::in | ios::out | ios::binary);

    file.seekg(0, ios::end);
    long count = file.tellg() / sizeof(PhoneOwner);
    if (count == 0) return;
    

    PhoneOwner p;

    file.seekg((count - 1) * sizeof(p));
    file.read(reinterpret_cast<char*>(&p), sizeof(p));

    file.seekp((position - 1) * sizeof(p));
    file.write(reinterpret_cast<char*>(&p), sizeof(p));

    file.close();
    filesystem::resize_file(binaryFileName, (count - 1) * sizeof(PhoneOwner));

    file.close();
}

void filterPhoneOwner(string& textFileName, string& binaryFileName, const string& prefix){
    fstream binaryFile;
    fstream textFile;

    openFile(binaryFileName, binaryFile, ios::binary | ios::in);
    openFile(textFileName, textFile, ios::out);

    PhoneOwner p;
    while(binaryFile.read(reinterpret_cast<char*>(&p), sizeof(p))){
        if(strncmp(p.phoneNumber + 2, prefix.c_str(), 3) == 0){
            textFile << p.id << " | " << p.phoneNumber << " | " << p.address << " | " << p.fio << '\n';
        }
    }

    textFile.close();
    binaryFile.close();
}


void deletePhoneOwner(string& binaryFileName, const char& prefix){
    fstream binaryFile(binaryFileName, ios::binary | ios::in);
    fstream textFile;
    string textFileName = "text" + binaryFileName;
    openFile(textFileName, textFile, ios::out);

    PhoneOwner p;

    while(binaryFile.read(reinterpret_cast<char*>(&p), sizeof(p))){
        if(strncmp(p.phoneNumber + 2, &prefix, 1) != 0){
            textFile << p.id << " | " << p.phoneNumber << " | " << p.address << " | " << p.fio << '\n';
        }
    }

    filesystem::remove("binary");
    filesystem::rename("textbinary", "binary");

    binaryFile.close();
    textFile.close();
}


int main(){

    std::cout << "Выберите действие для выполения(цифра от 1 до 6): " << endl;
    int action;
    cin >> action;

    switch(action){
        case 1 : {
            string binaryFileName;
            fstream binaryFile;
            openFile(binaryFileName, binaryFile, ios::in);

            string textFileName;

            textToBinary(textFileName, binaryFileName);
        }
        case 2 : {
            string textFileName;
            fstream textFile;
            openFile(textFileName, textFile, ios::in);

            string binaryFileName;

            textToBinary(textFileName, binaryFileName);
        }
        case 3 : {
            string binaryFileName;
            printAllRecords(binaryFileName);
        }

        case 4 : {
            string binaryFileName;

            cout << "Введите позицию искомой записи: " << endl;
            int position;
            cin >> position;

            findByPosition(binaryFileName, position);
        }

        case 5 : {
            string binaryFileName;

            cout << "Введите позицию для удаления записи: " << std::endl;
            int position;
            cin >> position;
            deleteByPosition(binaryFileName, position);
        }

        case 6 : {

            string textFileName;

            string binaryFileName;

            cout << "Введите строку для фильтрации: " << std::endl;
            string filterString;
            cin >> filterString;

            filterPhoneOwner(textFileName, binaryFileName, filterString);
        }
        
        case 7 : {
            cout << "Введите префикс для фильтраци(одну цифру)";
            char prefix;
            cin >> prefix;

            string textFileName;
            string binaryFileName;
            fstream file;

            openFile(binaryFileName, file, ios::in | ios::binary);

            deletePhoneOwner(binaryFileName, prefix);
        }
    }

    return 0;
}