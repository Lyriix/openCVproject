#include <iostream>
#include <fstream>
#include <string>
#include <cstring>


class model
{
public:
    /**Empty constructor */
    model();

    void getObjInfo(std::string fp);
    void extractObjData(std::string fp, float positions[][3], float texels[][2] , int faces[][6] ,std::string* materials, int m);
    int getMtlInfo(std::string fp);
    void extractMtlData(std::string fp, std::string* materials,float diffuses[][3], float speculars[][3]);

    void writeH(std::string fp, std::string name);
    void writeCvertices(std::string fp, std::string name);
    void writeCpositions(std::string fp, std::string name,  int faces[][6], float positions[][3], int counts[]);
    void writeCtexels(std::string fp, std::string name,  int faces[][6], float texels[][2]);
    void setMaterials(std::string fp);
    int getPositions();
    int getTexels();
    int getFaces();
    int getMaterials();

private:

    int vertices;
    int positions;
    int texels;
    //int normals;
    int faces;
    int materials;
};
