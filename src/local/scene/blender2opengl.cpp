
#include "blender2opengl .hpp"


Model::Model(): vertices(0), positions(0), texels(0), normals(0), faces(0), materials(0)
{}

void Model::getOBJinfo(std::string fp)
{
    //   Model model =  Model();

    //Open OBJ File
    std::ifstream inOBJ;
    inOBJ.open(fp);
    if(!inOBJ.is_open())
    {
        std::cout<< "ERROR OPENNING OBJ FILE at 17" <<std::endl;
        exit(1);
    }
    int i=0;
    //Read OBJ File
    std::string line;
   while(std::getline(inOBJ, line))
    while(! inOBJ.eof())
    {
       //  std::string line;
        getline(inOBJ, line);
        std::string type = line.substr(0,2);

        if(type.compare("v ")==0)
        {
            this->positions++;
            // std::cout<<this->positions<<std::endl;
        }
        if(type.compare("vt")==0)
            this->texels++;
        if(type.compare("vn") == 0)
            this->normals++;
        if(type.compare("f ") == 0)
            this->faces++;
    }
    this->vertices = this->faces * 3;

    //Close OBJ File
    inOBJ.close();

    //return model;
}


void Model::extractOBJdata(std::string fp, float positions[][3], float texels[][2] ,float normals[][3] ,int faces[][6] ,std::string* materials, int m)
{
    //Counters
    int p=0;
    int t=0;
    int n=0;
    int f=0;
    //Index
    int mtl=0;

    //Open OBJ File
    std::ifstream inOBJ;
    inOBJ.open(fp);
    if(! inOBJ.is_open())
    {
        std::cout<< "ERROR OPENING OBJ FILE at 60" << std::endl;
        exit(1);
    }

    //Read OBJ File
    //while(! inOBJ.eof())
    std::string line;
    while(std::getline(inOBJ, line))
    {
        getline(inOBJ, line);
        std::string type = line.substr(0,2);
        //Material
        if(type.compare("us") == 0)
        {
            //Extract Token
            std::string l = "usemtl ";
            std::string material = line.substr(l.size());
            for( int i=0; i<m; i++)
            {
                if(material.compare(materials[i]) == 0)
                    mtl=i;
            }
        }

        //Positions
        if(type.compare("v ") == 0) //sommet
        {
            //Copy line for parsing
            char* l = new char[line.size() + 1];
            memcpy(l,line.c_str(),line.size()+1);

            //Extract Tokens
            std::strtok(l, " ");
            for(int i=0; i<3; i++)
                positions[p][i] = std::atof(std::strtok(NULL, " "));

            //Wrap Up
            delete[] l;
            p++;
        }

        //Texels
        else if(type.compare("vt") == 0) //texture
        {
            char* l = new char[line.size()+1];
            memcpy(l,line.c_str(),line.size()+1);

            strtok(l, " ");
            for (int i=0; i<2 ; i++)
                normals[n][i] = atof(strtok(NULL, " "));

            delete[] l;
            n++;
        }

        //Faces
        else if(type.compare("f ") == 0)
        {
            char* l = new char[line.size()+1];
            memcpy(l,line.c_str(),line.size()+1);

            strtok(l, " ");
            for (int i=0; i<6 ; i++)
                faces[f][i] = atof(strtok(NULL, "/"));
            //Append Material
            faces[f][6] = mtl;

            delete[] l;
            f++;
        }
    }
    //Close OBJ File
    inOBJ.close();
}

void Model::writeH(std::string fp, std::string name)
{
    std::cout<<"aaaaaaa"<<std::endl;
    //Create H File
    std::ofstream outH;
    outH.open(fp);
    if(!outH.is_open())
    {
        std::cout<< "ERROR CREATING H FILE at 141"<< std::endl;
        exit(1);
    }

    //Write to H file
    outH<< "// This is a .h file for the model: "<<name<<std::endl;
    outH<< std::endl;

    //Write statistics
    outH << "// Positions: " << this->positions << std::endl;
    outH << "// Texels: " << this->texels << std::endl;
    outH << "// Normals: " << this->normals << std::endl;
    outH << "// Faces: " << this->faces << std::endl;
    outH << "// Vertices: " << this->vertices << std::endl;
    outH << "// Materials: " << this->materials << std::endl;
    outH << std::endl;

    // Write declarations
    outH << "const int " << name << "Vertices;" << std::endl;
    outH << "const float " << name << "Positions[" << this->vertices*3 << "];" << std::endl;
    outH << "const float " << name << "Texels[" << this->vertices*2 << "];" << std::endl;
    outH << "const float " << name << "Normals[" << this->vertices*3 << "];" << std::endl;
    outH << endl;

    outH << "const int " << name << "Materials;" << std::endl;
    outH << "const int " << name << "Firsts[" << this->materials << "];" << std::endl;
    outH << "const int " << name << "Counts[" << this->materials << "];" << std::endl;
    outH << endl;

    outH << "const float " << name << "Diffuses[" << this->materials << "]" << "[" << 3 << "];" << std::endl;
    outH << "const float " << name << "Speculars[" << this->materials << "]" << "[" << 3 << "];" << std::endl;
    outH << std::endl;

    // Close H File
    outH.close();
}

