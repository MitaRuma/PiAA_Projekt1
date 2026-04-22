#include "Bucketsort.h"
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

//sort przez wstawianie w kubelkach
static void sortWKubelku(vector<Film>& kubelek) {

    for (int i = 1; i < static_cast<int>(kubelek.size()); ++i) {
        Film klucz = kubelek[i];
        int j = i - 1;
        while (j >= 0 && kubelek[j].ocena > klucz.ocena) {
            kubelek[j + 1] = kubelek[j];
            --j;
        }
        kubelek[j + 1] = klucz;
    }
}


//bucketsort
void bucketsort(vector<Film>& dane) {

    if (dane.empty()) return;

    float minimum = dane[0].ocena, maksimum = dane[0].ocena;
    for (const auto& f : dane) {
        if (f.ocena < minimum) minimum = f.ocena;
        if (f.ocena > maksimum) maksimum = f.ocena;
    }

    if (maksimum == minimum) return;

    int n = static_cast<int>(dane.size());
    int liczbaKubelkow = static_cast<int>(sqrt(static_cast<double>(n))) + 1;

    vector<vector<Film>> kubelki(liczbaKubelkow);

    //dziel do kubelkow
    float zakres = maksimum - minimum;
    for (const auto& f : dane) {
        int idx = static_cast<int>((f.ocena - minimum) / zakres * (liczbaKubelkow - 1));
        if (idx < 0)               idx = 0;
        if (idx >= liczbaKubelkow) idx = liczbaKubelkow - 1;
        kubelki[idx].push_back(f);
    }

    //sort kadzego kubelka
    int pozycja = 0;
    for (auto& kubelek : kubelki) {
        sortWKubelku(kubelek);
        for (const auto& f : kubelek) {
            dane[pozycja++] = f;
        }
    }
}