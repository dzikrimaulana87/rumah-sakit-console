#include <iostream> 
#include <fstream>
#include <string>
#include <chrono>
#include <sstream>
using namespace std;

struct PatientData { //tampung data pasien
    int umur, bb, tb, dokter,obat,treat = 0,dayranap = 0;
    string nama, alergi, hispenyakit, alamat, tensi, id, goldar,trajal = "Belum ada",kelamin;
    PatientData* next;
    bool ranap = false;
};

struct Node { //penentuan potongan asuransi
    string question;
    Node* yes;
    Node* no;
    int discount;
};

#define maks 20

int graph[maks][maks] = { // jalur terpendek ambulance
    {0,2,0,0,0,0,0,0,1,4,0,0,0,0,0,0,0,0,0,0},
    {2,0,3,0,0,0,0,4,2,2,0,0,0,0,0,0,0,0,0,0},
    {0,3,0,1,0,0,4,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,2,6,1,3,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,2,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,6,2,0,3,0,0,0,0,0,0,5,3,0,0,0,0,0},
    {0,0,4,1,4,3,0,1,0,0,0,0,2,2,3,0,0,0,0,0},
    {0,4,1,3,0,0,1,0,2,0,0,4,3,2,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,2,0,4,4,4,3,0,0,0,0,0,0,0},
    {4,2,0,0,0,0,0,0,4,0,3,3,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,4,3,0,1,0,0,0,0,0,0,2,2},
    {0,0,0,0,0,0,0,4,4,3,1,0,2,0,0,0,0,4,2,1},
    {0,0,0,0,0,0,2,3,3,0,0,2,0,1,0,0,2,2,1,0},
    {0,0,0,0,0,5,2,2,0,0,0,0,1,0,4,1,3,3,0,0},
    {0,0,0,0,0,3,3,0,0,0,0,0,0,4,0,1,4,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,4,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,4,0,3,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,2,1,0,0,0,0,4,0,2},
    {0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,2,0}
};

#define room 1 //antrean tindakan kesehatan
struct queueroom{   
    int front[4] = {-1,-1,-1,-1}, rear[4] = {-1,-1,-1,-1};
    string medicroom[4][room];   
}medroom;


struct dokter {// harga tindakan medis
    string nama[2][10] = {
        {"dr. Michael Subroto, S.Ked",
        "dr. Andreas Gunawan, Sp.THT",
        "dr. Rahmat Saputra, Sp.PD",
        "dr. Sanusi Siregar, Sp.B ",
        "dr. Dzikri Maulana, Sp.KK",
        "dr. Fatimah Nurina, Sp.JP",
        "dr. Rangga Pratama, Sp.A",
        "dr. Muhammad Harun, Sp.N",
        "dr. Jesicca Alexia, Sp.OG",
        "dr. Wibowo Santoso, Sp.M"},
        {"umum", "tht", "dalam", "bedah", "kulit", "jantung", "Anak", "Syaraf", "Kandungan", "Mata"}
    };
    int harga[10] = { 50000, 300000, 270000, 500000, 400000, 450000, 300000, 400000, 350000, 250000 };
    string treat [4] = {"operasi","rontgen", "uji lab", "usg"};
    int pricetreat [4] = {15000000,250000,150000,300000};
} info;


void masuk();//pasien masuk
void inputData(PatientData** head);//input data
void update(PatientData** head, const string& id);//update data


//baca tulis file
void readFromFile(const string& id, int konfirmasi);
void writeToFile(const PatientData& patient, int konfirmasi);

void carilinked(const PatientData* head, const string& id);

void out();//pasien keluar

void filterByName(PatientData* head, const string& name);//pencarian dengan nama
string generateID();//nomor identitas
void allpatient();//menampilkan semua data pasien
string toLowercase(const string& str);//menyamakan huruf kapital dan non-kapital

void medictreat();//tindakan medis

bool isRoomFull();//mengecek ruangan ranap tersedia

//queue (antrean tindakan medis)
bool isEmpty(int action);
bool isFull(int action);
void enqueue(int action, const string& data);
void dequeue(int action);


void bayar(string id);//pembayaran

void printPath(int parent[], int j);//cetak alur ambulance
void dijkstra(int graph[maks][maks], int source, int destination);//menentukan rute ambulance

int processPayment(Node* root);//menentukan leaf, menentukan potongan
Node* createNode(string question, int discount);//tree
int diskon ();//potongan harga asuransi

PatientData* head = nullptr;

string id = "";
char slc;//pilihan

