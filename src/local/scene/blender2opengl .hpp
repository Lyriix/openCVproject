
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include<sstream>
#include <vector>

#include"../../lib/3d/vec3.hpp"
#include"../../lib/3d/vec2.hpp"
using namespace std;

struct Model
{
public:
    /** Constructor empty */
    Model();
    /** Accesors */

    /** Get information of .obj file */
    void getOBJinfo(const std::string& fp);
    /** Extraire les données brut du .obj */
    void extractOBJdata(const std::string& fp, int m);
    /** Obtenir information of .mtl file */
    int getMTLinfo(const std::string& fp);
    /** extraction des données du fichier mtl*/
    void extractMTLdata(const std::string& fp);

    /** Remplir une Map (clé : nom texture ; valeur : structure) */

    int  get_vertex() const;
    int& set_vertex();
    int get_vertices() const;
    int& set_vertices();
    int get_normals() const;
    int& set_normals();
    int get_color() const;
    int& set_color();
    int get_texture_coord() const;
    int& set_texture_coord();
    int get_faces() const;
    int& set_faces();
    int get_nbMaterials() const;
    int& set_nbMaterials();

    std::vector<cpe::vec3> vertices_tab; //coordonnées des vertices
    std::vector<std::vector<float>> texel_tab; //coordonnes des textures
    std::vector<std::vector<int>> faces_tab_vertex;//indice des polygones de vertex
    std::vector<std::vector<int>> faces_tab_texel;  //indice des polygones de texte
    std::vector<std::string> list_mat_tex;

private:


    int vertex; //contient 3 vertices
    int vertices; //3 vertices forment un vertx
    int normals;
    int color;
    int texture_coord;
    int faces;
    int nbMaterials;
};

std::vector<int> split_face_data(std::string const& face_data_str);
void read_vertex_obj(std::stringstream& tokens, Model& mod);
void read_face_obj(std::stringstream& tokens, Model& mod,int mtl);
void read_name_mtl(std::stringstream& tokens, Model& mod);
