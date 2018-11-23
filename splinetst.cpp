#include "vec2i.h"
#include "vec3.h"
#include "camera.h"
#include <fstream>

float normalized(int i, int k, float t, float *knots){
    if(k == 1){
        if(knots[i] <= t && t < knots[i+1])
            return 1.0;
        else
            return 0.0;
    }
    else{
        float t1 = (t - knots[i])/(knots[i+k-1]-knots[i]);
        float t2 = (knots[i+k]-t)/(knots[i+k]-knots[i+1]);
        return t1*normalized(i, k-1, t, knots) + t2*normalized(i+1, k-1, t, knots);
    }
}

vec3 boor(int n, int k, float t, float *knots, vec3 *cpts){
    vec3 result(0.0, 0.0, 0.0);
    for(int i = 0; i <= n; i++){
        result += (normalized(i, k, t, knots)*cpts[i]);
    }
    return result;
}


vec3 deBoor(int k, int degree, int i, float x, float* knots, vec3 *ctrlPoints)
{   
    if( k == 0)
        return ctrlPoints[i];
    else
    {
        float alpha = (x-knots[i])/(knots[i+degree+1-k]-knots[i]);
        return (deBoor(k-1,degree, i-1, x, knots, ctrlPoints)*(1-alpha )+deBoor(k-1,degree, i, x, knots, ctrlPoints)*alpha );
    }
}

int which_interval(float x, float *knot, int m)
{
    for(int i = 1; i < m-1; i++)
    {
        if(x < knot[i])
            return(i-1);
        else if(x == knot[m-1])
            return(m-1);
    }
    return -1;
}


int main()
{
    vec3 from(35,45,20);
    vec3 at(40,0,-1);
    float FoV = 60;
    float near = 1;
    camera cam(from, at, vec3(0,1,0), FoV, near, 400, 300);
    std::ofstream ofs; 
    ofs.open("./bspline.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << cam.imgWidth << "\" height=\"" << cam.imgHeight << "\">" << std::endl;
    ofs << "    <rect width=\"" << cam.imgWidth << "\" height=\"" << cam.imgHeight << "\" stroke=\"black\" stroke-width=\"0\" fill=\"rgb(120,120,120)\"/>\n" ; 
    

    //number of control points -1
    int n = 6;
    vec3 *cpts = new vec3[n+1];
    //k = order of curve
    int k = 4;
    //m = knot vector size -1
    int m = n + k + 1;
    
    cpts[0] = vec3(20, 270, 10);
    cpts[1] = vec3(60, 40, 10);
    cpts[2] = vec3(120, 130, 10);
    cpts[3] = vec3(185, 180, 10);
    cpts[4] = vec3(260, 90, 10);
    cpts[5] = vec3(345, 140, 10);
    cpts[6] = vec3(385, 220, 10);

    for(int i = 0; i < n+1; i++){
        ofs << "    <circle cx=\"" << cpts[i].x() << "\" cy=\"" << cpts[i].y() <<
        "\" r=\"2.5\" stroke=\"black\" stroke-width=\"1\" fill=\"blue\" />" << std::endl;
    }

    float kt[11] = {0,1,2,3,4,5,6,7,8,9,10};
    //float kt[11] = {0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1};

    int numSegments = 30;
    float t = 0;
    for (int i = 0; i < n; ++i){

        float t = (float)i+0.1;
        //vec3 pt = vec3(0,0,0);
        int j = which_interval(t, kt, m);
        //pt = deBoor(k, k, j, t, kt, cpts);
        vec3 pt = boor(n, k, t, kt, cpts);
        std::cout << pt << std::endl;
        ofs << "    <circle cx=\"" << pt.x() << "\" cy=\"" << pt.y() <<
        "\" r=\"0.8\" stroke=\"black\" stroke-width=\"0.3\" fill=\"red\" />" << std::endl;

    }

    ofs << "</svg>";
    return 0;
}