int main() {
    while (true) {
        system("cls");
        cout << "====================================================================================================" << endl;
        cout << "=                                                                                                  =" << endl;
        cout << "=---------------------------------RUMAH SAKIT ASEP KOMARUDIN---------------------------------------=" << endl;
        cout << "=                              Jl.Ponorogo, No.12, Jakarta Pusat                                   =" << endl;
        cout << "=                                  Izin No.12/XXI/2022/228922                                      =" << endl;
        cout << "====================================================================================================" << endl;
        cout << "1. Pasien Masuk" << endl;
        cout << "2. Update data pasien" << endl;
        cout << "3. Laporan tindakan medis" << endl; // rontgen, operasi, usg,
        cout << "4. Pasien Keluar" << endl;
        cout << "5. Tampilkan data pasien"<<endl;
        cout << "6. Panggilan Ambulance"<<endl;
        cout << "\nMasukkan menu pelayanan : ";
        cin >> slc;
        string nama;
        switch (slc) {
        case '1':
            masuk();
            break;
        case '2':
            system("cls");
            cout << "========================================= UPDATE DATA PASIEN =======================================" << endl;
            cout << "####################################################################################################" <<endl;
            cout << "Masukkan id pasien : ";
            cin >> id;
            update(&head, id);
            break;

        case '3':
            medictreat();
            break; 

        case '4':
            out();
            break;
        
        case '5':
        system ("cls");
            cout << "======================================= TAMPILKAN DATA PASIEN ======================================" << endl;
            cout << "####################################################################################################" <<endl;
            cout<<"\n\nAnda dapat memilih untuk menampilkan semua data atau memfilter dengan nama"<<endl;
            cout<<"\n1. Filter dengan nama"<<endl;
            cout<<"2. Tampilkan semua data"<<endl;
            cout<<"\n\nMasukkan Pilihan Anda : ";cin>>slc;
            if(slc=='1'){
                string name;
                cin.ignore();
                cout<<"Masukkan nama pasien : ";
                getline(cin, name);
                filterByName(head,name);
            }else{
                allpatient();
            }
            break;
        case '6':
        system("cls");
        int destination;
        cout << "======================================= Panggilan Ambulance ========================================" << endl;
        cout << "####################################################################################################" <<endl;
        
        cout<<"\n\n (1)  Desa Mawar - Kecamatan Jatiwangi"<<endl;
        cout<<" (2)  Desa Sumber Makmur - Kecamatan Cikarang Selatan"<<endl;
        cout<<" (3)  Desa Cipta Karya - Kecamatan Cibitung"<<endl;
        cout<<" (4)  Desa Harapan Jaya - Kecamatan Pondok Gede"<<endl;
        cout<<" (5)  Desa Mekar Sari - Kecamatan Cileungsi"<<endl;
        cout<<" (6)  Desa Bumi Indah - Kecamatan Bojong Gede"<<endl;
        cout<<" (7)  Desa Jaya Kusuma - Kecamatan Cisauk"<<endl;
        cout<<" (8)  Desa Cinta Damai - Kecamatan Parung"<<endl;
        cout<<" (10) Desa Suka Maju - Kecamatan Serpong"<<endl;
        cout<<" (11) Desa Bakti Jaya - Kecamatan Cikupa"<<endl;
        cout<<" (12) Desa Rukun Makmur - Kecamatan Cisoka"<<endl;
        cout<<" (13) Desa Sentosa Indah - Kecamatan Curug"<<endl;
        cout<<" (14) Desa Sejahtera - Kecamatan Karawaci"<<endl;
        cout<<" (15) Desa Gemilang - Kecamatan Kemang"<<endl;
        cout<<" (16) Desa Abadi Mulya - Kecamatan Sukamaju"<<endl;
        cout<<" (17) Desa Taman Bahagia - Kecamatan Tangerang"<<endl;
        cout<<" (18) Desa Makmur Jaya - Kecamatan Balaraja"<<endl;
        cout<<" (19) Desa Jaya Abadi - Kecamatan Sindang Jaya"<<endl;
        cout<<" (20) Desa Pusaka Indah - Kecamatan Jayanti"<<endl;
        cout<<"\n\nMasukkan kode tujuan \t: ";cin>>destination;

        if (destination > 0 && destination != 9 && destination<20){
            system("cls");
            cout << "======================================= Rute Ambulance ===========================================" << endl;
        cout << "####################################################################################################\n\n" <<endl;
            dijkstra(graph, 9, destination);
            cout<<"\nCatatan : Titik 9 merupakan titik keberangkatan (RS)"<<endl;
            cout<<"\n\nSerahkan ini pada supir Ambulance"<<endl;
            system("pause");
        }else{
            cout<<"Pilihan salah"<<endl;
            system("pause");
        }
        ;

        default:
            cout << "Pilihan tidak dalam menu" << endl;
            break;
        }
    }

    return 0;
}

