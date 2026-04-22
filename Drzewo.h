#pragma once
#include <string>
#include <fstream>
#include "Film.h"

//drzewo czerwono-czarne
enum class Kolor { CZERWONY, CZARNY };

struct Node {
    Film   film;
    Kolor  kolor;
    Node* lewy;
    Node* prawy;
    Node* rodzic;

    explicit Node(const Film& f)
        : film(f), kolor(Kolor::CZERWONY),
        lewy(nullptr), prawy(nullptr), rodzic(nullptr) {}
};

class Drzewo {

public:
    Drzewo();
    ~Drzewo();

    void wstaw(const Film& film);

    //zapisuje po kolei rosnaco
    void zapiszPoKolei(std::ofstream& strumien) const;

private:
    Node* korzen;
    Node* lisc;

    void rotacjaLewo(Node* x);
    void rotacjaPrawo(Node* x);

    void naprawDrzewko(Node* z);

    void poKolei(Node* node, std::ofstream& strumien) const;

    //zwalnianie pamieci
    void zwolnij(Node* node);
};