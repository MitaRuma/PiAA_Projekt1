#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <unordered_map>

#include "Film.h"
#include "Quicksort.h"
#include "Introsort.h"
#include "Bucketsort.h"
#include "Drzewo.h"

using namespace std;

//wczytywanie tytulow
//plik z tytulami filmow jest ogromny, wiec trzeba bylo jak najbardziej zoptymalizowac proces wczytywania danych
//m.in. :
//wczytywany jest caly plik naraz (rdbuf) zamiast getline() pare milionow razy
//parsowanie po char* zamiast stringstream
//po optymalizacji czas ladowania tego pliku spadl z ok. 4 minuty na ok. 30 sekund
unordered_map<string, string> wczytajTytuly(const string& nazwaPliku, ostream& tabelka) {

    ifstream plik(nazwaPliku);
    if (!plik.is_open()) {
        cerr << "Nie znaleziono pliku z nazwami!!!\n";
        exit(1);
    }

    unordered_map<string, string> tytuly;
    tytuly.reserve(1 << 21);
    tytuly.max_load_factor(0.25f);

    string linia;
    getline(plik, linia);

    ostringstream bufor;
    bufor << plik.rdbuf();
    string zawartosc = bufor.str();

    auto start = chrono::high_resolution_clock::now();

    const char* ptr = zawartosc.c_str();
    const char* koniecBufora = ptr + zawartosc.size();

    auto znajdzTabLubNL = [](const char* p, const char* koniec) {
        while (p < koniec && *p != '\t' && *p != '\n' && *p != '\r') ++p;
        return p;
        };

    auto przeskoczPole = [&](const char*& p) {
        p = znajdzTabLubNL(p, koniecBufora);
        if (p < koniecBufora && *p == '\t') ++p;
        };

    while (ptr < koniecBufora) {

        const char* poczId = ptr;
        ptr = znajdzTabLubNL(ptr, koniecBufora);
        string id(poczId, ptr);
        if (ptr < koniecBufora && *ptr == '\t') ++ptr;

        przeskoczPole(ptr);

        const char* poczTytul = ptr;
        ptr = znajdzTabLubNL(ptr, koniecBufora);
        string tytul(poczTytul, ptr);

        while (ptr < koniecBufora && *ptr != '\n') ++ptr;
        if (ptr < koniecBufora) ++ptr;

        if (!id.empty() && !tytul.empty())
            tytuly.emplace(move(id), move(tytul));
    }

    auto koniec = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(koniec - start).count();

    tabelka << "Wczytano " << tytuly.size() << " tytulow w " << ms << " ms\n";

    return tytuly;
}

//wczytywanie pliku z ocenami
vector<Film> wczytajDane(const string& nazwaPliku, const unordered_map<string, string>& tytuly, ostream& tabelka) {

    ifstream plik(nazwaPliku);
    if (!plik.is_open()) {
        cerr << "Nie znaleziono pliku z ocenami!!!\n";
        exit(1);
    }

    vector<Film> wynik;
    wynik.reserve(1500000);

    string linia;
    getline(plik, linia);

    auto start = chrono::high_resolution_clock::now();

    while (getline(plik, linia)) {
        if (linia.empty()) continue;

        istringstream ss(linia);
        string id, ocenaStr;

        if (!getline(ss, id, '\t')) continue;
        if (!getline(ss, ocenaStr, '\t')) continue;

        if (ocenaStr.empty() || ocenaStr == "\\N") continue;

        float ocena = 0.0f;
        try {
            ocena = stof(ocenaStr);
        }
        catch (...) {
            continue;
        }

        auto it = tytuly.find(id);
        string tytul = (it != tytuly.end()) ? it->second : id;

        wynik.push_back({ id, move(tytul), ocena });
    }

    auto koniec = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(koniec - start).count();

    tabelka << "Wczytano " << wynik.size() << " ocen w " << ms << " ms\n\n";

    return wynik;
}