void Model::writeCvertices(std::string fp, std::string name)
{
    //Create C file
    std::ofstream outC;
    outC.open(fp);
    if(!outC.is_open())
    {
        std::cout << "ERROR CREATING C FILE at 185" << std::endl;
        exit(1);
    }

    //Write to C file
    outC << "// This is a .c file for the model: "<<name << std::endl;
    outC <<std::endl;

    // Header
    outC << "#include " << "\"" << name << ".h" << "\"" << std::endl;
    outC << std::endl;

    // Vertices
    outC << "const int " << name << "Vertices = " << this->vertices << ";" << std::endl;
    std::cout<<this->vertices<<"x"<<std::endl;
    outC << std::endl;

    // Close C file
    outC.close();
}

void Model::writeCpositions(std::string fp, std::string name,  int faces[][6], float positions[][3], int counts[])
{
    // Append C file

    std::ofstream outC;
    outC.open(fp, std::ios::app);
    // outC.open(fp);
    // Positions
    outC << "const float " << name << "Positions[" << this->vertices*3 << "] = " << std::endl;
    outC << "{" << std::endl;
    for(int j=0; j<this->materials; j++)
    {
        counts[j] = 0;

        for(int i=0; i<this->faces; i++)
        {
            if(faces[i][9] == j)
            {
                int vA = faces[i][0] - 1;
                int vB = faces[i][2] - 1;
                int vC = faces[i][4] - 1;

                outC << positions[vA][0] << ", " << positions[vA][1] << ", " << positions[vA][2] << ", " << std::endl;
                outC << positions[vB][0] << ", " << positions[vB][1] << ", " << positions[vB][2] << ", " << std::endl;
                outC << positions[vC][0] << ", " << positions[vC][1] << ", " << positions[vC][2] << ", " << std::endl;

                counts[j] += 3;
            }
        }
    }
    outC << "};" << std::endl;
    outC << std::endl;

    // Close C file
    outC.close();
}

void Model::writeCtexels(std::string fp, std::string name,  int faces[][6], float texels[][2])
{
    // Append C file
    std::ofstream outC;
    outC.open(fp, ios::app);

    // Texels
    outC << "const float " << name << "Texels[" << this->vertices*2 << "] = " << std::endl;
    outC << "{" << std::endl;
    for(int j=0; j<this->materials; j++)
    {
        for(int i=0; i<this->faces; i++)
        {
            if(faces[i][6] == j)
            {
                int vtA = faces[i][1] - 1;
                int vtB = faces[i][3] - 1;
                int vtC = faces[i][5] - 1;

                outC << texels[vtA][0] << ", " << texels[vtA][1] << ", " << std::endl;
                outC << texels[vtB][0] << ", " << texels[vtB][1] << ", " << std::endl;
                outC << texels[vtC][0] << ", " << texels[vtC][1] << ", " << std::endl;
            }
        }
    }
    outC << "};" << std::endl;
    outC << std::endl;

    // Close C file
    outC.close();
}

void Model::writeCnormals(std::string fp, std::string name, int faces[][6], float normals[][3])
{
    /*
    // Append C file
    std::ofstream outC;
    outC.open(fp, ios::app);

    // Normals
    outC << "const float " << name << "Normals[" << this->vertices*3 << "] = " << std::endl;
    outC << "{" << std::endl;
    for(int j=0; j<this->materials; j++)
    {
        for(int i=0; i<this->faces; i++)
        {
            if(faces[i][9] == j)
            {
                int vnA = faces[i][2] - 1;
                int vnB = faces[i][5] - 1;
                int vnC = faces[i][8] - 1;

                outC << normals[vnA][0] << ", " << normals[vnA][1] << ", " << normals[vnA][2] << ", " << std::endl;
                outC << normals[vnB][0] << ", " << normals[vnB][1] << ", " << normals[vnB][2] << ", " << std::endl;
                outC << normals[vnC][0] << ", " << normals[vnC][1] << ", " << normals[vnC][2] << ", " << std::endl;
            }
        }
    }
    outC << "};" << std::endl;
    outC << std::endl;

    // Close C file
    outC.close();
    */
}

