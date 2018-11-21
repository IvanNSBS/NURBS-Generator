#include "camera.h"
#include <fstream> 

#define M_PI 3.141592653589793

int factorial(int n){
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

void getPoint(int offset, int n, float &t, vec3 *cp, vec3 &p){
    p = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i <= n; ++i){
        p += ( factorial(n)/ (factorial(i)*factorial(n-i)) ) * std::pow(t,i)*std::pow((1-t),(n-i))*cp[i+offset];
    }
    return;
}

void render_control_points(int min, int max, vec3 *cp, vec2i *rcp, camera &cam, std::ofstream &ofs){
    for(int i = min; i < max; ++i){
        bool visible0 = cam.compute_pixel_coordinates(cp[i], rcp[i]);
        bool visible1 = cam.compute_pixel_coordinates(cp[i+1], rcp[i+1]);
        ofs << "    <line x1=\"" << rcp[i].x() << "\" y1=\"" << rcp[i].y() << "\" x2=\"" << rcp[i+1].x() << "\" y2=\"" << rcp[i+1].y() << "\" style=\"stroke:rgb(255,0,0);stroke-width:1.2\" />\n";
    }
    return;
}

void render_bezier_curve(int nSegments, int n, vec3 *cp, camera &cam, std::ofstream &ofs){
    vec3 *rp = new vec3[nSegments];
    vec2i *rpc = new vec2i[nSegments];
    for (int i = 0; i < nSegments; ++i) { 
        float t0 = i / (float)nSegments; 
        float t1 = (i+1) / (float)nSegments; 
        getPoint(0, n, t0, cp, rp[i]);
        getPoint(0, n, t1, cp, rp[i+1]);
        bool visible = cam.compute_pixel_coordinates(rp[i], rpc[i]);
        bool visible1 = cam.compute_pixel_coordinates(rp[i+1], rpc[i+1]);
        ofs << "    <line x1=\"" << rpc[i].x() << "\" y1=\"" << rpc[i].y() << "\" x2=\"" << rpc[i+1].x() << "\" y2=\"" << rpc[i+1].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n"; 
    }
    return;
}

void render_surface_cp(int &n, vec3 *cp, camera &cam, std::ofstream &ofs){
    int size = (n*n) - 1;
    vec2i *rcp = new vec2i[size + 1];

    for(int i = 0; i < size; i+=n){
        int max = i+3;
        render_control_points(i, max, cp, rcp, cam, ofs);
    }

    int lig = (n*(n-1));
    for(int i = 0; i < lig; i++)
        ofs << "    <line x1=\"" << rcp[i].x() << "\" y1=\"" << rcp[i].y() << "\" x2=\"" << rcp[i+n].x() << "\" y2=\"" << rcp[i+n].y() << "\" style=\"stroke:rgb(255,0,0);stroke-width:1.2\" />\n";
}

void render_bezier_surface(int nSegments, int n, vec3 *cp, camera &cam, std::ofstream &ofs){
    int size = (nSegments+1)*(n+1);

    vec2i *rpc = new vec2i[size];

    int aux = 0;
    for(int j = 0; j <= n; j++)
    {
        vec3 *rp = new vec3[nSegments+1];
        for (int i = 0; i < nSegments; ++i) { 
            float t0 = i / (float)nSegments; 
            float t1 = (i+1) / (float)nSegments; 
            int index = i + (j*(nSegments+1));
            getPoint((n+1)*j, n, t0, cp, rp[i]);
            getPoint((n+1)*j, n, t1, cp, rp[i+1]);
            bool visible = cam.compute_pixel_coordinates(rp[i], rpc[index]);
            bool visible1 = cam.compute_pixel_coordinates(rp[i+1], rpc[index+1]);
            aux++;
            ofs << "    <line x1=\"" << rpc[index].x() << "\" y1=\"" << rpc[index].y() << "\" x2=\"" << rpc[index+1].x() << "\" y2=\"" << rpc[index+1].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n"; 
            //ofs << "    <circle cx=\"" << rpc[index].x() << "\" cy=\"" << rpc[index].y() << "\" r=\"2.5\" stroke=\"black\" stroke-width=\"1\" fill=\"blue\" />" << std::endl;
            //ofs << "    <circle cx=\"" << rpc[index+1].x() << "\" cy=\"" << rpc[index+1].y() << "\" r=\"2.5\" stroke=\"black\" stroke-width=\"1\" fill=\"blue\" />" << std::endl;
        }
    }

    int o = nSegments+1;
    for(int i = 0; i < (nSegments+1)*n; i++)
    {
        ofs << "    <line x1=\"" << rpc[i].x() << "\" y1=\"" << rpc[i].y() << "\" x2=\"" << rpc[i+o].x() << "\" y2=\"" << rpc[i+o].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n";
        ofs << "    <circle cx=\"" << rpc[i].x() << "\" cy=\"" << rpc[i].y() << "\" r=\"2.5\" stroke=\"black\" stroke-width=\"1\" fill=\"blue\" />" << std::endl;
        ofs << "    <circle cx=\"" << rpc[i+o].x() << "\" cy=\"" << rpc[i+o].y() << "\" r=\"2.5\" stroke=\"black\" stroke-width=\"1\" fill=\"blue\" />" << std::endl;
    }
}

int main(){

    vec3 from(35,45,20);
    vec3 at(40,0,-1);
    camera cam(from, at, vec3(0,1,0), 20.0, 1.0, 800, 400);

    std::ofstream ofs; 
    ofs.open("./pinhole.svg"); 
    ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << cam.imgWidth << "\" height=\"" << cam.imgHeight << "\">" << std::endl;
    ofs << "    <rect width=\"" << cam.imgWidth << "\" height=\"" << cam.imgHeight << "\" stroke=\"black\" stroke-width=\"0\" fill=\"rgb(120,120,120)\"/>\n" ; 

    int pnum = 4;
    int pnum2 = pnum*pnum;
    int n = pnum-1;
    int numSegments = 10;
    vec3 *cp = new vec3[pnum];
    vec3 *cp2 = new vec3[pnum];
    vec3 *cp3 = new vec3[pnum];
    vec3 *cp4 = new vec3[pnum];
    vec3 *scp2 = new vec3[pnum2];

    cp[0] = scp2[0] = vec3(20.0, 0, -10);
    cp[1] = scp2[1] = vec3(40.0, 13, -15);
    cp[2] = scp2[2] = vec3(60.0, 12, -13);
    cp[3] = scp2[3] = vec3(80.0, 0, -10);

    cp2[0] = scp2[4] = vec3(20.0, 0, 0);
    cp2[1] = scp2[5] = vec3(40.0, 17, -5);
    cp2[2] = scp2[6] = vec3(58.0, 15, -3);
    cp2[3] = scp2[7] = vec3(75.0, 0, 2);

    cp3[0] = scp2[8] = vec3(20.0, 0, 10);
    cp3[1] = scp2[9] = vec3(40.0, 16, 5);
    cp3[2] = scp2[10] = vec3(57.0, 14.5, 7);
    cp3[3] = scp2[11] = vec3(73.0, 0, 13);

    cp4[0] = scp2[12] = vec3(20.0, 0, 20);
    cp4[1] = scp2[13] = vec3(40.0, 3, 15);
    cp4[2] = scp2[14] = vec3(60.0, 1.5, 17);
    cp4[3] = scp2[15] = vec3(75.0, 0, 23);
    
    render_surface_cp(pnum, scp2, cam, ofs);
    render_bezier_surface(numSegments, n, scp2, cam, ofs);

    ofs << "</svg>";
    return 0;
}
