#include "camera.h"
#include "nurbs.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

bool readFile(NURBS &nurbs, const std::string &filename){
    
    camera *cam;
    int width, height;
    float fov, near;
    vec3 from;
    vec3 at;
    vec3 up;

    int cpsize;
    int order;

    float *weights;
    int auxWeight = 1;

    float *knots;
    int auxKnot = 1;

    vec3 *cpPoints;
    int auxN = 0;

    std::ifstream file;
    file.open(filename);
    if(!file){
        std::cout << "Arquivo nao encontrado\n";
        return false;
    }

    string str;
    getline(file, str);

    while(file)
    {
        if(str.length() > 0 && str.at(0) != '#')
        {
            std::stringstream stream(str);
            while(stream)
            {
                string next;
                stream >> next;
                if(next == "width"){
                    stream >> width;
                }
                else if(next == "height"){
                    stream >> height;
                }
                else if(next == "from"){
                    float px, py, pz;
                    stream >> px >> py >> pz;
                    from = vec3(px,py,pz);
                }
                else if(next == "at"){
                    float px, py, pz;
                    stream >> px >> py >> pz;
                    at = vec3(px,py,pz);
                }
                else if(next == "up"){
                    float px, py, pz;
                    stream >> px >> py >> pz;
                    up = vec3(px,py,pz);
                }
                else if(next == "FoV"){
                    stream >> fov;
                }
                else if(next == "near"){
                    stream >> near;
                }
                else if(next == "cpsize"){
                    stream >> cpsize;
                }
                else if(next == "order"){
                    stream >> order;
                    knots = new float[(cpsize+1+order)];
                    weights = new float[(cpsize+1)*(cpsize+1)];
                    cpPoints = new vec3[(cpsize+1)*(cpsize+1)];
                }
                if(next == "peso"){
                    for(int i = (cpsize+1)*(auxWeight-1); i < (cpsize+1)*(auxWeight); i++){
                        stream >> weights[i];
                    }
                    auxWeight++;
                }
                if(next == "no"){
                    for(int i = 0; i < cpsize+1+order; i++){
                        stream >> knots[i];
                    }
                }
                if(next == "cp"){
                    float x, y, z;
                    stream >> x >> y >> z;
                    cpPoints[auxN] = vec3(x,y,z);
                    auxN++;
                }
            }
        }
        getline(file, str);
    }

    cam = new camera(from, at, up, fov, near, width, height);
    nurbs = NURBS(cpsize, order, cpPoints, weights, knots);
    nurbs.cam = cam;
    std::cout << "Arquivo lido com sucesso.\n";
    return true;
}

void renderScene(string fileName, NURBS &n, bool drawCPTs, bool drawCurve, bool drawBBox, int segments){

    std::ofstream ofs; 
    ofs.open("./" + fileName + ".svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << n.cam->imgWidth << "\" height=\"" << n.cam->imgHeight << "\">" << std::endl;
    
    ofs << "    <rect width=\"" << n.cam->imgWidth << "\" height=\"" << n.cam->imgHeight << "\" stroke=\"black\" stroke-width=\"0\" fill=\"rgb(150,150,150)\"/>\n" ; 
    if(drawCurve)
        n.render_curve(ofs, segments);
    
    if(drawCPTs)
        n.render_control_points(ofs);

    if(drawBBox)
        n.render_bounding_box(ofs);

    ofs << "</svg>";
}

int main()
{
    NURBS n;
    cout << "Digite o nome do arquivo a ser lido.\nO(s) arquivo(s) testado(s) foram example1.txt e example2.txt\n";
    string file;
    cin >> file;
    cout << endl;
    if(readFile(n, file)){
        cout << "Digite o nome do arquivo a ser criado(ele sera criado apos a cena ser renderizada).\n";
        string fileName;
        cin >> fileName;
        cout << endl;
        cout << "\nPode-se aplicar os comandos disponiveis na NURBS.\n" <<
                 "Os comandos disponiveis sao: renderScene a b c, onde a,b e c sao um inteiro 0 ou 1,\nque indica false ou true, respectivamente\n" << 
                 "para indicar se sera renderizado, respectivamente os pontos de controle, a curva e a bounding box\n, seguido do nivel de detalhamento das malhas geradas.\n" <<
                 "rotX, rotY e rotZ seguido de um float para rotacionar\no NURBS no sentido anti-horario, em graus, com o valor do float\n" <<
                 "evalSurface s t   para printar a superficie avaliada(x,y,z) dado o espaco parametrico (s,t)\n" << 
                 "get_tg s t para printar a tangente no espaco paramametrico (s,t)\n" <<
                 "para encerrar, aplica-se o comando quit.\n\n";

        string str;
        cin >> str;
        do{
            std::stringstream stream(str);
            string next;
            stream >> next;
            if(next == "renderScene" ){
                bool a, b, c;
                int segments;
                cin >> a >> b >> c;
                cin >> segments;
                renderScene(fileName, n, a, b, c, segments);
                std::cout << "Scena renderizada. O arquivo pode ser visto com as modificacoes feitas.\n";
            }
            else if(next == "rotX"){
                float x;
                cin >> x;
                n.rot_x(x);
                cout << "Rotacao realizada\n";
            }
            else if(next == "rotY"){
                float x;
                cin >> x;
                n.rot_y(x);
                cout << "Rotacao realizada\n";
            }
            else if(next == "rotZ"){
                float rz;
                cin >> rz;
                cout << "X: " << rz << endl;
                n.rot_z(rz);
                cout << "Rotacao realizada\n";
            }
            else if(next == "evalSurface"){
                float s, t;
                cin >> s >> t;
                bool valid = false;
                vec3 point = n.eval_surface(n.order, s, t, valid);
                if(valid)
                    cout << "Ponto Avaliado = " << point << endl;
                else
                    cout << "(s,t) estao num intervalo invalido.\n";
            }
            else if(next == "get_tg"){
                float s, t;
                cin >> s >> t;
                bool valid = false;
                cout << "evaluating\n";
                vec3 point = n.get_tg(n.order, s, t, valid);
                if(valid)
                    cout << "Ponto Avaliado = " << point << endl;
                else
                    cout << "(s,t) estao num intervalo invalido.\n";
            }
            cin >> str;
        }while(str != "quit");
    }
    return 0;
}