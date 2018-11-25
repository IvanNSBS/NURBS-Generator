#include "vec3.h"
#include "camera.h"
#include "nurbs.h"
#include <fstream>

int main()
{
    //vec3 from(10,45,20);
    //vec3 at(15,0,-1);
    vec3 from(-28, 40, 10);
    vec3 at(-10, 27, -1);
    float FoV = 30;
    float near = 1.2;
    camera *cam = new camera(from, at, vec3(0,1,0), FoV, near, 400, 300);
    std::ofstream ofs; 
    ofs.open("./bspline2.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << cam->imgWidth << "\" height=\"" << cam->imgHeight << "\">" << std::endl;
    ofs << "    <rect width=\"" << cam->imgWidth << "\" height=\"" << cam->imgHeight << "\" stroke=\"black\" stroke-width=\"0\" fill=\"rgb(150,150,150)\"/>\n" ; 
       
    vec3 *cpts1 = new vec3[16];
    float kt[28] = {2, 2.1, 2.3, 2.8, 3.4, 3.5, 3.8,
                    2, 2.1, 2.3, 2.8, 3.4, 3.5, 3.8,
                    2, 2.1, 2.3, 2.8, 3.4, 3.5, 3.8,
                    2, 2.1, 2.3, 2.8, 3.4, 3.5, 3.8};
                    
    float weights[16] = {1, 0.4, 0.4, 1,
                         1, 0.55, 0.55, 1,
                         1, 0.55, 0.55, 1,
                         1, 0.4, 0.4, 1};

    cpts1[0] = vec3(2, 11, -7);
    cpts1[1] = vec3(5, 11, -7);
    cpts1[2] = vec3(8, 11, -7);
    cpts1[3] = vec3(11, 11, -7);

    cpts1[4] = vec3(2, 11, -10);
    cpts1[5] = vec3(5, 15, -10);
    cpts1[6] = vec3(8, 15, -10);
    cpts1[7] = vec3(11, 11, -10);

    cpts1[8] = vec3(2, 11, -13);
    cpts1[9] = vec3(5, 15, -13);
    cpts1[10] = vec3(8, 15, -13);
    cpts1[11] = vec3(11, 11, -13);

    cpts1[12] = vec3(2, 11, -16);
    cpts1[13] = vec3(5, 11, -16);
    cpts1[14] = vec3(8, 11, -16);
    cpts1[15] = vec3(11, 11, -16);

    /*for(int i = 0; i < 6; ++i)
    {
        matrix44 tr(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -2,-27,10,1);
        matrix44 itr = tr.inverse();
        float sen30 = 1;
        float cos30 = 0;
        matrix44 rot(cos30, 0, -sen30, 0, 
                       0,   1,    0,   0,
                     sen30, 0, cos30, 0, 
                       0,   0,    0,   1);
        matrix44 result = (tr*rot)*itr;
        vec3 aux;
        result.multVecMatrix(cpts[i], aux);
        cpts[i] = aux;
    }*/

    NURBS n(3, 3, cpts1, weights, kt);
    n.cam = cam;
    n.render_curve(ofs, 40);
    //n.render_control_points(ofs);
    //n.render_bounding_box(ofs);
    ofs << "</svg>";

    return 0;
}

    /*cpts[0] = vec3(2, 13, -10);
    cpts[1] = vec3(2, 16, -10);
    cpts[2] = vec3(8, 16, -10);
    cpts[3] = vec3(8, 12, -10);
    cpts[4] = vec3(12, 15, -10);
    cpts[5] = vec3(14, 12, -10);

    cpts[0] = vec3(2, 13, -10);
    cpts[1] = vec3(2, 20, -10);
    cpts[2] = vec3(8, 20, -10);
    cpts[3] = vec3(8, 11, -10);
    cpts[4] = vec3(4, 11, -10);
    cpts[5] = vec3(4, 15, -10);*/

    /*cpts[0] = vec3(2, 27, -10);
    cpts[1] = vec3(6, 4, -10);
    cpts[2] = vec3(12, 13, -10);
    cpts[3] = vec3(18.5, 18, -10);
    cpts[4] = vec3(26, 9, -10);
    cpts[5] = vec3(34.5, 14, -10);
    cpts[6] = vec3(34, 21, -10);*/