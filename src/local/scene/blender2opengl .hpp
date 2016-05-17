
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

class Model
{
public:
    /** Constructor empty */
    Model();

    void getOBJinfo(std::string fp);
    void extractOBJdata(std::string fp, float positions[][3], float texels[][2] ,float normals[][3] ,int faces[][6] ,std::string* materials, int m);
    void writeH(std::string fp, std::string name);
    void writeCvertices(string fp, string name);
    void writeCpositions(std::string fp, std::string name,  int faces[][6], float positions[][3], int counts[]);
    void writeCtexels(std::string fp, std::string name, int faces[][6], float texels[][2]);
    void writeCnormals(std::string fp, std::string name, int faces[][6], float normals[][3]);
    int getMTLinfo(std::string fp);
    void extractMTLdata(string fp, string* materials, float diffuses[][3], float speculars[][3]);
    void writeCmaterials(string fp, string name, int firsts[], int counts[]);
    void writeCdiffuses(string fp, string name,  float diffuses[][3]);
    void writeCspeculars(string fp, string name,  float speculars[][3]);
    //Mutateurs et Accesseurs
    void setMaterials(string fp);
    int getPositions();
    int getTexels();
    int getNormals();
    int getFaces();
    int getMaterials();


private:

    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
    int materials;
};
