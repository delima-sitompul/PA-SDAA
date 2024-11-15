#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <unistd.h>
#include <regex>
using namespace std;

// Struktur Node untuk Double Linked List
struct Node {
    string username;
    string password;
    string nik;
    string namalengkap;
    string alamat;
    string tanggallahir;
    string notelepon;
    Node* next;
    Node* prev;

    string keluhan;
    string poli;
    string admin;
    string waktuKunjungan;
    bool sudahDitugaskan;
    bool darurat;
    int noUrut;
};

// Struktur Node untuk Keluhan
struct KeluhanNode {
    Node* node;
    string username;
    string nik;
    string namalengkap;
    string keluhan;
    string poli;
    bool statusDarurat;
    bool sudahDitugaskan;
    bool status;
    bool darurat;
    string waktuKunjungan;
    string keterangan;
    KeluhanNode* next;
};

Node* head = nullptr;
Node* tail = nullptr;
KeluhanNode* headKeluhan = nullptr;
KeluhanNode* tailKeluhan = nullptr;

struct KeluhanStack {
    KeluhanNode* head = nullptr;

    void push(KeluhanNode* newNode) {
        newNode->next = head;
        head = newNode;
    }

    KeluhanNode* pop() {
        if (head == nullptr) return nullptr;
        KeluhanNode* temp = head;
        head = head->next;
        return temp;
    }

    bool isEmpty() {
        return head == nullptr;
    }
};

KeluhanStack riwayatKeluhan;

// Stack menggunakan Linked List
void push(Node* newNode) {
    newNode->next = head;
    if (head != nullptr) {
        head->prev = newNode;
    }
    head = newNode;
}

// Pop untuk Stack
Node* pop() {
    if (head == nullptr) return nullptr;
    Node* temp = head;
    head = head->next;
    
    if (head == nullptr) {
        tail = nullptr;
    } else {
        head->prev = nullptr;
    }
    
    temp->next = nullptr;
    return temp;
}