void masuk() {
ulang:
    system("cls");
    cout << "========================================= PASIEN MASUK =======================================" << endl;
    cout << "##############################################################################################" <<endl;
    char tipe;

    cout << "\n\n1. Pasien berobat pertama" << endl;
    cout << "2. Pasien rawat jalan" << endl;
    cout << "\nMasukkan jenis pasien : ";
    cin >> tipe;

    if (tipe == '1') {
        inputData(&head);
    }
    else if (tipe == '2') {
        cout<<"Masukkan kode identitas pasien : ";cin>>id;
        readFromFile(id,1);
        
        system("pause");
    }
    else {
        cout << "Pilihan tidak dalam menu" << endl;
        system("pause");
        goto ulang;
    }
}

void inputData(PatientData** head) {
    system("cls");
    int poli;
    cout << "========================================= INPUT DATA PASIEN =======================================" << endl;
    cout << "###################################################################################################" << endl;
    PatientData* newNode = new PatientData();

    cout << "\nMasukkan nama pasien\t\t\t: ";
    cin.ignore();
    getline(cin, newNode->nama);

    cout << "Masukkan umur pasien\t\t\t: ";
    cin >> newNode->umur;

    cout << "Masukkan jenis kelamin pasien\t\t: ";
    cin >> newNode->kelamin;

    cout << "Masukkan berat badan pasien\t\t: ";
    cin >> newNode->bb;

    cout << "Masukkan tinggi badan pasien\t\t: ";
    cin >> newNode->tb;

    cout << "Masukkan golongan darah pasien\t\t: ";
    cin >> newNode->goldar;

    cin.ignore(); // Menghapus karakter '\n' dalam buffer

    cout << "Masukkan riwayat alergi pasien\t\t: ";
    getline(cin, newNode->alergi);

    cout << "Masukkan riwayat penyakit pasien\t: ";
    getline(cin, newNode->hispenyakit);

    cout << "Masukkan alamat pasien\t\t\t: ";
    getline(cin, newNode->alamat);

    cout << "Masukkan tekanan darah pasien\t\t: ";
    getline(cin, newNode->tensi);
    poli:
    system ("cls");
        cout << "====================================================================================================" << endl;
        cout << "=                                                                                                  =" << endl;
        cout << "=--------------------------------POLIKLINIK RUMAH SAKIT ASEP KOMARUDIN-----------------------------=" << endl;
        cout << "=                                                                                                  =" << endl;
        cout << "=                                                                                                  =" << endl;
        cout << "====================================================================================================" << endl;
    cout<<"1. Poli umum"<<endl;
    cout<<"2. Poli THT"<<endl;
    cout<<"3. Poli dalam"<<endl;
    cout<<"4. Poli bedah"<<endl;
    cout<<"5. Poli kulit"<<endl;
    cout<<"6. Poli jantung"<<endl;
    cout<<"7. Poli anak"<<endl;
    cout<<"8. Poli syaraf"<<endl;
    cout<<"9. Poli kandungan"<<endl;
    cout<<"10.Poli mata"<<endl;

    cout<<"\nPilih Poli : ";cin>>poli;
    if (poli > 0 && poli <11){
        newNode->dokter = poli;
        newNode->id = generateID();
        cout<<"Apakah pasien dalam keadaan gawat darurat? (y/n) \t: ";cin>>slc;
       
    }else{
        cout<<"\nPoli tidak tersedia, pilih kembali"<<endl;
        system("pause");
        goto poli;
    }
    system("cls");
     if(slc == 'y' || slc == 'Y'){
            cout<<"Pasien berhasil terdaftar, silahkan alihkan ke UGD tanpa antre"<<endl;
        }else{
            cout<<"Silahkan berikan struk ini pada pasien untuk antre pada Poliklinik"<<endl;
        }
    
    cout<<"====================================================================="<<endl;
    cout<<"    Nama Pasien \t\t:\t"<<newNode->nama<<endl;
    cout<<"    kode identitas \t\t:\t"<<newNode->id<<endl;
    cout<<"    Poliklinik Tujuan\t\t:\tPoli "<<info.nama[1][newNode->dokter-1]<<endl;
    cout<<"    Dokter Penanggung jawab \t:\t"<<info.nama[0][newNode->dokter-1]<<endl;
    cout<<"====================================================================="<<endl;

    newNode->next = nullptr;

    if (*head == nullptr) {
        *head = newNode;
    }
    else {
        PatientData* current = *head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    system("pause");


}

void update(PatientData** head, const string& id) {
    PatientData* current = *head;
    while (current != nullptr) {
        if (current->id == id) {

            cout << "\n\nData pasien ditemukan. Silakan masukkan data yang ingin diupdate: " << endl;
            cout << "1. Nama: " << current->nama << endl;
            cout << "2. Umur: " << current->umur << endl;
            cout << "3. Jenis Kelamin : " << current->kelamin << endl;
            cout << "4. Berat Badan: " << current->bb << endl;
            cout << "5. Tinggi Badan: " << current->tb << endl;
            cout << "6. Riwayat Penyakit: " << current->hispenyakit << endl;
            cout << "7. Alergi: " << current->alergi << endl;
            cout << "8. Alamat: " << current->alamat << endl;
            cout << "9. Tensi: " << current->tensi << endl;
            cout << "10. Golongan Darah: " << current->goldar << endl;
            

            int choice;
            cout << "\n\nPilih nomor data yang ingin diupdate: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    cout << "Masukkan Data Nama Baru: ";
                    cin.ignore();
                    getline(cin, current->nama);
                    break;
                case 2:
                    cout << "Masukkan Data Umur Baru: ";
                    cin >> current->umur;
                    break;
                case 3:
                    cout<< "Masukkan Data Jenis Kelamin Baru: ";
                    cin>> current->kelamin;
                case 4:
                    cout << "Masukkan Data Berat Badan Baru: ";
                    cin >> current->bb;
                    break;
                case 5:
                    cout << "Masukkan Data Tinggi Badan Baru: ";
                    cin >> current->tb;
                    break;
                case 6:
                    cout << "Masukkan Data Riwayat Penyakit Baru: ";
                    cin.ignore();
                    getline(cin, current->hispenyakit);
                    break;
                case 7:
                    cout << "Masukkan Data Alergi Baru: ";
                    cin.ignore();
                    getline(cin, current->alergi);
                    break;
                case 8:
                    cout << "Masukkan Data Alamat Baru: ";
                    cin.ignore();
                    getline(cin, current->alamat);
                    break;
                case 9:
                    cout << "Masukkan Data Tensi Baru: ";
                    cin.ignore();
                    getline(cin, current->tensi);
                    break;
                case 10:
                    cout << "Masukkan Data Golongan Darah Baru: ";
                    cin.ignore();
                    getline(cin, current->goldar);
                    break;
                case 12:
                    cout << "Masukkan Data Jenis Kelamin Baru: ";
                    cin.ignore();
                    getline(cin, current->kelamin);
                    break;
                default:
                    cout << "Pilihan tidak valid!" << endl;
                    break;
            }

            cout << "Data pasien berhasil diupdate." << endl;
            return;
        }
        current = current->next;
    }

    cout << "Data pasien dengan ID tersebut tidak ditemukan." << endl;
}