int Model::getMTLinfo(std::string fp)
{
    int m=0;

    //Open MTL File
    std::ifstream inMTL;
    inMTL.open(fp);
    if(!inMTL.is_open())
    {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }

    // Read MTL file
    while(!inMTL.eof())
    {
        string line;
        getline(inMTL, line);
        string type = line.substr(0,2);

        if(type.compare("ne") == 0)
            m++;
    }

    // Close MTL file
    inMTL.close();

    return m;
}

void Model::extractMTLdata(std::string fp, std::string* materials, float diffuses[][3], float speculars[][3])
{
    // Counters
    int m = 0;
    int d = 0;
    int s = 0;

    // Open MTL file
    std::ifstream inMTL;
    inMTL.open(fp);
    if(!inMTL.is_open())
    {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }

    // Read MTL file
    while(!inMTL.eof())
    {
        std::string line;
        std::getline(inMTL, line);
        std::string type = line.substr(0,2);

        // Names
        if(type.compare("ne") == 0)
        {
            // Extract token
            std::string l = "newmtl ";
            materials[m] = line.substr(l.size());
            m++;
        }

        // Diffuses
        else if(type.compare("Kd") == 0)
        {
            // Copy line for parsing
            char* l = new char[line.size()+1];
            std::memcpy(l, line.c_str(), line.size()+1);

            // Extract tokens
            strtok(l, " ");
            for(int i=0; i<3; i++)
                diffuses[d][i] = atof(strtok(NULL, " "));

            // Wrap up
            delete[] l;
            d++;
        }

        // Speculars
        else if(type.compare("Ks") == 0)
        {
            char* l = new char[line.size()+1];
            std::memcpy(l, line.c_str(), line.size()+1);

            strtok(l, " ");
            for(int i=0; i<3; i++)
                speculars[s][i] = atof(strtok(NULL, " "));

            delete[] l;
            s++;
        }
    }

    // Close MTL file
    inMTL.close();
}

void Model::writeCmaterials(std::string fp, std::string name, int firsts[], int counts[])
{
    // Append C file
    std::ofstream outC;
    outC.open(fp, ios::app);

    // Materials
    outC << "const int " << name << "Materials = " << this->materials << ";" << endl;
    outC << endl;

    // Firsts
    outC << "const int " << name << "Firsts[" << this->materials << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<this->materials; i++)
    {
        if(i == 0)
            firsts[i] = 0;
        else
            firsts[i] = firsts[i-1]+counts[i-1];

        outC << firsts[i] << "," << endl;
    }
    outC << "};" << endl;
    outC << endl;

    // Counts
    outC << "const int " << name << "Counts[" << this->materials << "] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<this->materials; i++)
    {
        outC << counts[i] << "," << endl;
    }
    outC << "};" << endl;
    outC << endl;

    // Close C file
    outC.close();
}

void Model::writeCdiffuses(std::string fp, std::string name, float diffuses[][3])
{
    // Append C file
    std::ofstream outC;
    outC.open(fp, ios::app);

    // Diffuses
    outC << "const float " << name << "Diffuses[" << this->materials << "][3] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<this->materials; i++)
    {
        outC << diffuses[i][0] << ", " << diffuses[i][1] << ", " << diffuses[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;

    // Close C file
    outC.close();
}

void Model::writeCspeculars(std::string fp, std::string name, float speculars[][3])
{
    // Append C file
    std::ofstream outC;
    outC.open(fp, ios::app);

    // Speculars
    outC << "const float " << name << "Speculars[" << this->materials << "][3] = " << endl;
    outC << "{" << endl;
    for(int i=0; i<this->materials; i++)
    {
        outC << speculars[i][0] << ", " << speculars[i][1] << ", " << speculars[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;

    // Close C file
    outC.close();
}

void Model::setMaterials(std::string fp)
{
    materials = Model::getMTLinfo(fp);
}

int Model::getPositions()
{
    return this->positions;
}

int Model::getTexels()
{
    return this->texels;
}

int Model::getNormals()
{
    return this->normals;
}

int Model::getFaces()
{
    return this->faces;
}

int Model::getMaterials()
{
    return this->materials;
}
