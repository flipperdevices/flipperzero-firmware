#ifndef OBSTACLE_H
#define OBSTACLE_H

//Crée l'objet obstacle
//Inspirée de: https://chgi.developpez.com/c/objet/
typedef struct {
    int positionX;
    int positionY;
    int largeur;
    int longueur;
    int positionXInitiale; // Ajout de ces champs
    int positionYInitiale;
    float vitesseInitiale;
    float vitesse;
} Obstacle;

#endif