void medictreat(){
    system("cls");
    cout << "======================================= PENGAMBILAN TINDAKAN MEDIS =================================" << endl;
    cout << "####################################################################################################" <<endl;
    cout<<"\n\nMasukkan ID pasien\t:\t";cin>>id;
    carilinked(head,id);
    PatientData* current = head;
    PatientData* prev = nullptr;
    bool ditemukan = false;

    while (current != nullptr) {
        if (current->id == id) {
            // Pasien ditemukan
            ditemukan = true;
            break;
        }

        prev = current;
        current = current->next;
    }

    if (!ditemukan) {
        cout << "Pasien dengan id " << id << " tidak ditemukan." << endl;
        return;
    }
    system("cls");
    cout << "======================================= STATUS TINDAKAN MEDIS ======================================" << endl;
    cout << "####################################################################################################\n\n" <<endl;
    cout<<"1. Pasien Masuk"<<endl;
    cout<<"2. Pasien Keluar"<<endl;
    cout<<"Masukkan status pasien : ";cin>>slc;
    if(slc == '1'){
    int x;
    system("cls");
    cout << "======================================= PASIEN MASUK TINDAKAN MEDIS ================================" << endl;
    cout << "####################################################################################################" <<endl;
    cout<<"\n\n1. Operasi"<<endl;
    cout<<"2. Rontgen"<<endl;
    cout<<"3. Uji Lab"<<endl;
    cout<<"4. USG"<<endl;
    cout<<"5. Rawat inap"<<endl;
    cout<<"Masukkan tindakan medis yang diperlukan : ";cin>>x;
    
    if (x == 5){
        if (isRoomFull){
            current->ranap = true;
            cout<<"Pasien berhasil ditambahkan ke daftar rawat inap"<<endl;
            writeToFile(*current,2);
        }
    }else{
        current->treat = x;
        enqueue(current->treat,id);
    }
            }else{
                system("cls");
                cout << "======================================= PASIEN KELUAR TINDAKAN MEDIS ===============================" << endl;
                cout << "####################################################################################################\n\n" <<endl;
                if(current->treat < 4 && current->treat > 0){
                   dequeue(current->treat); 
                }
    }
    

}

