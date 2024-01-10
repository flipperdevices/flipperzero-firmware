//Jeu Crosssy Road
//Mikael Fortier
//2023-11-24

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include "crossy_road.h"
#include <stdlib.h>

#define ENOUGH 2

//Créateur d'obstacle
//x, y, largeur, hauteur, xInitiale, yInitiale, vitesseInitiale, vitesse
Obstacle obstacles[] = {
    {19, 0, 7, 17, 19, 0, 1, 1},
    {27, 9, 9, 23, 27, 9, 1, 1},
    {38, 47, 8, 22, 38, 47, 2, 2},
    {62, 8, 6, 15, 62, 8, 1.5, 1.5},
    {55, 17, 5, 16, 55, 17, 3.4, 3.4},
    {72, 51, 11, 41, 72, 51, 2.1, 2.1},
    {85, 7, 7, 24, 85, 7, 1.2, 1.2},
    {94, 3, 7, 29, 94, 3, 2.3, 2.3},
    {103, 32, 7, 22, 103, 32, 3, 3},
    {111, 26, 5, 6, 111, 26, 1.2, 1.2},
    {113, 57, 5, 6, 113, 57, 1.2, 1.2}};

//Déclaration de variables
int nombreObstacles = sizeof(obstacles) / sizeof(Obstacle);
int xActuel = 0;
int yActuel = 29;
int point = 0;
bool finMenu = false;
bool obstacleBouge = false;
bool reset = false;

//Permet de remettre les obstacles à leur position initiale
void ResetObstacle() {
    for(int i = 0; i < nombreObstacles; ++i) {
        obstacles[i].positionX = obstacles[i].positionXInitiale;
        obstacles[i].positionY = obstacles[i].positionYInitiale;
        obstacles[i].vitesse = obstacles[i].vitesseInitiale;
    }
}

//Permet d'augmenter la vitesse des obstacles si elle inférieure au maximum
void AugmentationVitesseObstacle() {
    for(int i = 0; i < nombreObstacles; ++i) {
        //Permet de s'assurer qu'il y ait un maximum de vitesse
        if(obstacles[i].vitesse < 5) {
            obstacles[i].vitesse += 0.5;
        }
    }
}

//Permet d'afficher les obstacles
void AfficherObstacle(Canvas* canvas, Obstacle* obstacle) {
    canvas_draw_box(
        canvas, obstacle->positionX, obstacle->positionY, obstacle->largeur, obstacle->longueur);
}

//Fonction de base dans le template
static void crossy_road_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

//Permet d'affiche la page d'acceuil
static void AfficherTitre(Canvas* canvas, void* ctx) {
    canvas_clear(canvas);
    UNUSED(ctx);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 32, 10, "Crossy Road");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 63, 19, "de");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 32, 31, "Mikael Fortier");
}

//Permet d'affiche la page de fin de partie
static void AfficherFinPartie(Canvas* canvas, void* ctx) {
    int* context = (int*)ctx;

    //Déclaration de la variable qui affiche les points du joueur
    int pointageFinal = context[2];

    //Déclaration d'une varaible en string
    char pointString[ENOUGH];

    //Convertir le int en string
    snprintf(pointString, sizeof(pointString), "%d", pointageFinal);

    //Affiche le menu de fin de partie
    canvas_clear(canvas);
    UNUSED(ctx);
    canvas_draw_line(canvas, 0, 12, 127, 12);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 37, 10, "Game Over");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 43, 42, "Appuyer sur");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 59, 54, "OK");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 29, 61, "pour recommencer");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 45, 27, "Score:");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 76, 27, pointString);
}

//CallBack du timer
static void CallbackDeplacementObstacle(void* context) {
    bool* obstacleBouge = (bool*)context;
    *obstacleBouge = true;
}

//Permet de voir si le joueur a touché un obstacle
bool joueurToucheObstacle(Obstacle* obstacle, int xJoueur, int yJoueur) {
    // Vérifie si la position x-y du joueur est dedans les limites de l'obstacle en x-y
    bool toucheX = (xJoueur < (obstacle->positionX + obstacle->largeur)) &&
                   ((xJoueur + 4) > obstacle->positionX);
    bool toucheY = (yJoueur < (obstacle->positionY + obstacle->longueur)) &&
                   ((yJoueur + 4) > obstacle->positionY);

    // Retourne true si le joueur touche en X et Y
    if(toucheX && toucheY) {
        return true;
    } else {
        return false;
    }
}

