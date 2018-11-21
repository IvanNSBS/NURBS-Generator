#include <cstdio> 
#include <cstdlib> 
#include <cmath> 
#include "vec3.h"
#include "matrix44.h" 
#include <fstream> 

int factorial(int n){
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

void getPoint(int &n, float &t, vec3 *cp, vec3 &p){
    p = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i <= n; ++i){
        p += ( factorial(n)/ (factorial(i)*factorial(n-i)) ) * std::pow(t,i)*std::pow((1-t),(n-i))*cp[i];
    }
}

/*bool computePixelCoordinates( 
    const vec3 &pWorld, 
    const matrix44 &worldToCamera, 
    const float &b, 
    const float &l, 
    const float &t, 
    const float &r, 
    const float &near, 
    const uint32_t &imageWidth, 
    const uint32_t &imageHeight, 
    vec3 &pRaster) 
{ 
    vec3 pCamera; 
    worldToCamera.multVecMatrix(pWorld, pCamera); 
    vec3 pScreen; 
    pScreen.x = pCamera.x / -pCamera.z * near; 
    pScreen.y = pCamera.y / -pCamera.z * near; 
 
    vec3 pNDC; 
    pNDC.x = (pScreen.x + r) / (2 * r); 
    pNDC.y = (pScreen.y + t) / (2 * t); 
    pRaster.x = (int)(pNDC.x * imageWidth); 
    pRaster.y = (int)((1 - pNDC.y) * imageHeight); 
 
    bool visible = true; 
    if (pScreen.x < l || pScreen.x > r || pScreen.y < b || pScreen.y > t) 
        visible = false; 
 
    return visible; 
}*/

int main(){
    std::ofstream ofs; 
    ofs.open("./pinhole.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << 800 << "\" height=\"" << 400 << "\">" << std::endl;
    ofs << "<rect width=\"800\" height=\"400\" stroke=\"black\" stroke-width=\"0\" fill=\"green\"/>\n" ;
    float aspectratio = 800/400;

    int pnum = 4;
    int n = pnum-1;
    vec3 *cp = new vec3[pnum];
    int numSegments = 30;
    vec3 *rp = new vec3[numSegments]; 

    cp[0] = vec3(20.0*aspectratio, 110, 2);
    cp[1] = vec3(40.0*aspectratio, 180, 2);
    cp[2] = vec3(80.0*aspectratio, 60, 2);
    cp[3] = vec3(110.0*aspectratio, 135, 2);

    //render control points
    for(int i = 0; i < pnum-1; ++i){
        ofs << "    <line x1=\"" << cp[i].x() << "\" y1=\"" << cp[i].y() << "\" x2=\"" << cp[i+1].x() << "\" y2=\"" << cp[i+1].y() << "\" style=\"stroke:rgb(255,0,0);stroke-width:1.5\" />\n"; 
    }

    //create curve points
    for (int j = 0; j <= numSegments; ++j) { 
        float t = j / (float)numSegments; 
        getPoint(n, t, cp, rp[j]); 
    }

    for(int i = 0; i < numSegments; ++i){
        ofs << "    <line x1=\"" << rp[i].x() << "\" y1=\"" << rp[i].y() << "\" x2=\"" << rp[i+1].x() << "\" y2=\"" << rp[i+1].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n"; 
    }
    ofs << "</svg>";
    return 0;
}