void out() {
    system("cls");
    cout << "========================================= PASIEN KELUAR =======================================" << endl;
    cout << "##############################################################################################\n" <<endl;
    cout << "Masukkan id pasien : ";
    cin >> id;
    carilinked(head, id);

    PatientData* current = head;
    PatientData* prev = nullptr;
    bool ditemukan = false;

    while (current != nullptr) {
        if (current->id == id) {
            // Pasien ditemukan
            ditemukan = true;
            break;
        }

        prev = current;
        current = current->next;
    }

    if (!ditemukan) {
        cout << "Pasien dengan id " << id << " tidak ditemukan." << endl;
        return;
    }
    if (current->ranap == true){
        cout<<"Masukkan lama pasien di rawat inap (hari) : ";cin>>current->dayranap;
    }
    char rawat;
    cout << "Apakah pasien ini butuh rawat jalan? (y/n): ";
    cin >> rawat;

    if (rawat == 'y' || rawat == 'Y') {
        cout<<"Masukkan tanggal jadwal kunjungan selanjutnya: ";cin>>current->trajal;
        writeToFile(*current,1);
            system("cls");
            cout<<"=============================KARTU RAWAT JALAN======================"<<endl;
            cout<<"====================================================================="<<endl;
            cout<<"    Nama Pasien \t\t:\t"<<current->nama<<endl;
            cout<<"    kode identitas \t\t:\t"<<current->id<<endl;
            cout<<"    Poliklinik Tujuan\t\t:\tPoli "<<info.nama[1][current->dokter-1]<<endl;
            cout<<"    Dokter Penanggung jawab \t:\t"<<info.nama[0][current->dokter-1]<<endl;
            cout<<"    Tanggal Kunjungan \t\t:\t"<<current->trajal<<endl;
            cout<<"====================================================================="<<endl;
            cout<<"Bawa kartu ini di kunjugan selanjutnya..."<<endl;
    }
    cout<<"\nMasukkan biaya obat dari apotek\t\t  : Rp.";cin>>current->obat;

    bayar(id);
    readFromFile(id,2);
    // Menghapus node dari linked list
    if (prev == nullptr) {
        // Node yang dihapus adalah head
        head = current->next;
    } else {
        prev->next = current->next;
    }

    delete current;
    
    system("pause");
}

void writeToFile(const PatientData& patient, int konfirmasi) {
    ofstream file;
    if (konfirmasi == 1) {
        file.open("rajal.txt", ios::app);
    } else if (konfirmasi == 2) {
        file.open("ranap.txt", ios::app);
    } else {
        cout << "Konfirmasi tidak valid." << endl;
        return;
    }

    if (file.is_open()) {
        file << patient.id << "|" << patient.nama << "|" << patient.tensi << "|" << patient.umur << "|" << patient.goldar
            << "|" << patient.bb << "|" << patient.tb << "|" << patient.alergi << "|" << patient.hispenyakit
            << "|" << patient.alamat << "|" <<patient.trajal<< "|" <<patient.kelamin<< "|" <<patient.dokter<< "\n";
        file.close();
    } else {
        cout << "Tidak dapat membuka file." << endl;
    }
    system("pause");
}