//wrzuca wszystkie filmy do drzewa czerwono-czarnego i zapisuje w pliku rosnaco
void zapiszPrzezDrzewo(const vector<Film>& dane, const string& nazwaPliku) {

    Drzewo drzewo;
    for (const auto& f : dane)
        drzewo.wstaw(f);

    ofstream plik(nazwaPliku);
    drzewo.zapiszPoKolei(plik);
}

double srednia(const vector<Film>& dane) {

    double suma = 0;
    for (const auto& f : dane) suma += f.ocena;
    return suma / dane.size();
}

double mediana(const vector<Film>& dane) {

    int n = static_cast<int>(dane.size());
    if (n % 2 == 0)
        return (dane[n / 2 - 1].ocena + dane[n / 2].ocena) / 2.0;
    return dane[n / 2].ocena;
}

//pomiar czasu
struct WynikPomiaru {
    double czasMs;
    double srednia;
    double mediana;
};

using FunkcjaSortujaca = void(*)(vector<Film>&);

//wrappery bo qsort i introsort wymagaja indeksow lewego/prawego
void quicksortWrapper(vector<Film>& v) {
    if (!v.empty()) quicksort(v, 0, static_cast<int>(v.size()) - 1);
}
void introsortWrapper(vector<Film>& v) {
    if (!v.empty()) introsort(v, 0, static_cast<int>(v.size()) - 1);
}

WynikPomiaru zmierzCzas(const vector<Film>& zrodlo, size_t n, FunkcjaSortujaca fn) {

    size_t liczba = min(n, zrodlo.size());
    vector<Film> dane(zrodlo.begin(), zrodlo.begin() + liczba);

    auto start = chrono::high_resolution_clock::now();
    fn(dane);
    auto koniec = chrono::high_resolution_clock::now();

    double ms = chrono::duration<double, milli>(koniec - start).count();
    return { ms, ::srednia(dane), ::mediana(dane) };
}

// MAIN
int main() {

    const string PLIK_OCENY = "title.ratings.tsv/data.tsv";
    const string PLIK_TYTULY = "title.basics.tsv/data.tsv";
    const string PLIK_POSORTOWANY = "posortowane_filmy.tsv";

    ostream& tabelka = cout;

    auto tytuly = wczytajTytuly(PLIK_TYTULY, tabelka);

    //laczenie tytulow z ocenami
    vector<Film> wszystkie = wczytajDane(PLIK_OCENY, tytuly, tabelka);

    vector<size_t> rozmiary = { 10000, 100000, 500000, 1000000, wszystkie.size() };

    struct Algorytm {
        const char* nazwa;
        FunkcjaSortujaca fn;
    };

    vector<Algorytm> algorytmy = {
        {"Quicksort",  quicksortWrapper},
        {"Introsort",  introsortWrapper},
        {"Bucketsort", bucketsort},
    };

    //tabelka
    tabelka << left
        << setw(14) << "Algorytm"
        << setw(12) << "N"
        << setw(14) << "Czas [ms]"
        << setw(10) << "Srednia"
        << setw(10) << "Mediana"
        << "\n";
    tabelka << string(60, '-') << "\n";

    for (const auto& alg : algorytmy) {
        for (size_t r : rozmiary) {
            size_t liczba = min(r, wszystkie.size());
            if (liczba == 0) continue;

            WynikPomiaru wynik = zmierzCzas(wszystkie, liczba, alg.fn);

            tabelka << left << setw(14) << alg.nazwa
                << setw(12) << liczba
                << fixed << setprecision(2)
                << setw(14) << wynik.czasMs
                << setw(10) << wynik.srednia
                << setw(10) << wynik.mediana
                << "\n";
        }
        tabelka << "\n";
    }

    //posortowane filmy do drzewa
    auto start = chrono::high_resolution_clock::now();
    zapiszPrzezDrzewo(wszystkie, PLIK_POSORTOWANY);
    auto koniec = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(koniec - start).count();

    tabelka << "Posortowane dane zapisano do: " << PLIK_POSORTOWANY << " w czasie" << ms <<" ms (drzewo binarne)\n";

    return 0;
}