//Initialisation du jeux, affichage de l'ui
static void InitialisationJeux(Canvas* canvas, void* ctx) {
    int* context = (int*)ctx;

    //Déclaration des variables de position du joueur
    int xActuel = context[0];
    int yActuel = context[1];

    //Variable en int contenant le pointage
    int point = context[2];

    //Déclaration d'une varaible en string
    char pointString[ENOUGH];

    //Convertir le int en string
    snprintf(pointString, sizeof(pointString), "%d", point);

    //Permet de déplacer les obtsacles en respectant les lim<ites de la carte tout en les affichant
    for(int i = 0; i < nombreObstacles; ++i) {
        obstacles[i].positionY = obstacles[i].positionY + obstacles[i].vitesse;
        AfficherObstacle(canvas, &obstacles[i]);

        //Permet de repositionner les obstacles s'il dépassent les limites de la carte
        if(obstacles[i].positionY >= 63) {
            obstacles[i].positionX = obstacles[i].positionXInitiale;
            obstacles[i].positionY = obstacles[i].longueur - ((obstacles[i].longueur * 2) + 2);
        }
    }

    //Peremet de mettre le bitmap en mode 1 bit
    canvas_set_bitmap_mode(canvas, 1);

    //Permet d'afficher la ligne d'arrivée
    canvas_draw_line(canvas, 120, 0, 120, 63);
    canvas_draw_box(canvas, 121, 0, 3, 4);
    canvas_draw_box(canvas, 124, 4, 4, 4);
    canvas_draw_box(canvas, 121, 8, 3, 4);
    canvas_draw_box(canvas, 124, 12, 5, 4);
    canvas_draw_box(canvas, 121, 16, 3, 4);
    canvas_draw_box(canvas, 124, 20, 4, 4);
    canvas_draw_box(canvas, 121, 24, 3, 4);
    canvas_draw_box(canvas, 124, 28, 4, 4);
    canvas_draw_box(canvas, 121, 32, 3, 4);
    canvas_draw_box(canvas, 124, 36, 4, 4);
    canvas_draw_box(canvas, 121, 40, 3, 4);
    canvas_draw_box(canvas, 124, 44, 4, 4);
    canvas_draw_box(canvas, 121, 48, 3, 4);
    canvas_draw_box(canvas, 124, 52, 4, 4);
    canvas_draw_box(canvas, 121, 56, 3, 4);
    canvas_draw_box(canvas, 124, 60, 4, 4);

    //Affiche la ligne de départ
    canvas_draw_line(canvas, 7, 0, 7, 63);

    //Affiche le pointage
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 9, pointString);

    //Affiche le joueur
    canvas_draw_box(canvas, xActuel, yActuel, 4, 4);
}

//Le main de l'application
int32_t crossy_road() {
    //Ligne présent dans le template, permttent de faire fonctionner divers fonction
    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, AfficherTitre, NULL);
    view_port_input_callback_set(view_port, crossy_road_input_callback, event_queue);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    //Permet de faire bouger les obstacles ave cun délai de 1 seconde
    FuriTimer* TimerDeplacementObstacle =
        furi_timer_alloc(CallbackDeplacementObstacle, FuriTimerTypePeriodic, &obstacleBouge);
    furi_timer_start(TimerDeplacementObstacle, 1000 / portTICK_PERIOD_MS);

    while(true) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        //Permet de quitter l'Application
        if(event.key == InputKeyBack) {
            break;
        }

        //Permet de commencer le jeux, en sortant de la page d'acceuil
        if(event.key == InputKeyOk) {
            finMenu = true;
        }

        //Si on est sorti du menu de départ, le jeu s'initialise
        if(finMenu == true) {
            //Permet d'envoyer les variables au callback
            int context[3] = {xActuel, yActuel, point};
            view_port_draw_callback_set(view_port, InitialisationJeux, context);

            //Permet de voir si le joueur a touché un obstacle, si oui on recommence la partie
            for(int i = 0; i < nombreObstacles; ++i) {
                if(joueurToucheObstacle(&obstacles[i], xActuel, yActuel)) {
                    xActuel = 0;
                    yActuel = 29;
                    point = 0;

                    //Permet de repositionner les obstacles
                    ResetObstacle();
                    view_port_draw_callback_set(view_port, AfficherFinPartie, context);
                    finMenu = false;
                }
            }

            //Permet de déplacer le joueur en respectant les limites de la carte
            if(event.key == InputKeyRight) {
                //Vérifie quele joueur est dans la zone de jeu
                if(xActuel <= 123) {
                    //Déplace le joueur sur l'axe des x positivement
                    xActuel++;
                } else {
                    //Repositionnement du joueur à la limite maximal auquel il peut se déplacer
                    xActuel = 124;
                }
            }

            else if(event.key == InputKeyLeft) {
                //Vérifie quele joueur est dans la zone de jeu
                if(xActuel >= 1) {
                    //Déplace le joueur sur l'axe des x négativement
                    xActuel--;
                } else {
                    //Repositionnement du joueur à la limite maximal auquel il peut se déplacer
                    xActuel = 0;
                }
            }

            else if(event.key == InputKeyUp) {
                //Vérifie quele joueur est dans la zone de jeu
                if(yActuel >= -2) {
                    //Déplace le joueur sur l'axe des y positivement
                    yActuel--;
                } else {
                    //Permet de déplacer le joueur à l'opposé de la carte sur l'axe y
                    yActuel = 62;
                }
            }

            else if(event.key == InputKeyDown) {
                //Vérifie quele joueur est dans la zone de jeu
                if(yActuel <= 62) {
                    //Déplace le joueur sur l'axe des y négativement
                    yActuel++;
                } else {
                    //Permet de déplacer le joueur à l'opposé de la carte sur l'axe y
                    yActuel = -2;
                }
            }

            //Pemet de voir si le joueur a touché la ligne d'arrivée, si oui, on augmente le score d'un point
            if(xActuel >= 119) {
                xActuel = 0;
                yActuel = 29;
                point++;

                AugmentationVitesseObstacle();
            }
        }
    }

    //Fonction de fermeture, permet de libérer les ressources afin d'empêcher l'application de planter
    furi_message_queue_free(event_queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    //N'était pas présent
    furi_timer_free(TimerDeplacementObstacle);

    return 0;
}