// Queue enqueue
void enqueue(Node* newNode) {
    if (newNode == nullptr) {
        cout << "Node tidak valid!" << endl;
        return;
    }

    newNode->next = nullptr;

    if (tail == nullptr) {
        head = tail = newNode;
        newNode->prev = nullptr;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

// Queue dequeue
Node* dequeue() {
    if (head == nullptr) {
        cout << "Antrian kosong, tidak ada pasien yang bisa diproses." << endl;
        return nullptr;
    }

    Node* temp = head;
    head = head->next;

    if (head != nullptr) {
        head->prev = nullptr;
    } else {
        tail = nullptr;
    }
    
    return temp;
}
  
bool isUpdated = false;
string adminYangLogin = "ADMIN1";
int noUrut = 1;



bool isValidDateFormat(const string& date) {
    regex date_format("\\d{4}-\\d{2}-\\d{2}");
    return regex_match(date, date_format);
}

// Fungsi untuk menambahkan data ke Double Linked List
void addToList(string username, string password, string nik, string namalengkap, string alamat, string tanggallahir, string notelepon) {
    Node* newNode = new Node();
    newNode->username = username;
    newNode->password = password;
    newNode->nik = nik;
    newNode->namalengkap = namalengkap;
    newNode->alamat = alamat;
    newNode->tanggallahir = tanggallahir;
    newNode->notelepon = notelepon;
    newNode->next = nullptr;
    newNode->keluhan = "";
    newNode->poli = "Belum Ditentukan";
    newNode->sudahDitugaskan = false;
    // newNode->next = head;
    // head = newNode;
    newNode->prev = tail;

    if (tail != nullptr) {
        tail->next = newNode;
    }
    tail = newNode;

    if (head == nullptr) {
        head = newNode;
    }
}

// Fungsi untuk membaca data dari file dan membangun Double Linked List
void loadDataFromFile() {
    ifstream file("data_users.txt");
    string username, password, nik, namalengkap, alamat, tanggallahir, notelepon;
    
    if (file.is_open()) {
        while (file >> username >> password >> nik >> namalengkap >> alamat >> tanggallahir >> notelepon) {
            addToList(username, password, nik, namalengkap, alamat, tanggallahir, notelepon);
        }
        file.close();
    }
}

// Fungsi untuk menyimpan data dari Double Linked List ke file
void saveDataToFile() {
    ofstream file("data_users.txt", ios::trunc);
    Node* temp = head;
    while (temp != nullptr) {
        file << temp->username << " " << temp->password << " " << temp->nik << " " << temp->namalengkap << " " 
             << temp->alamat << " " << temp->tanggallahir << " " << temp->notelepon << endl;
        temp = temp->next;
    }
    file.close();
}

void enqueueKeluhan(string& keluhan, string& username) {
    KeluhanNode* newKeluhan = new KeluhanNode;
    newKeluhan->keluhan = keluhan;
    newKeluhan->username = username;
    newKeluhan->next = nullptr;

    if (headKeluhan == nullptr) {
        headKeluhan = newKeluhan;
        tailKeluhan = newKeluhan;
    } else {
        tailKeluhan->next = newKeluhan;
        tailKeluhan = newKeluhan;
    }
}

void dequeueKeluhan() {
    if (headKeluhan == nullptr) {
        cout << "Antrian keluhan kosong!" << endl;
        return;
    }

    KeluhanNode* temp = headKeluhan;
    headKeluhan = headKeluhan->next;

    delete temp;

    if (headKeluhan == nullptr) {
        tailKeluhan = nullptr;
    }
}

// ================================================================ menu Admin ===================================================================
void tampilkanAntrian() {
    system("cls");

    if (headKeluhan == nullptr) {
        cout << "Tidak ada keluhan dalam antrian." << endl;
        return;
    }

    KeluhanNode* temp = headKeluhan;
    cout << "==== Antrian Keluhan Pasien ====" << endl << endl;
    while (temp != nullptr) {
        cout << "Username: " << temp->username << ", Keluhan: " << temp->keluhan << endl;
        temp = temp->next;
    }
}

void hapusAntrian(string& username) {

    if (headKeluhan == nullptr) {
        cout << "Tidak ada keluhan dalam antrian." << endl;
        cout << endl;
        cout << "Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    KeluhanNode* temp = headKeluhan;
    KeluhanNode* prev = nullptr;
    bool keluhanDitemukan = false;

    // Mencari keluhan berdasarkan username
    while (temp != nullptr) {
        if (temp->username == username) {
            keluhanDitemukan = true;
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    if (keluhanDitemukan) {
        if (prev == nullptr) {
            headKeluhan = temp->next;
        } else {
            prev->next = temp->next;
        }
        delete temp;

        cout << "Keluhan pasien " << username << " berhasil dihapus dari antrian." << endl;
    } else {
        cout << "Keluhan dengan nama pasien " << username << " tidak ditemukan dalam antrian." << endl;
    }
    cout << endl;
    cout << "Tekan ENTER untuk kembali...";
    cin.ignore();
    cin.get();
}

void tetapkanPoli() {
    system("cls");
    if (headKeluhan == nullptr) {
        cout << "Tidak ada keluhan dalam antrian untuk diproses." << endl;
        cout << "Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    // Menampilkan keluhan yang ada
    KeluhanNode* keluhanTemp = headKeluhan;
    cout << "Keluhan yang ada dalam antrian\n" << endl;
    
    // Mengambil keluhan pertama dari antrian
    KeluhanNode* keluhanDiproses = headKeluhan;
    headKeluhan = headKeluhan->next;

    // Mencari pasien berdasarkan username
    Node* pasienTemp = head;
    while (pasienTemp != nullptr) {
        if (pasienTemp->username == keluhanDiproses->username) {
            pasienTemp->keluhan = keluhanDiproses->keluhan;
            pasienTemp->noUrut = noUrut++;
            cout << "No. Urut   : " << pasienTemp->noUrut << endl;
            cout << "Pasien     : " << pasienTemp->namalengkap << endl;
            cout << "Keluhan    : " << pasienTemp->keluhan << endl;

            string poli;
            int statusDarurat;
            cout << endl;
            cout << "Pilih poli: \n1. Umum\n2. THT\n3. Jantung\n4. Kulit\n \nMasukkan pilihan (1-4): ";
            int pilihanPoli;
            cin >> pilihanPoli;
            switch (pilihanPoli) {
                case 1:
                    poli = "Umum";
                    break;
                case 2:
                    poli = "THT";
                    break;
                case 3:
                    poli = "Jantung";
                    break;
                case 4:
                    poli = "Kulit";
                    break;
                default:
                    cout << "Pilihan tidak valid, menetapkan ke poli Umum." << endl;
                    poli = "Umum";
                    break;
            }

            cout << endl;
            cout << "Apakah ini kasus darurat? (1 untuk Ya, 0 untuk Tidak) : ";
            cin >> statusDarurat;

            pasienTemp->darurat = (statusDarurat == 1);
            pasienTemp->poli = poli;
            pasienTemp->sudahDitugaskan = true;

            string waktu;
            do {
                cout << "Masukkan Hari/Tanggal kunjungan (format yyyy-mm-dd)   : ";
                cin >> waktu;
                if (!isValidDateFormat(waktu)) {
                    cout << "\nFormat tanggal tidak valid. Harap masukkan dengan format yyyy-mm-dd." << endl;
                }
            } while (!isValidDateFormat(waktu));

            pasienTemp->waktuKunjungan = waktu;
            pasienTemp->admin = adminYangLogin;

            cout << endl;
            cout << "Poli dan waktu kunjungan berhasil ditetapkan oleh admin " << adminYangLogin << "!" << endl << endl;
            cout << "Tekan ENTER untuk kembali...";
            cin.ignore();
            cin.get();
            enqueue(pasienTemp);

            // Menambahkan data pasien ke riwayat
            KeluhanNode* riwayat = new KeluhanNode;
            riwayat->node = pasienTemp;
            riwayat->keluhan = pasienTemp->keluhan;
            riwayat->waktuKunjungan = pasienTemp->waktuKunjungan;
            riwayat->poli = pasienTemp->poli;
            riwayat->namalengkap = pasienTemp->namalengkap;
            riwayat->nik = pasienTemp->nik;
            riwayat->sudahDitugaskan = pasienTemp->sudahDitugaskan;
            riwayat->statusDarurat = pasienTemp->darurat; 
            riwayat->next = nullptr;

            riwayatKeluhan.push(riwayat);           
            return;
        }
        pasienTemp = pasienTemp->next;
    }

    cout << "Pasien dengan username " << keluhanDiproses->username << " tidak ditemukan." << endl;
    delete keluhanDiproses; // Menghapus keluhan yang tidak terpakai
}

void lihatRiwayatAntrian() {
    system("cls");
    if (riwayatKeluhan.isEmpty()) {
        cout << "\nTidak ada riwayat antrian yang tersedia." << endl;
        cout << "Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n==== Riwayat Antrian Pasien ====" << endl << endl;
    KeluhanNode* temp = riwayatKeluhan.head;    
    int noUrut = 1;
    while (temp != nullptr) {
        cout << "Nomor Urut     : " << noUrut << endl;
        if (!temp->keterangan.empty()) {
            cout << "Keterangan     : " << temp->keterangan << endl;
        }
        cout << "Nama Lengkap   : " << temp->namalengkap << "\n";
        cout << "NIK            : " << temp->nik << "\n";
        cout << "Keluhan        : " << (temp->keluhan.empty() ? "Belum Ada Keluhan" : temp->keluhan) << "\n";
        cout << "Poli           : " << (temp->poli.empty() ? "Belum Ditugaskan" : temp->poli) << "\n";
        cout << "Status         : " << (temp->sudahDitugaskan ? "Sudah Ditugaskan" : "Belum Ditugaskan") << "\n";
        cout << "Darurat        : " << (temp->statusDarurat ? "Ya" : "Tidak") << "\n";
        cout << "Hari/Tanggal   : " << (temp->waktuKunjungan.empty() ? "Belum Ditentukan" : temp->waktuKunjungan) << "\n";
        cout << "-----------------------------------------\n" << endl;
        temp = temp->next;
        noUrut++;
    }
    cout << "ENTER untuk selanjutnya...";
    cin.ignore();
    cin.get();

    int pilihanRiwayat;
    do {
        system("cls");
        cout << "\n==== Menu Riwayat Antrian ====" << endl;
        cout << "1. Lihat Riwayat Pasien Teratas\n";
        cout << "2. Hapus Riwayat Pasien Teratas\n";
        cout << "3. Kembali ke Menu Admin\n";
        cout << endl;
        cout << "Masukkan pilihan Anda: ";
        cin >> pilihanRiwayat;
        switch (pilihanRiwayat) {
            case 1: {
                if (!riwayatKeluhan.isEmpty()) {
                    system("cls");
                    KeluhanNode* node = riwayatKeluhan.head;
                    cout << "====== Detail Pasien Teratas di Riwayat ======" << endl << endl;
                    cout << "Nama Lengkap   : " << node->namalengkap << "\n";  // Akses melalui node pointer
                    cout << "NIK            : " << node->nik << "\n";  // Akses melalui node pointer
                    cout << "Keluhan        : " << node->keluhan << "\n";
                    cout << "Poli           : " << node->poli << "\n";
                    cout << "Status         : " << (node->sudahDitugaskan ? "Sudah Ditugaskan" : "Belum Ditugaskan") << "\n";
                    cout << "Darurat        : " << (node->statusDarurat ? "Ya" : "Tidak") << "\n";
                    cout << endl;
                    cout << "Tekan ENTER untuk kembali...";
                    cin.ignore();
                    cin.get();
                } else {
                    cout << "Tidak ada pasien di riwayat." << endl;
                    cout << "Tekan ENTER untuk kembali...";
                    cin.ignore();
                    cin.get();   
                }
                break;
            }
            case 2: {
                if (!riwayatKeluhan.isEmpty()) {
                    KeluhanNode* node = riwayatKeluhan.pop(); 
                    if (temp != nullptr) {
                        cout << "\nPasien " << node->namalengkap << " telah dihapus dari riwayat." << endl;
                        cout << "Tekan ENTER untuk kembali...";
                        cin.ignore();
                        cin.get();
                        delete node;  // Menghapus node pasien setelah dihapus dari riwayat
                    }
                } else {
                    cout << "\nTidak ada pasien di riwayat untuk dihapus." << endl;
                    cout << "Tekan ENTER untuk kembali...";
                    cin.ignore();
                    cin.get();
                }
                break;
            }
            case 3:
                cout << "Kembali ke Menu Admin..." << endl;
                return;
            default:
                cout << "Pilihan tidak valid." << endl;
        }
    } while (pilihanRiwayat != 3);
}

void perbaruiDataPasien(string& username) {
    system("cls");

    if (head == nullptr) {
        cout << "Tidak ada pasien dalam antrian." << endl;
        return;
    }

    Node* temp = head;
    bool pasienDitemukan = false;

    // Mencari pasien berdasarkan username
    while (temp != nullptr) {
        if (temp->username == username) {
            pasienDitemukan = true;

            cout << "Pasien atas nama " << temp->namalengkap << endl << endl;
            cout << "Status saat ini" << endl;
            cout << "Keluhan            : " << temp->keluhan << endl;
            cout << "Poli               : " << temp->poli << endl;
            cout << "Status             : " << (temp->sudahDitugaskan ? "Sudah Ditugaskan" : "Belum Ditugaskan") << "\n";
            cout << "Darurat            : " << (temp->darurat ? "Ya" : "Tidak") << endl;
            cout << "Tanggal Kunjungan  : " << temp->waktuKunjungan << endl;
            cout << endl;

            // Update status sudah ditugaskan
            string statusDitugaskanInput;
            cout << "Apakah pasien ini sudah ditugaskan? (1 untuk Ya, 0 untuk Tidak) : ";
            getline(cin, statusDitugaskanInput);
            if (!statusDitugaskanInput.empty()) {
                temp->sudahDitugaskan = (statusDitugaskanInput == "1");
            }

            // Update status darurat
            string statusDaruratInput;
            cout << "Apakah pasien ini kasus darurat? (1 untuk Ya, 0 untuk Tidak)    : ";
            getline(cin, statusDaruratInput);
            if (!statusDaruratInput.empty()) {
                temp->darurat = (statusDaruratInput == "1");
            }

            // Update tanggal kunjungan
            string waktuInput;
            cout << "Masukkan tanggal kunjungan baru (format yyyy-mm-dd)             : ";
            getline(cin, waktuInput);
            if (!waktuInput.empty()) {
                if (isValidDateFormat(waktuInput)) {
                    temp->waktuKunjungan = waktuInput;
                } else {
                    cout << "Format tanggal tidak valid, tanggal tidak diubah." << endl;
                }
            }

            // Menyimpan perubahan ke riwayat
            KeluhanNode* riwayatBaru = new KeluhanNode;
            riwayatBaru->node = temp;
            riwayatBaru->namalengkap = temp->namalengkap;
            riwayatBaru->nik = temp->nik;
            riwayatBaru->keluhan = temp->keluhan;
            riwayatBaru->poli = temp->poli;
            riwayatBaru->waktuKunjungan = temp->waktuKunjungan;
            riwayatBaru->statusDarurat = temp->darurat;
            riwayatBaru->sudahDitugaskan = temp->sudahDitugaskan;
            riwayatBaru->keterangan = "Data Update";
            riwayatBaru->next = nullptr;

            riwayatKeluhan.push(riwayatBaru);

            cout << "\nData pasien berhasil diperbarui!" << endl << endl;
            cout << "Tekan ENTER untuk kembali...";
            cin.get();
            return;
        }
        temp = temp->next;
    }

    if (!pasienDitemukan) {
        cout << "Pasien dengan username " << username << " tidak ditemukan." << endl;
    }
}


// ================================================================ menu pasien ===================================================================
void showUserData(string& username) {
    system("cls");
    Node* temp = head;

    if (temp == nullptr) {
        cout << "Tidak ada data pengguna yang ditemukan.\n";
        cout << "Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    while (temp != nullptr) {
        if (temp->username == username) {
            cout << "\n               Data Diri Anda\n";
            cout << "------------------------------------------" << endl;
            cout << "       Username       : " << temp->username << endl;
            cout << "       Nama Lengkap   : " << temp->namalengkap << endl;
            cout << "       NIK            : " << temp->nik << endl;
            cout << "       Alamat         : " << temp->alamat << endl;
            cout << "       Tanggal Lahir  : " << temp->tanggallahir << endl;
            cout << "       No Telepon     : " << temp->notelepon << endl;
            cout << "------------------------------------------" << endl;

            cout << "\nTekan ENTER untuk kembali...";
            cin.ignore();
            cin.get();
            system("cls");
            return;
        }
        temp = temp->next;
    }
    cout << "Data tidak ditemukan.\n";
    cout << "Tekan ENTER untuk kembali...";
    cin.ignore();
    cin.get();
    return;
}

void updateUserData(string& username) {
    system("cls");
    Node* temp = head;

    if (temp == nullptr) {
        cout << "Tidak ada data pengguna yang ditemukan.\n";
        cout << "Tekan ENTER untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    while (temp != nullptr) {
        if (temp->username == username) {
            cout << "Pilih data yang ingin diperbarui:\n";
            cout << "1. NIK\n";
            cout << "2. Nama Lengkap\n";
            cout << "3. Alamat\n";
            cout << "4. Tanggal Lahir\n";
            cout << "5. No Telepon\n";
            cout << "Pilihan: ";
            int choice;
            cin >> choice;
            cin.ignore();
            system("cls");

            string newData;

            if (choice == 1) {
                cout << "Perbarui NIK   : ";
                getline(cin, newData);
                if (newData.empty()) {
                    cout << "Tidak ada data yang berubah." << endl;
                } else if (newData.length() == 16) {
                    temp->nik = newData;
                    isUpdated = true;
                } else {
                    cout << "NIK harus 16 digit." << endl;
                }
            } else if (choice == 2) {
                cout << "Masukkan Nama Lengkap Baru : ";
                getline(cin, newData);
                if (newData.empty()) {
                    cout << "Tidak ada data yang berubah." << endl;
                } else {
                    temp->namalengkap = newData;
                    isUpdated = true;
                }
            } else if (choice == 3) {
                cout << "Masukkan Alamat Baru : ";
                getline(cin, newData);
                if (newData.empty()) {
                    cout << "Tidak ada data yang berubah." << endl;
                } else {
                    temp->alamat = newData;
                    isUpdated = true;
                }
            } else if (choice == 4) {
                cout << "Masukkan Tanggal Lahir Baru (YYYY-MM-DD): ";
                getline(cin, newData);
                if (newData.empty()) {
                    cout << "Tidak ada data yang berubah." << endl;
                } else {
                    temp->tanggallahir = newData;
                    isUpdated = true;
                }
            } else if (choice == 5) {
                cout << "Masukkan No Telepon Baru  : ";
                getline(cin, newData);
                if (newData.empty()) {
                    cout << "Tidak ada data yang berubah." << endl;
                } else {
                    temp->notelepon = newData;
                    isUpdated = true;
                }
            } else {
                cout << "Pilihan tidak valid.\n";
                sleep(1);
                return;
            }

            if (isUpdated) {
                cout << "\nData berhasil diperbarui.\n";
                saveDataToFile();
                isUpdated = false;
            }
            cout << "Tekan ENTER untuk kembali...";
            cin.ignore();
            cin.get();
            return;
        }
        temp = temp->next;
    }

    cout << "Akun tidak ditemukan.\n";
    cout << "Tekan ENTER untuk kembali...";
    cin.ignore();
    cin.get();
}


void kirimKeluhan(string& username) {
    system("cls");
    
    if (head == nullptr) {
        cout << "Tidak ada pasien dalam antrian." << endl;
        return;
    }

    Node* temp = head;
    bool pasienDitemukan = false;

    // Mencari pasien berdasarkan username
    while (temp != nullptr) {
        if (temp->username == username) {
            // Pasien ditemukan, maka kirim keluhan
            cout << "\nMasukkan keluhan Anda: ";
            cin.ignore();
            getline(cin, temp->keluhan);

            // Update status pasien dalam antrian
            temp->sudahDitugaskan = false;
            temp->poli = "Menunggu";

            enqueueKeluhan(temp->keluhan, temp->username);

            cout << "Keluhan berhasil dikirim! Silakan tunggu admin untuk menetapkan poli." << endl;
            cout << "Tekan ENTER untuk kembali...";
            cin.ignore();
            return;
        }
        temp = temp->next;
    }
    
    if (!pasienDitemukan) {
        cout << "Pasien tidak ditemukan!" << endl;
        return;
    }
}

void lihatAntrianPengguna(string& username) {
    system("cls");
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->username == username) {
            cout << "\n==== Status Antrian Anda ====" << endl;
            cout << "No Urut        : " << (temp->noUrut == 0 ? "Belum Ditentukan" : to_string(temp->noUrut)) << "\n";
            cout << "Nama Lengkap   : " << temp->namalengkap << "\n";
            cout << "NIK            : " << temp->nik << "\n";
            cout << "Keluhan        : " << (temp->keluhan.empty() ? "Belum Ada Keluhan" : temp->keluhan) << "\n";
            cout << "Poli           : " << (temp->poli.empty() ? "Belum Ditugaskan" : temp->poli) << "\n";
            cout << "Status         : " << (temp->sudahDitugaskan ? "Sudah Ditugaskan" : "Belum Ditugaskan") << "\n";
            cout << "Darurat        : " << (temp->darurat ? "Ya" : "Tidak") << "\n";
            cout << "Hari/Tanggal   : " << (temp->waktuKunjungan.empty() ? "Belum Ditentukan" : temp->waktuKunjungan) << "\n";
            cout << "--------------------------------" << endl << endl;

            cout << "Tekan ENTER untuk kembali...";
            cin.ignore();
            cin.get();
            return;
        }
        temp = temp->next;
    }
    cout << "Pasien tidak ditemukan!" << endl;
}


// ================================================================= MENU ==========================================================
int logres();
int menuadmin(){
    string username, sort, cari;
    string pilihanAdmin;
        do {   
            system("cls");         
            cout << "---------------------------------------" << endl;
            cout << "               Menu Admin              " << endl;
            cout << "---------------------------------------" << endl;
            cout << "     1. Lihat Antrian                  " << endl;
            cout << "     2. Tetapkan Poli                  " << endl;
            cout << "     3. Lihat Riwayat Antrian          " << endl;
            cout << "     4. Sort Riwayat Pasien            " << endl;
            cout << "     5. Cari Riwayat Pasien            " << endl;
            cout << "     6. Hapus Pasien dari Antrian      " << endl;
            cout << "     7. Perbarui Detail Pasien         " << endl;
            cout << "     8. Kembali                        " << endl;
            cout << "---------------------------------------" << endl;
            cout << endl;
            cout << "Pilih Menu: ";
            cin >> pilihanAdmin;

            if (pilihanAdmin == "1") {
                tampilkanAntrian();
                cout << endl;
                cout << "Tekan ENTER untuk kembali...";
                cin.ignore();
                cin.get();
            } 
            else if (pilihanAdmin == "2") {
                tetapkanPoli();
            } 
            else if (pilihanAdmin == "3") {
                lihatRiwayatAntrian();
            } 
            else if (pilihanAdmin == "4") {
                do{
                    system("cls");         
                    cout << "---------------------------------------------------------" << endl;
                    cout << "                         Menu Sort                       " << endl;
                    cout << "---------------------------------------------------------" << endl;
                    cout << "     1. Urutkan Berdasarkan Nomor Secara Descending      " << endl;
                    cout << "     2. Urutkan Berdasarkan Nama Secara Ascending        " << endl;
                    cout << "     3. Kembali                                          " << endl;
                    cout << "---------------------------------------------------------" << endl;
                    cout << endl;
                    cout << "Pilih Menu: ";
                    cin >> sort;

                    if (sort == "1"){

                    }
                    else if (sort == "2"){

                    }
                    else if (sort == "3"){
                        menuadmin();
                    }
                    else{
                        cout << "Pilihan tidak valid." << endl;
                        sleep(1);
                    }

                }while (sort != "3");
            } 
            else if (pilihanAdmin == "5") {
                do{
                    system("cls");         
                    cout << "--------------------------------------------" << endl;
                    cout << "                 Menu Search                " << endl;
                    cout << "--------------------------------------------" << endl;
                    cout << "     1. Caru Berdasarkan Nomor Riwayat      " << endl;
                    cout << "     2. Caru Berdasarkan Nama Riwayat       " << endl;
                    cout << "     3. Kembali                             " << endl;
                    cout << "--------------------------------------------" << endl;
                    cout << endl;
                    cout << "Pilih Menu: ";
                    cin >> sort;

                    if (sort == "1"){

                    }
                    else if (sort == "2"){

                    }
                    else if (sort == "3"){
                        menuadmin();
                    }
                    else{
                        cout << "Pilihan tidak valid." << endl;
                        sleep(1);
                    }

                }while (sort != "3");
            } 
            else if (pilihanAdmin == "6") {
                tampilkanAntrian();
                cout << endl;
                cout << "Masukkan username pasien yang ingin dihapus : ";
                cin >> username;
                hapusAntrian(username);
            } 
            else if (pilihanAdmin == "7") {
                cout << "Masukkan username pasien yang ingin diperbarui : ";
                cin.ignore();
                getline(cin, username);
                perbaruiDataPasien(username);
            } 
            else if (pilihanAdmin == "8") {
                cout << endl;
                cout << "Kembali...." << endl;
                sleep(1);
                system("cls");
                logres();
                return 0;
            } 
            else {
                cout << "Pilihan tidak valid." << endl;
                sleep(1);
                menuadmin();
            }
        }while (pilihanAdmin != "7");
};

int menupasien(){
    string username;
    string pilihanPengguna;
        do {
            system("cls");
            cout << "---------------------------------------" << endl;
            cout << "              Menu Pengguna            " << endl;
            cout << "---------------------------------------" << endl;
            cout << "     1. Lihat Data Diri                " << endl;
            cout << "     2. Update Data Diri               " << endl;
            cout << "     3. Keluhan Penyakit               " << endl;
            cout << "     4. Lihat Status Antrian           " << endl;
            cout << "     5. Kembali                        " << endl;
            cout << "---------------------------------------" << endl;
            cout << endl;
            cout << "Pilih Menu: ";
            cin >> pilihanPengguna;

            if (pilihanPengguna == "1") {
                cout << "Masukkan username: ";
                cin >> username;
                showUserData(username);
            } else if (pilihanPengguna == "2") {
                cout << "Masukkan username: ";
                cin >> username;
                updateUserData(username);
            } else if (pilihanPengguna == "3") {
                cout << "Masukkan username: ";
                cin >> username;
                kirimKeluhan(username);
            } else if (pilihanPengguna == "4") {
                cout << "Masukkan username: ";
                cin >> username;
                lihatAntrianPengguna(username);
            } else if (pilihanPengguna == "5") {
                cout << endl;
                cout << "Kembali...." << endl;
                sleep(1);
                system("cls");
                logres();
                return 0;
            } else {
                cout << "Pilihan tidak valid." << endl;
                sleep(1);
                menupasien();
            }

        } while (pilihanPengguna != "5");
}

// =============================================================================================================================================

// Fungsi untuk mengecek login
bool isLoggedIn() {
    string username, password;
    string un, pw;

    system("cls");

    cout << "---------------------------" << endl;
    cout << "        Harap Login        " << endl;
    cout << "---------------------------" << endl;
    cout << "   Username  : ";
    cin >> username;
    cin.ignore();
    cout << "   Password  : ";
    cin >> password;

    if (username == "admin" && password == "rs") {
        system("cls");
        menuadmin();
        return true;
    }

    // Periksa login dengan data dari double linked list
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->username == username && temp->password == password) {
            return true;
        }
        temp = temp->next;
    }

    return false;  // Login gagal jika tidak ada yang cocok
}

int logres() {
    string username;
    string pilihan;
    system("cls");
    cout << endl;
    do {
        cout << "---------------------------" << endl;
        cout << "       Pilihan Menu        " << endl;
        cout << "---------------------------" << endl;
        cout << "       1. Register         " << endl;
        cout << "       2. Login            " << endl;
        cout << "       3. Logout           " << endl;
        cout << "---------------------------" << endl;

        cout << endl;
        cout << "Pilih : ";
        cin >> pilihan;

        if (pilihan == "1") { 
            system("cls");

            cout << "                                        DISCLAIMER                                            " << endl << endl;
            cout << "   1. Pastikan Anda mengisi data dengan benar dan lengkap.                                    " << endl;
            cout << "   2. Gunakan underscore untuk pengganti spasi pada username.                                 " << endl;
            cout << "   3. NIK yang dimasukkan harus 16 digit sesuai dengan identitas resmi Anda.                  " << endl;
            cout << "   4. Data yang dimasukkan akan digunakan untuk keperluan administrasi dan pelayanan sistem.  " << endl;
            cout << "   5. Kami tidak akan membagikan atau menyalahgunakan data pribadi Anda tanpa izin.           " << endl;
            cout << "   6. Jika Anda mengalami kesulitan atau masalah, hubungi layanan pelanggan kami.             " << endl;
            cout << "   7. Program ini terkadang masih memiliki bug tidak terduga.                                 " << endl;
            cout << "===============================================================================================" << endl << endl << endl; 

            string username, password, nik, namalengkap, alamat, tanggallahir, notelepon;

            cout << "                                     -- DATA DIRI -- " << endl << endl;

            cout << "Buat Username Login     : ";
            cin >> username;
            cout << "Buat Password Login     : ";
            cin >> password;

            while (true) {
                cout << "Masukkan NIK            : ";
                cin >> nik;
                if (nik.length() == 16) break;
                cout << "Harap input NIK dengan benar. \nENTER untuk lanjut...";
                getche();
                cin.get();
            }

            cout << "Masukkan Nama Lengkap   : ";
            getline(cin >> ws, namalengkap);
            cout << "Masukkan Alamat         : ";
            getline(cin, alamat);
            cout << "Masukkan Tanggal Lahir (DD-MM-YYYY): ";
            cin >> tanggallahir;
            cout << "Masukkan No. Telepon    : ";
            cin >> notelepon;

            // Simpan data ke file
            ofstream file("data_users.txt", ios::app);
            if (file.is_open()) {
                file << username << " " << password << " " << nik << " " << namalengkap << " " << alamat << " " << tanggallahir << " " << notelepon << endl;
                file.close();

                // Simpan data ke Double Linked List
                addToList(username, password, nik, namalengkap, alamat, tanggallahir, notelepon);

                cout << endl;
                cout << "Hello " << username << "! Selamat Anda berhasil Register. Silahkan Login ke akun Anda." << endl;
                cout << endl;
                cout << "ENTER untuk lanjut...";
                getche();
                system("cls");
                logres();
            } else {
                cout << "Gagal memuat data." << endl;
            }
        } 
        else if (pilihan == "2") {
            bool status = isLoggedIn();
            if (!status) {
                cout << endl;
                cout << "Invalid Login." << endl;
                cout << "ENTER untuk lanjut...";
                getche();
                system("cls");
                logres();
            } else {
                cout << endl;
                cout << "Login Berhasil!" << endl;
                cout << endl;
                cout << "ENTER untuk lanjut...";
                getche();
                system("cls");
                cout << endl;
                menupasien();
            }
        } else if (pilihan == "3") {
            system("cls");
            cout << "Bye Bye... Sampai Jumpa Kembali." << endl;
            cout << endl;
            break;
        } else {
            cout << "Pilihan tidak valid." << endl;
            sleep(1);
            logres();
            break;
        }
    } while (pilihan != "3");
}

int main() {
    system("cls");
    cout << endl;
    cout << "SELAMAT DATANG....\n";
    loadDataFromFile(); // Load data dari file ke dalam double linked list
    sleep(1);
    logres();
    saveDataToFile(); // Save data dari double linked list ke file sebelum program berhenti
    return 0;
}