void readFromFile(const string& id, int konfirmasi) {
    ifstream file;
    ofstream outputFile;
    if (konfirmasi == 1) {
        file.open("rajal.txt");
        outputFile.open("rajal_temp.txt");
    } else if (konfirmasi == 2) {
        file.open("ranap.txt");
        outputFile.open("ranap_temp.txt");
    } else {
        cout << "Konfirmasi tidak valid." << endl;
        return;
    }

    if (!file.is_open() || !outputFile.is_open()) {
        cout << "Tidak dapat membuka file." << endl;
        return;
    }

    string line;
    bool ditemukan = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        getline(ss, temp, '|');

        if (temp == id) {
            // Pasien ditemukan
            ditemukan = true;
            PatientData* newNode = new PatientData();
            newNode->id = id;

            // Ambil data lain dari file
            getline(ss, newNode->nama, '|');
            getline(ss, newNode->tensi, '|');
            ss >> newNode->umur;
            ss.ignore();
            getline(ss, newNode->goldar, '|');
            ss >> newNode->bb;
            ss.ignore();
            ss >> newNode->tb;
            ss.ignore();
            getline(ss, newNode->alergi, '|');
            getline(ss, newNode->hispenyakit, '|');
            getline(ss, newNode->alamat, '|');
            getline(ss, newNode->trajal, '|');
            getline(ss, newNode->kelamin, '|');
            ss >> newNode->dokter;
            ss.ignore();
            if (konfirmasi == 1) {
                // Tambahkan ke linked list
                newNode->next = nullptr;
                if (head == nullptr) {
                    head = newNode;
                } else {
                    PatientData* current = head;
                    while (current->next != nullptr) {
                        current = current->next;
                    }
                    current->next = newNode;
                }
                cout << "\n\nData Pasien sudah masuk ke dalam antrean dokter\nSilahkan kunjungi poli terkait dengan membawa kartu rawat jalan..." << endl;
            }

            continue;
        }

        // Salin data ke file output
        outputFile << line << "\n";
    }

    file.close();
    outputFile.close();

    if (!ditemukan && konfirmasi == 1) {
        cout << "Pasien dengan id " << id << " tidak ditemukan." << endl;
        return;
    }

    if (konfirmasi == 1) {
        // Ganti nama file temporary menjadi file asli
        remove("rajal.txt");
        rename("rajal_temp.txt", "rajal.txt");
    } else if (konfirmasi == 2) {
        // Ganti nama file temporary menjadi file asli
        remove("ranap.txt");
        rename("ranap_temp.txt", "ranap.txt");
    }
}


void allpatient() {
    system("cls");
    int jumlah = 0;
    PatientData* current = head;

    while (current != nullptr) {
        jumlah++;
        current = current->next;
    }

    cout << "Jumlah pasien: " << jumlah << endl;

    current = head;
    int count = 0;
    const int maxData = 5;

    while (current != nullptr) {
        if (count >= maxData) {
            char konfirmasi;
            cout << "Data selanjutnya? (y/n): ";
            cin >> konfirmasi;

            if (konfirmasi != 'y' && konfirmasi != 'Y') {
                break;
            }

            count = 0;
        }

        cout << "Data Pasien ke-" << (count + 1) << ":" << endl;
        cout << "Id                        : " << current->id << endl;
        cout << "Nama pasien               : " << current->nama << endl;
        cout << "Umur pasien               : " << current->umur << endl;
        cout << "Jenis kelamin pasien      : " << current->kelamin << endl;
        cout << "Berat badan pasien        : " << current->bb << endl;
        cout << "Tinggi badan pasien       : " << current->tb << endl;
        cout << "Golongan darah pasien     : " << current->goldar << endl;
        cout << "Riwayat alergi pasien     : " << current->alergi << endl;
        cout << "Riwayat penyakit pasien   : " << current->hispenyakit << endl;
        cout << "Alamat pasien             : " << current->alamat << endl;
        cout << "Tekanan darah pasien      : " << current->tensi << endl;
        cout << "Jadwal rawat jalan        : " << current->trajal << endl;
        cout<<  "Dokter Penanggung jawab   : " << info.nama[0][current->dokter-1] << endl;
        cout<<  "Poliklinik tujuan         : Poli " << info.nama[1][current->dokter-1] << endl;
        cout << "-------------------------------------------------" << endl;

        current = current->next;
        count++;
    }
    system("pause");
}

void filterByName(PatientData* head, const string& name) {
    string lowercaseName = toLowercase(name);
    PatientData* current = head;
    bool found = false;

    while (current != nullptr) {
        string lowercaseCurrentName = toLowercase(current->nama);

        if (lowercaseCurrentName == lowercaseName) {
            cout << "Id                        : " << current->id << endl;
            cout << "Nama pasien               : " << current->nama << endl;
            cout << "Umur pasien               : " << current->umur << endl;
            cout << "Jenis kelamin pasien      : " << current->kelamin << endl;
            cout << "Berat badan pasien        : " << current->bb << endl;
            cout << "Tinggi badan pasien       : " << current->tb << endl;
            cout << "Golongan darah pasien     : " << current->goldar << endl;
            cout << "Riwayat alergi pasien     : " << current->alergi << endl;
            cout << "Riwayat penyakit pasien   : " << current->hispenyakit << endl;
            cout << "Alamat pasien             : " << current->alamat << endl;
            cout << "Tekanan darah pasien      : " << current->tensi << endl;
            cout << "Jadwal rawat jalan        : " << current->trajal << endl;
            cout<<  "Dokter Penanggung jawab   : " << info.nama[0][current->dokter-1] << endl;
            cout<<  "Poliklinik tujuan         : Poli " << info.nama[1][current->dokter-1] << endl;
            cout << "-------------------------------------------------" << endl;

            found = true;
        }

        current = current->next;
    }

    if (!found) {
        cout << "Tidak ada pasien dengan nama yang cocok." << endl;
    }
    system("pause");
}

