#include "vec2i.h"
#include "vec3.h"
#include "camera.h"
#include "nurbs.h"
#include <fstream>

/*float S(int i, int k, float t, float *knots){
    int index = i-1;
    if(k == 1){
        if( t >= knots[index] && t < knots[index + 1])
            return 1.0;
        else
            return 0.0;
    }
    else{
        float numeratorA = (t - knots[index]);
        float denominatorA = (knots[index+k-1]-knots[index]);
        float numeratorB = (knots[index+k]-t);
        float denominatorB = (knots[index+k]-knots[index+1]);

        float rA = 0;
        float rB = 0;

        if( denominatorA != 0 )
            rA = (numeratorA/denominatorA) * S(i, k-1, t, knots) ;
        
        if( denominatorB != 0)
            rB = (numeratorB/denominatorB) * S(i+1, k-1, t, knots);

        return rA + rB;
    }
}

vec3 deBoor(int n, int k, float t, float *knots, vec3 *cpts, float* weights){
    vec3 result(0.0, 0.0, 0.0);
    float denom = 0;
    for(int i = 1; i <= n+1; i++){
        float w = S(i, k, t, knots);

        vec3 numerator = w * cpts[i-1] * weights[i-1];
        float denominator = w * weights[i-1];

        denom += denominator;
        result += numerator;
    }
    if(denom != 0)
        result /= denom;
    //else
    //    result = vec3(0,0,0);
    //std::cout << "result: " << result << std::endl;
    return result;
}*/

int main()
{
    //vec3 from(10,45,20);
    //vec3 at(15,0,-1);
    vec3 from(-18, 25, 10);
    vec3 at(0, 15, -1);
    float FoV = 40;
    float near = 1;
    camera *cam = new camera(from, at, vec3(0,1,0), FoV, near, 400, 300);
    std::ofstream ofs; 
    ofs.open("./bspline2.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << cam->imgWidth << "\" height=\"" << cam->imgHeight << "\">" << std::endl;
    ofs << "    <rect width=\"" << cam->imgWidth << "\" height=\"" << cam->imgHeight << "\" stroke=\"black\" stroke-width=\"0\" fill=\"rgb(150,150,150)\"/>\n" ; 
       
    vec3 *cpts1 = new vec3[4];
    vec3 *cpts2 = new vec3[4];
    vec3 *cpts3 = new vec3[4];
    vec3 *cpts4 = new vec3[4];
    float kt[7] = {2, 2.1, 2.3, 2.8, 3.4, 3.5, 3.8};
    float weights[4] = {1, 0.6, 0.6, 1};

    cpts2[0] = vec3(2, 11, -7);
    cpts2[1] = vec3(5, 11, -7);
    cpts2[2] = vec3(8, 11, -7);
    cpts2[3] = vec3(11, 11, -7);

    cpts1[0] = vec3(2, 11, -10);
    cpts1[1] = vec3(5, 15, -10);
    cpts1[2] = vec3(8, 15, -10);
    cpts1[3] = vec3(11, 11, -10);

    cpts3[0] = vec3(2, 11, -13);
    cpts3[1] = vec3(5, 15, -13);
    cpts3[2] = vec3(8, 15, -13);
    cpts3[3] = vec3(11, 11, -13);

    cpts4[0] = vec3(2, 11, -16);
    cpts4[1] = vec3(5, 11, -16);
    cpts4[2] = vec3(8, 11, -16);
    cpts4[3] = vec3(11, 11, -16);

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
    NURBS n2(3, 3, cpts2, weights, kt);
    NURBS n3(3, 3, cpts3, weights, kt);
    NURBS n4(3, 3, cpts4, weights, kt);
    n.cam = cam;
    n.render_curve(ofs, 60);
    n.render_control_points(ofs);

    n2.cam = cam;
    n2.render_curve(ofs, 60);
    n2.render_control_points(ofs);

    n3.cam = cam;
    n3.render_curve(ofs, 60);
    n3.render_control_points(ofs);

    n4.cam = cam;
    n4.render_curve(ofs, 60);
    n4.render_control_points(ofs);

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