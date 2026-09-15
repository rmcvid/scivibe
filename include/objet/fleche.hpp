#ifndef FLECHE_HPP
#define FLECHE_HPP
#include "glm.hpp"
#include "objet/color.hpp"
#include <vector>


namespace scivibe {
    class Fleche {
        private :
            glm::vec2 debut;
            glm::vec2 fin;
            scivibe::Color couleur;
            float epaisseur {10.0f};
            glm::vec2 dir;
            glm::vec2 perp;

        public:
            Fleche(glm::vec2 debut, glm::vec2 fin, scivibe::Color couleur, float epaisseur = 10.0f)
                : debut(debut), fin(fin), couleur(couleur), epaisseur(epaisseur) {
                    updateDirection();
            }
            void updateDirection(){
                dir = glm::normalize(fin - debut);
                perp = glm::vec2(-dir.y, dir.x);
            }    
            void setDebut(glm::vec2 debut) { 
                this->debut = debut;
                updateDirection();
            }
            void setFin(glm::vec2 fin) { 
                this->fin = fin; 
                updateDirection();
            }
            void setCouleur(scivibe::Color couleur) { this->couleur = couleur; }
            void setEpaisseur(float epaisseur) { this->epaisseur = epaisseur; }
            glm::vec2 getDebut() const { return debut; }
            glm::vec2 getFin() const { return fin; }
            scivibe::Color getCouleur() const { return couleur; }
            float getEpaisseur() const { return epaisseur; }

            std::vector<glm::vec2> getVertices() const {
                float demiEpaisseur = this->epaisseur / 2.0f;
                float longueurTete = this->epaisseur * 3.0f;
                float demiLargeurTete = this->epaisseur * 1.5f;
                glm::vec2 baseTete = this->fin - this->dir * longueurTete;
                
                // Rectangle
                glm::vec2 p1 = this->debut + this->perp * demiEpaisseur;
                glm::vec2 p2 = this->debut - this->perp * demiEpaisseur;
                glm::vec2 p3 = baseTete - this->perp * demiEpaisseur;
                glm::vec2 p4 = baseTete + this->perp * demiEpaisseur;
                
                // Triangle
                glm::vec2 p5 = baseTete + this->perp * demiLargeurTete;
                glm::vec2 p6 = baseTete - this->perp * demiLargeurTete;
                glm::vec2 p7 = this->fin;

                return {p1, p2, p3, p1, p3, p5, p5, p6, p7};
            }

            void draw() const {
                // Code to draw the arrow using OpenGL goes here
                // This would typically involve setting up vertex data for the line and the arrowhead,
                // binding buffers, and issuing draw calls.
            }
    };
}
#endif // FLECHE_HPP