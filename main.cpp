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

void render_bezier_curve(int numSegments, int n, vec3 *cp, camera &cam, std::ofstream &ofs){
    vec3 *rp = new vec3[numSegments];
    vec2i *rpc = new vec2i[numSegments];
    for (int i = 0; i < numSegments; ++i) { 
        float t0 = i / (float)numSegments; 
        float t1 = (i+1) / (float)numSegments; 
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

    int lig = (n*n) - n;
    for(int i = 0; i < lig; i++)
        ofs << "    <line x1=\"" << rcp[i].x() << "\" y1=\"" << rcp[i].y() << "\" x2=\"" << rcp[i+n].x() << "\" y2=\"" << rcp[i+n].y() << "\" style=\"stroke:rgb(255,0,0);stroke-width:1.2\" />\n";
}

void render_surface(int numSegments, int n, vec3 *cp, camera &cam, std::ofstream &ofs){
    int size = numSegments*(n+1);

    vec2i *rpc = new vec2i[size];

        vec3 *rp = new vec3[numSegments];
    for(int j = 0; j <= n; j++)
    {
        for (int i = 0; i < numSegments; ++i) { 
            float t0 = i / (float)numSegments; 
            float t1 = (i+1) / (float)numSegments; 
            int index = i*(j+1);
            getPoint((n+1)*j, n, t0, cp, rp[i]);
            getPoint((n+1)*j, n, t1, cp, rp[i+1]);
            bool visible = cam.compute_pixel_coordinates(rp[i], rpc[index]);
            bool visible1 = cam.compute_pixel_coordinates(rp[i+1], rpc[index+1]);
            ofs << "    <line x1=\"" << rpc[index].x() << "\" y1=\"" << rpc[index].y() << "\" x2=\"" << rpc[index+1].x() << "\" y2=\"" << rpc[index+1].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n"; 
        }
    }

    int lig = (numSegments*numSegments) - numSegments;
    int i = 0;
    int o = 1;
    return;
    //for(int i = 0; i < lig; i++)
    //    ofs << "    <line x1=\"" << rpc[i].x() << "\" y1=\"" << rpc[i].y() << "\" x2=\"" << rpc[i+o].x() << "\" y2=\"" << rpc[i+o].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n";
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
    int n = pnum-1;
    int numSegments = 10;
    vec3 *cp = new vec3[pnum];
    vec3 *cp2 = new vec3[pnum];
    vec3 *cp3 = new vec3[pnum];
    vec3 *cp4 = new vec3[pnum];
    vec3 *scp2 = new vec3[pnum*pnum];

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
    
    vec2i *rpc = new vec2i[numSegments];
    render_surface_cp(pnum, scp2, cam, ofs);
    render_surface(numSegments, n, scp2, cam, ofs);


    /*render_bezier_curve( numSegments, n, cp, cam, ofs);
    render_bezier_curve( numSegments, n, cp2, cam, ofs);
    render_bezier_curve( numSegments, n, cp3, cam, ofs);
    render_bezier_curve( numSegments, n, cp4, cam, ofs);*/

    /*vec3 *rp = new vec3[numSegments];
    //create curve points
    for (int i = 0; i <= numSegments; ++i) { 
        float t = i / (float)numSegments; 
        getPoint(n, t, cp, rp[i]);
        bool visible = cam.compute_pixel_coordinates(rp[i], rpc[i]);
    }

    for(int i = 0; i < numSegments; ++i){
        ofs << "    <line x1=\"" << rpc[i].x() << "\" y1=\"" << rpc[i].y() << "\" x2=\"" << rpc[i+1].x() << "\" y2=\"" << rpc[i+1].y() << "\" style=\"stroke:rgb(0,0,0);stroke-width:1.2\" />\n"; 
    }*/
    //render_surface_cp(scp, 4, n, cam, ofs);

    //render control points

    ofs << "</svg>";
    return 0;
}