string toLowercase(const string& str) {
    string lowercaseStr = str;
    for (char& c : lowercaseStr) {
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';  // Mengubah huruf kapital menjadi huruf kecil
        }
    }
    return lowercaseStr;
}

void carilinked(const PatientData* head, const string& id) {

    const PatientData* current = head;
    system ("cls");
        cout << "====================================================================================================" << endl;
        cout << "=                                                                                                  =" << endl;
        cout << "=------------------------------------------DATA PASIEN---------------------------------------------=" << endl;
        cout << "=                                                                                                  =" << endl;
        cout << "====================================================================================================" << endl;
    bool ditemukan = false;
    
    while (current != nullptr) {
        if (current->id == id) {
            // Pasien ditemukan
        cout << "Id\t\t\t\t: " << current->id << endl;
        cout << "Nama pasien\t\t\t: " << current->nama << endl;
        cout << "Umur pasien\t\t\t: " << current->umur << endl;
        cout << "Jenis kelamin pasien\t\t: " << current->kelamin << endl;
        cout << "Berat badan pasien\t\t: " << current->bb << endl;
        cout << "Tinggi badan pasien\t\t: " << current->tb << endl;
        cout << "Golongan darah pasien\t\t: " << current->goldar << endl;
        cout << "Riwayat alergi pasien\t\t: " << current->alergi << endl;
        cout << "Riwayat penyakit pasien\t\t: " << current->hispenyakit << endl;
        cout << "Alamat pasien\t\t\t: " << current->alamat << endl;
        cout << "Tekanan darah pasien\t\t: " << current->tensi << endl;
        cout << "Jadwal rawat jalan\t\t: " << current->trajal << endl;
            
            ditemukan = true;
            break;
        }
        
        current = current->next;
    }
    
    if (!ditemukan) {
        cout << "Pasien dengan id " << id << " tidak ditemukan." << endl;
    }
    system("pause");
}

string generateID() {
    string id = "";
    
    srand(time(0));
    
    for (int i = 0; i < 5; i++) {
        int digit = rand() % 10;
        id += to_string(digit);
    }
    
    return id;
}

bool isRoomFull()
{
    ifstream file("ranap.txt");
    if (file)
    {
        // Menghitung jumlah baris pada file
        int lineCount = 0;
        string line;
        while (getline(file, line))
        {
            lineCount++;
        }

        // Mengecek apakah jumlah baris sudah mencapai 200
        if (lineCount >= 200)
        {
            return true;
        }
    }

    return false;
}

bool isEmpty(int action) {
    return medroom.front[action] == -1;
}

bool isFull(int action) {
    return (medroom.rear[action] + 1) % room == medroom.front[action];
}

void enqueue(int action, const string& data) {
    if (isFull(action)) {
        cout << "Antrian tindakan medis " << action << " penuh." << endl;
    } else {
        if (isEmpty(action)) {
            medroom.front[action] = 0;
            medroom.rear[action] = 0;
        } else {
            medroom.rear[action] = (medroom.rear[action] + 1) % room;
        }
        medroom.medicroom[action][medroom.rear[action]] = data;
        cout << "Pasien dengan nomor identitas '" << data << "' ditambahkan ke antrian tindakan medis " << action << endl;
    }
    system("pause");
}

void dequeue(int action) {

    if (isEmpty(action)) {
        cout << "Antrian tindakan medis " << action << " kosong." << endl;
    } else {
        string data = medroom.medicroom[action][medroom.front[action]];
        if (medroom.front[action] == medroom.rear[action]) {
            medroom.front[action] = -1;
            medroom.rear[action] = -1;
        } else {
            medroom.front[action] = (medroom.front[action] + 1) % room;
        }
        cout << "Pasien dengan nomor identitas '" << data << "' telah keluar dari ruang tindakan medis " << action << endl;
    }
    system("pause");
}

