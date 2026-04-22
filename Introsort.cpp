#include "Introsort.h"
#include <vector>
#include <cmath>
using namespace std;

static void sortWstawianie(vector<Film>& dane, int lewy, int prawy) {

    for (int i = lewy + 1; i <= prawy; ++i) {
        Film klucz = dane[i];
        int j = i - 1;
        while (j >= lewy && dane[j].ocena > klucz.ocena) {
            dane[j + 1] = dane[j];
            --j;
        }
        dane[j + 1] = klucz;
    }
}

//podtablice
static void kopce(vector<Film>& dane, int korzen, int rozmiar, int baza) {

    int najwiekszy = korzen;
    int lewy = 2 * (korzen - baza) + 1 + baza;
    int prawy = 2 * (korzen - baza) + 2 + baza;

    if (lewy  < baza + rozmiar && dane[lewy].ocena  > dane[najwiekszy].ocena) najwiekszy = lewy;
    if (prawy < baza + rozmiar && dane[prawy].ocena > dane[najwiekszy].ocena) najwiekszy = prawy;

    if (najwiekszy != korzen) {
        swap(dane[korzen], dane[najwiekszy]);
        kopce(dane, najwiekszy, rozmiar, baza);
    }
}

static void sortKopcowanie(vector<Film>& dane, int lewy, int prawy) {

    int rozmiar = prawy - lewy + 1;

    for (int i = lewy + rozmiar / 2 - 1; i >= lewy; --i)
        kopce(dane, i, rozmiar, lewy);

    for (int i = prawy; i > lewy; --i) {
        swap(dane[lewy], dane[i]);
        kopce(dane, lewy, i - lewy, lewy);
    }
}


//introsort
static int podzialIntro(vector<Film>& dane, int lewy, int prawy) {

    int srodek = lewy + (prawy - lewy) / 2;
    if (dane[lewy].ocena > dane[srodek].ocena)  swap(dane[lewy], dane[srodek]);
    if (dane[lewy].ocena > dane[prawy].ocena)   swap(dane[lewy], dane[prawy]);
    if (dane[srodek].ocena > dane[prawy].ocena) swap(dane[srodek], dane[prawy]);

    float pivot = dane[srodek].ocena;
    swap(dane[srodek], dane[prawy - 1]);

    int i = lewy, j = prawy - 1;
    while (true) {
        while (dane[++i].ocena < pivot) {}
        while (dane[--j].ocena > pivot) {}
        if (i >= j) break;
        swap(dane[i], dane[j]);
    }
    swap(dane[i], dane[prawy - 1]);
    return i;
}

static void introsortImpl(vector<Film>& dane, int lewy, int prawy, int limitGlebokosci) {

    if (prawy - lewy < 16) {
        sortWstawianie(dane, lewy, prawy);
        return;
    }
    if (limitGlebokosci == 0) {
        sortKopcowanie(dane, lewy, prawy);
        return;
    }

    int pivot = podzialIntro(dane, lewy, prawy);
    introsortImpl(dane, lewy, pivot - 1, limitGlebokosci - 1);
    introsortImpl(dane, pivot + 1, prawy, limitGlebokosci - 1);
}

void introsort(vector<Film>& dane, int lewy, int prawy) {
    if (lewy >= prawy) return;
    int limitGlebokosci = 2 * static_cast<int>(log2(prawy - lewy + 1));
    introsortImpl(dane, lewy, prawy, limitGlebokosci);
}