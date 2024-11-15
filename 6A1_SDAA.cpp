#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <unistd.h>
#include <regex>
#include <cmath>
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
KeluhanNode* KeluhanDiproses = headKeluhan;


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
int NoUrut = 0;



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
    newNode->keluhan = "";
    newNode->poli = "Belum Ditentukan";
    newNode->sudahDitugaskan = false;
    newNode->next = nullptr;
    newNode->prev = tail;

    newNode->noUrut = ++NoUrut;

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
            pasienTemp->noUrut = NoUrut++;
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

// quick sort
Node* partitionNodes(Node* head, Node* end, Node*& newHead, Node*& newEnd) {
    Node* pivot = end;
    Node* prev = nullptr, *cur = head, *tail = pivot;

    while (cur != pivot) {
        if (cur->noUrut < pivot->noUrut) {
            if (newHead == nullptr) {
                newHead = cur;
            }
            prev = cur;
            cur = cur->next;
        } else {
            if (prev) {
                prev->next = cur->next;
            }
            Node* tmp = cur->next;
            cur->next = nullptr;
            tail->next = cur;
            tail = cur;
            cur = tmp;
        }
    }

    if (newHead == nullptr) {
        newHead = pivot;
    }
    newEnd = tail;
    return pivot;
}

Node* quickSortRecur(Node* head, Node* end) {
    if (!head || head == end) {
        return head; 
    }

    Node *newHead = nullptr, *newEnd = nullptr;
    Node* pivot = partitionNodes(head, end, newHead, newEnd);
 
    if (newHead != pivot) {
        Node* tmp = newHead;
        while (tmp->next != pivot) {
            tmp = tmp->next;
        }
        tmp->next = nullptr; 
        newHead = quickSortRecur(newHead, tmp); 

        tmp = newHead;
        while (tmp->next != nullptr) {
            tmp = tmp->next;
        }
        tmp->next = pivot;
    }

    pivot->next = quickSortRecur(pivot->next, newEnd);
    return newHead; 
}

void quickSort(Node*& head) {
    if (!head) return;

    Node* tail = head;
    while (tail->next) {
        tail = tail->next; 
    }

    head = quickSortRecur(head, tail); 
}


Node* searchByNoUrut(Node* head, int noUrut) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->noUrut == noUrut) {
            return temp;  
        }
        temp = temp->next;
    }
    return nullptr;  
}

void printList(Node* head) {
    Node* temp = head;
    while (temp != nullptr) {
        cout << temp->noUrut << " ";
        temp = temp->next;
    }
    cout << endl;
}


// shell sort

void linkedList2Array(Node*head, int counter, Node* arr[]) {
    int i = 0;
    while (head != nullptr) {
        arr[i] = head;
        head = head->next;
        i++;
    }
}

void array2LinkedList(Node *&head, int counter, Node* arr[]) {
    for (int i = 0; i < counter; i++) {
        if (i > 0) {
            arr[i-1]->next = arr[i];  
        }
        arr[i]->next = nullptr;  
    }
    head = arr[0];  
}

void shellSort(Node*& head) {
    int counter = 0;
    Node* temp = head;
    while (temp != nullptr) {
        counter++;
        temp = temp->next;
    }

    if (counter == 0) {
        cout << "No patients to sort." << endl;
        return;
    }

    Node** arr = new Node*[counter];
    linkedList2Array(head, counter, arr);

    for (int gap = counter / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < counter; i++) {
            Node* temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap]->namalengkap < temp->namalengkap; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }

    array2LinkedList(head, counter, arr);

    cout << "\n==== Sorted Antrian Pasien ====" << endl;
    temp = head;
    while (temp != nullptr) {
        cout << "Nama: " << temp->namalengkap << ", No Urut: " << temp->noUrut << endl;
        temp = temp->next;
    }

    delete[] arr;
}