void bayar(string id){
    int bayartreat;
    system("cls");
    cout << "========================================= PEMBAYARAN PASIEN ==================================" << endl;
    cout << "##############################################################################################\n" <<endl;

    PatientData* current = head;
    PatientData* prev = nullptr;
    bool ditemukan = false;

    while (current != nullptr) {
        if (current->id == id) {
            // Pasien ditemukan
            ditemukan = true;
            break;
        }

        prev = current;
        current = current->next;
    }

    if (!ditemukan) {
        cout << "Pasien dengan id " << id << " tidak ditemukan." << endl;
        return;
    }
    
    cout<<"Apakah pasien mempunyai jaminan kesehatan? (y/n) : ";cin>>slc;
    int diss;
    if (slc == 'y' || slc == 'Y'){
        diss = diskon();
    }else{
        diss = 0;
    }
    system("cls");
    cout << "=================================== STRUK PEMBAYARAN PASIEN ==================================" << endl;
    cout << "##############################################################################################\n" <<endl;
    if (current->treat <4 && current->treat > 0){
        
        bayartreat = info.pricetreat[current->treat-1];
        cout<<"Biaya Tindakan "<<info.treat[current->treat-1]<<" \t\t\t: "<<bayartreat<<endl;
        
    }
    int total,tmptotal;
    cout<<"Pemeriksaan dokter \t\t\t: "<<info.harga[current->dokter-1]<<endl;
    cout<<"Biaya Obat\t\t\t\t: "<<current->obat<<endl;
    cout<<"Biaya Rawat Inap\t\t\t: "<<current->dayranap*200000<<endl;
    cout << "==============================================================================================" << endl;
    
    tmptotal = (info.harga[current->dokter-1]+current->obat+current->dayranap*200000+bayartreat);
    total = tmptotal-tmptotal*diss/100;
    cout<<"Total \t\t\t\t\t: Rp."<<tmptotal<<endl;
    cout<<"Potongan Jaminan kesehatan\t\t: "<<diss<<"\%"<<endl;
    cout<<"Total Pembayaran\t\t\t: Rp."<<total<<endl;
    


}

void printPath(int parent[], int j) {
    // Base case: jika mencapai node awal
    if (parent[j] == -1)
        return;

    printPath(parent, parent[j]);
    cout << parent[j] + 1 << " - ";
}

void dijkstra(int graph[maks][maks], int source, int destination) {
    int dist[maks];
    bool visited[maks];
    int parent[maks];

    for (int i = 0; i < maks; i++) {
        dist[i] = INT_MAX;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[source - 1] = 0;

    for (int count = 0; count < maks - 1; count++) {
        int minDist = INT_MAX;
        int minIndex = -1;

        for (int v = 0; v < maks; v++) {
            if (!visited[v] && dist[v] <= minDist) {
                minDist = dist[v];
                minIndex = v;
            }
        }

        visited[minIndex] = true;

        for (int v = 0; v < maks; v++) {
            if (!visited[v] && graph[minIndex][v] && dist[minIndex] != INT_MAX &&
                dist[minIndex] + graph[minIndex][v] < dist[v]) {
                dist[v] = dist[minIndex] + graph[minIndex][v];
                parent[v] = minIndex;
            }
        }
    }

    if (dist[destination - 1] != INT_MAX) {
        cout << "Rute terpendek dari rumah sakit ke desa (" << destination << ") adalah: ";
        printPath(parent, destination - 1);
        cout << destination << endl;
        cout << "Jarak\t: " << dist[destination - 1] <<"km"<< endl;
    } else {
        cout << "Tidak ada jalur yang tersedia dari rumah sakit " << source << " ke node " << destination << endl;
    }
}

int diskon() {
    system("cls");
    cout << "========================================= JAMINAN KESEHATAN ==================================" << endl;
    cout << "##############################################################################################\n" <<endl;

    
    // Membangun pohon keputusan
    Node* root = createNode("\n\n(1) Mandiri\n(2) Bpjs\n\nPilih jenis asuransi?", 0);
    root->yes = createNode("\n\n(1) Kelas I\n(2) Kelas II\n\nPilih Kelas Asuransi?", 0);
    root->yes->yes = createNode("Kelas I", 90);
    root->yes->no = createNode("Kelas II", 80);
    root->no = createNode("\n\n(1) Kelas I\n(2) Kelas II\n\nPilih kelas BPJS?", 0);
    root->no->yes = createNode("Kelas I", 95);
    root->no->no = createNode("Kelas II", 75);

    Node* tunaiNode = createNode("Tunai", 0);

    // Memproses pembayaran dan mendapatkan diskon
    int discount = processPayment(root);

    return discount;
    system ("cls");

}

Node* createNode(string question, int discount) {
    Node* newNode = new Node();
    newNode->question = question;
    newNode->discount = discount;
    newNode->yes = nullptr;
    newNode->no = nullptr;
    return newNode;
}

int processPayment(Node* root) {
    if (root->yes == nullptr && root->no == nullptr) {
        // Leaf node reached, return the discount value
        return root->discount;
    }

    cout << root->question << " : ";
    char answer;
    cin >> answer;

    if (answer == '1') {
        return processPayment(root->yes);
    } else {
        return processPayment(root->no);
    }
}