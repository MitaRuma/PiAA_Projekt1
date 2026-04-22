#include "Drzewo.h"
#include <iomanip>
using namespace std;

Drzewo::Drzewo() {

    lisc = new Node({ "", "", 0.0f });
    lisc->kolor = Kolor::CZARNY;
    lisc->lewy = lisc;
    lisc->prawy = lisc;
    lisc->rodzic = lisc;
    korzen = lisc;
}

Drzewo::~Drzewo() {
    zwolnij(korzen);
    delete lisc;
}

void Drzewo::zwolnij(Node* node) {

    if (node == lisc) return;
    zwolnij(node->lewy);
    zwolnij(node->prawy);
    delete node;
}

//rotacje
void Drzewo::rotacjaLewo(Node* x) {

    Node* y = x->prawy;
    x->prawy = y->lewy;

    if (y->lewy != lisc)
        y->lewy->rodzic = x;

    y->rodzic = x->rodzic;

    if (x->rodzic == lisc)
        korzen = y;
    else if (x == x->rodzic->lewy)
        x->rodzic->lewy = y;
    else
        x->rodzic->prawy = y;

    y->lewy = x;
    x->rodzic = y;
}

void Drzewo::rotacjaPrawo(Node* y) {

    Node* x = y->lewy;
    y->lewy = x->prawy;

    if (x->prawy != lisc)
        x->prawy->rodzic = y;

    x->rodzic = y->rodzic;

    if (y->rodzic == lisc)
        korzen = x;
    else if (y == y->rodzic->lewy)
        y->rodzic->lewy = x;
    else
        y->rodzic->prawy = x;

    x->prawy = y;
    y->rodzic = x;
}

//wstawianie do drzewka
void Drzewo::wstaw(const Film& film) {

    Node* z = new Node(film);
    z->lewy = lisc;
    z->prawy = lisc;
    z->rodzic = lisc;

    Node* poprzednik = lisc;
    Node* aktualny = korzen;

    while (aktualny != lisc) {
        poprzednik = aktualny;
        if (z->film.ocena < aktualny->film.ocena)
            aktualny = aktualny->lewy;
        else
            aktualny = aktualny->prawy;
    }

    z->rodzic = poprzednik;

    if (poprzednik == lisc)
        korzen = z;
    else if (z->film.ocena < poprzednik->film.ocena)
        poprzednik->lewy = z;
    else
        poprzednik->prawy = z;

    //nowy wezel jest czerwony — napraw ewentualne bledy
    naprawDrzewko(z);
}

void Drzewo::naprawDrzewko(Node* z) {

    while (z->rodzic->kolor == Kolor::CZERWONY) {
        if (z->rodzic == z->rodzic->rodzic->lewy) {
            //gdy "ojciec" jest lewym synem "dziadka"
            Node* wujek = z->rodzic->rodzic->prawy;

            if (wujek->kolor == Kolor::CZERWONY) {
                //wujek czerwony - zmien kolor
                z->rodzic->kolor = Kolor::CZARNY;
                wujek->kolor = Kolor::CZARNY;
                z->rodzic->rodzic->kolor = Kolor::CZERWONY;
                z = z->rodzic->rodzic;
            }
            else {
                if (z == z->rodzic->prawy) {
                    //z jest prawym synem — rotacja w lewo
                    z = z->rodzic;
                    rotacjaLewo(z);
                }
                //z jest lewym synem — rotacja w prawo
                z->rodzic->kolor = Kolor::CZARNY;
                z->rodzic->rodzic->kolor = Kolor::CZERWONY;
                rotacjaPrawo(z->rodzic->rodzic);
            }
        }
        else {
            //to samo co wyzej tylko jak ojciec jest po prawej
            Node* wujek = z->rodzic->rodzic->lewy;

            if (wujek->kolor == Kolor::CZERWONY) {

                z->rodzic->kolor = Kolor::CZARNY;
                wujek->kolor = Kolor::CZARNY;
                z->rodzic->rodzic->kolor = Kolor::CZERWONY;
                z = z->rodzic->rodzic;
            }
            else {
                if (z == z->rodzic->lewy) {

                    z = z->rodzic;
                    rotacjaPrawo(z);
                }

                z->rodzic->kolor = Kolor::CZARNY;
                z->rodzic->rodzic->kolor = Kolor::CZERWONY;
                rotacjaLewo(z->rodzic->rodzic);
            }
        }
    }
    korzen->kolor = Kolor::CZARNY; // wlasciwosc 2: korzen zawsze czarny
}

//zapisywanie po kolei
void Drzewo::poKolei(Node* node, ofstream& strumien) const {

    if (node == lisc) return;
    poKolei(node->lewy, strumien);
    strumien << node->film.ocena << "\t"
        << fixed << setprecision(1)
        << node->film.tytul << "\n";
    poKolei(node->prawy, strumien);
}

void Drzewo::zapiszPoKolei(ofstream& strumien) const {
    poKolei(korzen, strumien);
}