// jump search
int countPasien(Node* head) {
    int count = 0;
    Node* current = head;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

int jumpSearchNoUrut(Node* head, int targetNoUrut, int n) {
    if (n == 0) {
        cout << "Daftar pasien kosong." << endl;
        return -1;
    }

    int step = sqrt(n);  // Langkah untuk meloncat
    // int prev = 0;        // Indeks node sebelumnya
    Node* current = head;
    Node* prevNode = nullptr;

    // Mencari rentang yang bisa mengandung targetNoUrut
    while (current != nullptr && current->noUrut < targetNoUrut) {
        prevNode = current;
        for (int i = 0; i < step && current != nullptr; i++) {
            prevNode = current;
            current = current->next;  // Melangkah sesuai 'step'
        }
        // Jika sudah melewati targetNoUrut, hentikan pencarian
        if (current == nullptr || current->noUrut >= targetNoUrut) {
            break;
        }
    }

    // Lakukan pencarian linear dari node sebelumnya
    while (prevNode != nullptr && prevNode->noUrut < targetNoUrut) {
        prevNode = prevNode->next;
    }

    // Jika pasien ditemukan
    if (prevNode != nullptr && prevNode->noUrut == targetNoUrut) {
        Node* pasien = prevNode;
        cout << "Pasien ditemukan: " << endl;
        cout << "No Urut: " << pasien->noUrut << endl;
        cout << "Nama: " << pasien->namalengkap << endl;
        cout << "NIK: " << pasien->nik << endl;
        cout << "Keluhan: " << (pasien->keluhan.empty() ? "Belum Ada Keluhan" : pasien->keluhan) << endl;
        cout << "Poli: " << (pasien->poli.empty() ? "Belum Ditugaskan" : pasien->poli) << endl;
        cout << "Status: " << (pasien->sudahDitugaskan ? "Sudah Ditugaskan" : "Belum Ditugaskan") << endl;
        cout << "Darurat: " << (pasien->darurat ? "Ya" : "Tidak") << endl;
        cout << "Hari/Tanggal: " << (pasien->waktuKunjungan.empty() ? "Belum Ditentukan" : pasien->waktuKunjungan) << endl;
        return prevNode->noUrut;
    }

    // Jika tidak ditemukan
    cout << "Pasien dengan No Urut " << targetNoUrut << " tidak ditemukan." << endl;
    return -1;
}

// boyermoore search
const int NO_OF_CHARS = 256;

void badCharHeuristic(const string& str, int size, int badChar[NO_OF_CHARS]) {
    for (int i = 0; i < NO_OF_CHARS; i++) {
        badChar[i] = -1;  
    }
    for (int i = 0; i < size; i++) {
        badChar[(int)str[i]] = i;  
    }
}

int boyerMooreSearch(const string& text, const string& pattern) {
    int m = pattern.size();
    int n = text.size();
    if (m > n) return -1;  

    int badChar[NO_OF_CHARS];
    badCharHeuristic(pattern, m, badChar);  

    int s = 0;  
    while (s <= (n - m)) {  
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) {  
            j--;
        }
        if (j < 0) {  
            return s;  
        } else {  
            s += max(1, j - badChar[(int)text[s + j]]);
        }
    }
    return -1; 
}

void cariPasienBerdasarkanNama(const string& nama) {
    bool found = false;
    Node* current = head; 

    while (current != nullptr) {
        int result = boyerMooreSearch(current->namalengkap, nama);
        
        if (result != -1) {
            cout << "Pasien ditemukan: " << endl;
            cout << "No Urut: " << current->noUrut << endl;
            cout << "Nama: " << current->namalengkap << endl;
            cout << "NIK: " << current->nik << endl;
            cout << "Keluhan: " << (current->keluhan.empty() ? "Belum Ada Keluhan" : current->keluhan) << endl;
            cout << "Poli: " << (current->poli.empty() ? "Belum Ditugaskan" : current->poli) << endl;
            cout << "Status: " << (current->sudahDitugaskan ? "Sudah Ditugaskan" : "Belum Ditugaskan") << endl;
            cout << "Darurat: " << (current->darurat ? "Ya" : "Tidak") << endl;
            cout << "Hari/Tanggal: " << (current->waktuKunjungan.empty() ? "Belum Ditentukan" : current->waktuKunjungan) << endl;
            cout << "-------------------------------------" << endl;
            found = true;
        }
        current = current->next;  
    }

    if (!found) {
        cout << "Pasien dengan nama " << nama << " tidak ditemukan." << endl;
    }
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
                    cout << "     1. Urutkan Berdasarkan Nomor Secara Ascending       " << endl;
                    cout << "     2. Urutkan Berdasarkan Nama Secara descending       " << endl;
                    cout << "     3. Kembali                                          " << endl;
                    cout << "---------------------------------------------------------" << endl;
                    cout << endl;
                    cout << "Pilih Menu: ";
                    cin >> sort;

                    if (sort == "1"){
                    quickSort(head); 
                        Node* temp = head;
                        cout << "\n==== Sorted Antrian Pasien Berdasarkan No Urut Ascending ====" << endl;
                        while (temp != nullptr) {
                        cout << "Nama: " << temp->namalengkap << ", No Urut: " << temp->noUrut << endl;
                        temp = temp->next;
                    }
                        cout << endl;
                        cout << "Tekan ENTER untuk kembali...";
                        cin.ignore();
                        cin.get();
                    }
                    else if (sort == "2"){
                    shellSort(head);
                        cout << endl;
                        cout << "Tekan ENTER untuk kembali...";
                        cin.ignore();
                        cin.get();
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
                    cout << "     1. Cari Berdasarkan Nomor Riwayat      " << endl;
                    cout << "     2. Cari Berdasarkan Nama Riwayat       " << endl;
                    cout << "     3. Kembali                             " << endl;
                    cout << "--------------------------------------------" << endl;
                    cout << endl;
                    cout << "Pilih Menu: ";
                    cin >> sort;

                    if (sort == "1"){
                    int targetNoUrut;
                    cout << "Masukkan Nomor Urut Pasien: ";
                    cin >> targetNoUrut;

                    int n = countPasien(head); // Get total number of Pasien
                    int result = jumpSearchNoUrut(head, targetNoUrut, n);
                    
                    if (result == -1) {
                        cout << "Pasien dengan Nomor Urut " << targetNoUrut << " tidak ditemukan." << endl;
                    } else {
                        cout << "Pasien ditemukan." << endl;
                    }
                    cout << endl;
                    cout << "Tekan ENTER untuk kembali...";
                    cin.ignore();
                    cin.get();
                    }
                    else if (sort == "2"){
                    string namalengkap;
                    cout << "Masukkan Nama Pasien: ";
                    cin.ignore(); 
                    getline(cin, namalengkap);

                    cariPasienBerdasarkanNama(namalengkap);
                    cout << "Tekan ENTER untuk kembali...";
                    cin.ignore();
                    cin.get();
                    
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

        return 0;
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
