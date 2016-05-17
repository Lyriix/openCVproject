#include "obj2gl.hpp"

model::model():vertices(0), positions(0), texels(0), faces(0), materials(0)
{}

void model::getObjInfo(std::string fp)
{
    //Open OBJ File
    std::ifstream in;
    in.open(fp);
    if(! in.is_open())
    {
        std::cout<<"Error openning OBJ File";
        exit(1);
    }

    //Read the OJ File
    std::string line;
    while(std::getline(in,line))
    {
        std::string type = line.substr(0,2);
        if(type.compare("v ")==0) //sommets
            this->positions++;
        if(type.compare("vt")==0) //textures
            this->texels++;
        if(type.compare("f ")==0)//faces : sommet/tex sommet/tex sommet/tex
            this->faces++;
    }
    this->vertices =this->positions * 3;
    std::cout<<this->positions<<"***************"<<std::endl;

    //Close OBJ File
    in.close();
}

void model::extractObjData(std::string fp, float positions[][3], float texels[][2] ,int faces[][6] ,std::string* materials, int m)
{
    //counters
    int p=0, t=0, f=0;
    //Index
    int mtl=0;

    //Open OBJ File
    std::ifstream in;
    in.open(fp);
    if(! in.is_open())
    {
        std::cout<<"Error Oppenning obj file"<<std::endl;
        exit(1);
    }

    //Read the OBJ File
    std::string line;
    while(std::getline(in, line))
    {
        std::string type = line.substr(0,2);
        //Material
        if(type.compare("us")==0)
        {
            //Extraire jeton
            std::string l = "usemtl ";
            std::string material = line.substr(l.size());
            for(int i=0;i<m;i++)
            {
                if(material.compare(materials[i]) == 0)
                    mtl = i;
            }
        }

        //Positions sommets
        if(type.compare("v ")==0)
        {
            //copy line for parsing
            char* l = new char[line.size()+1];
            std::memcpy(l,line.c_str(),line.size()+1);

            //Extraction du jeton
            std::strtok(l," "); //Création des jetons
            for (int i=0; i<3; i++)
                positions[p][i]= std::atof(std::strtok(NULL," ")); //passage jeton suivant
            //Wrap up
            delete[] l;
            p++;
        }
       //Texels , textures
        else if(type.compare("vt") == 0) //texture
        {
            char* l = new char[line.size()+1];
            memcpy(l,line.c_str(),line.size()+1);

            strtok(l, " ");
            for (int i=0; i<2 ; i++)
                texels[t][i] = atof(strtok(NULL, " "));

            delete[] l;
            t++;
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
        }/* */
    }
    //Close the obj file
    in.close();
}

int model::getMtlInfo(std::string fp)
{
    int m=0;
    //Open Mtl file
    std::ifstream in;
    in.open(fp);
    if(!in.is_open())
    {
        std::cout<<"Error oppening Mtl File"<<std::endl;
        exit(1);
    }

    //Read the Mtl file
    std::string line;
    while(std::getline(in,line))
    {
        std::string type = line.substr(0,2);
        if(type.compare("ne") == 0)
            m++;
    }
    //Close MTL File
    in.close();
    return m;
}

void model::extractMtlData(std::string fp, std::string* materials, float diffuses[][3], float speculars[][3])
{
    //Counters
    int m=0,d=0,s=0;
    //Open MTL File
    std::ifstream in;
    in.open(fp);
    if(!in.is_open())
    {
        std::cout<<"Error openning Mtl File"<<std::endl;
        exit(1);
    }
    //Read Mtl file
    std::string line;
    while(std::getline(in,line))
    {
        std::string type=line.substr(0,2);
        //Names
        if(type.compare("ne") == 0)
        {
            //Extract token
            std::string l = "newmtl ";
            materials[m] = line.substr(l.size());
            m++;
        }
        //Diffuses
        else if(type.compare("Kd") == 0)
        {
            //Copy line to parse
            char* l=new char[line.size()+1];
            std::memcpy(l,line.c_str(),line.size()+1);
            //Extract the tokens
            strtok(l," ");
            for(int i=0;i<3;i++)
                diffuses[d][i] = atof(strtok(NULL," "));
            //Wrap up
            delete[] l;
            d++;
        }
        //Speculars
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
    //Close MTL File
    in.close();
}

void model::writeH(std::string fp, std::string name)
{
    //Create the H file
    std::ofstream outH;
    outH.open(fp);
    if(! outH.is_open())
    {
        std::cout<<"Error creating H File"<<std::endl;
        exit(1);
    }
    //Write to H file
    outH<< "// This is a .h file for the model: "<<name<<std::endl;
    outH<< std::endl;
    //Write statistics
    outH << "// Positions: " << this->positions << std::endl;
    outH << "// Texels: " << this->texels << std::endl;
    outH << "// Faces: " << this->faces << std::endl;
    outH << "// Vertices: " << this->vertices << std::endl;
    outH << "// Materials: " << this->materials << std::endl;
    outH << std::endl;

    // Write declarations
    outH << "const int " << name << "Vertices;" << std::endl;
    outH << "const float " << name << "Positions[" << this->vertices << "];" << std::endl;
    outH << "const float " << name << "Texels[" << this->texels << "];" << std::endl;
    outH << std::endl;

    outH << "const int " << name << "Materials;" << std::endl;
    outH << "const int " << name << "Firsts[" << this->materials << "];" << std::endl;
    outH << "const int " << name << "Counts[" << this->materials << "];" << std::endl;
    outH << std::endl;

    outH << "const float " << name << "Diffuses[" << this->materials << "]" << "[" << 3 << "];" << std::endl;
    outH << "const float " << name << "Speculars[" << this->materials << "]" << "[" << 3 << "];" << std::endl;
    outH << std::endl;
}

void model::writeCvertices(std::string fp, std::string name)
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

void model::writeCpositions(std::string fp, std::string name,  int faces[][6], float positions[][3], int counts[])
{
    // Append C file

    std::ofstream outC;
    outC.open(fp, std::ios::app);

    // Positions
    outC << "const float " << name << "Positions[" << this->vertices << "] = " << std::endl;
    outC << "{" << std::endl;
    for(int j=0; j<this->materials; j++)
    {
        counts[j] = 0;

        for(int i=0; i<this->faces; i++)
        {
            if(faces[i][6] == j)
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

void model::writeCtexels(std::string fp, std::string name,  int faces[][6], float texels[][2])
{
    // Append C file
    std::ofstream outC;
    outC.open(fp, std::ios::app);

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

void model::setMaterials(std::string fp)
{
    this->materials = model::getMtlInfo(fp);
}

int model::getPositions()
{
    return this->positions;
}

int model::getTexels()
{
    return this->texels;
}

int model::getFaces()
{
    return this->faces;
}

int model::getMaterials()
{
    return this->materials;
}