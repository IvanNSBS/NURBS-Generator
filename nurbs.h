#ifndef NURBSH
#define NURBSH

#include "vec3.h"
#include "camera.h"
#include <fstream>
#include <vector>
#include <limits>
#include <math.h>

#define MIN 0
#define MAX 1
#define PI 3.14159265

static const float FMAX = std::numeric_limits<float>::max();

struct surface{
    vec3 point;
    vec3 normal;
};

class NURBS
{
public: 
    int n, order, m;
    vec3 *controlPoints;
    float *weights;
    float *knots;
    camera *cam;

    vec3 min_max[2];

    NURBS(const int &nn = 0, const int &ord = 0, 
          vec3 *cps = 0, float *w = 0, float *kts = 0):
          n(nn), order(ord), m(nn + ord + 1), controlPoints(cps), weights(w), knots(kts)
    {
        min_max[MIN] = vec3(FMAX, FMAX, FMAX);
        min_max[MAX] = vec3(-9999999, -9999999, -9999999);

        if(cps == 0)
            controlPoints = new vec3[nn+1];

        if(kts == 0){
            knots = new float[m];
            for(int i = 0; i < m; i++){
                knots[i] = ((i/(float)m)*((nn+1) - (ord-1))) + (ord-1);
            }
        }
        if(w == 0){
            weights = new float[nn+1];
            for(int i = 0; i < n+1; i++)
                weights[i] = 1;
        }
        
        for(int i = 0; i <= n; i++)
            for(int j = 0; j <= n; j++)
                eval_min_max(controlPoints[(i*(n+1))+j]);
    }

    void eval_min_max(vec3 &p){

        if(p.x() <= min_max[MIN].x())
            min_max[MIN][0] = p.x();

        if(p.x() > min_max[MAX].x())
            min_max[MAX][0] = p.x();

        if(p.y() <= min_max[MIN].y())
            min_max[MIN][1] = p.y();

        if(p.y() > min_max[MAX].y())
            min_max[MAX][1] = p.y();

        if(p.z() <= min_max[MIN].z())
            min_max[MIN][2] = p.z();

        if(p.z() > min_max[MAX].z())
            min_max[MAX][2] = p.z();

        //std::cout << "min-> " << min_max[MIN] << " :::  max->" << min_max[MAX] << std::endl;
    }

    vec3 get_box_center(){
        vec3 a = (min_max[MAX]+min_max[MIN])/2.0;
        vec2f b;
        cam->compute_pixel_coordinates(a, b);
        return (min_max[MAX]+min_max[MIN])/2.0;
    }

    bool inside_box(vec3 &p){
        return( p.x() <= min_max[MAX].x() && p.x() >= min_max[MIN].x() && 
                p.y() <= min_max[MAX].y() && p.y() >= min_max[MIN].y() &&
                p.z() <= min_max[MAX].z() && p.z() >= min_max[MIN].z());
    }

    float S(int i, int k, float t){
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

            if( denominatorA != 0 ){
                rA = (numeratorA/denominatorA) * S(i, k-1, t) ;
            }
            
            if( denominatorB != 0){
                rB = (numeratorB/denominatorB) * S(i+1, k-1, t);
            }

            return rA + rB;
        }
    }

    float d_S(int i, int k, float t){
        int index = i-1;
        if(k == 1){
            if( t >= knots[index] && t < knots[index + 1])
                return 1.0;
            else
                return 0.0;
        }
        else{
            float numeratorA = (k);
            float denominatorA = (knots[index+k-1]-knots[index]);
            float numeratorB = (k);
            float denominatorB = (knots[index+k]-knots[index+1]);

            float rA = 0;
            float rB = 0;

            if( denominatorA != 0 ){
                rA = (numeratorA/denominatorA) * d_S(i, k-1, t) ;
            }
            
            if( denominatorB != 0){
                rB = (numeratorB/denominatorB) * d_S(i+1, k-1, t);
            }

            return rA - rB;
        }
    }

    vec3 get_tg_s(int k, float s, float t){
        vec3 result(0.0, 0.0, 0.0);
        for(int i = 1; i <= n+1; i++)
        {
            for(int j = 1; j <= n+1; j++)
            {
                float w1 = S(j, k, t);
                float w2 = d_S(i, k, s);
                vec3 numerator = w2 * w1 * controlPoints[j-1 + ((n+1)*(i-1)) ];
                result += numerator;
            }
        }
        return result;
    }

    vec3 get_tg_t(int k, float s, float t){
        vec3 result(0.0, 0.0, 0.0);
        for(int i = 1; i <= n+1; i++)
        {
            for(int j = 1; j <= n+1; j++)
            {
                float w1 = d_S(j, k, t);
                float w2 = S(i, k, s);
                vec3 numerator = w2 * w1 * controlPoints[j-1 + ((n+1)*(i-1)) ];
                result += numerator;
            }
        }
        return result;
    }

    vec3 eval_surface(int k, float s, float t){
        vec3 result(0.0, 0.0, 0.0);
        float denom = 0;

        for(int i = 1; i <= n+1; i++)
        {
            for(int j = 1; j <= n+1; j++)
            {
                float w1 = S(j, k, t);
                float w2 = S(i, k, s);
                vec3 numerator = w2 * w1 * controlPoints[j-1 + ((n+1)*(i-1)) ] * weights[j-1 + ((n+1)*(i-1))];
                float denominator = w2 * w1 * weights[j-1 + ((n+1)*(i-1))];
                denom += denominator;
                result += numerator;
            }
        }
        result /= denom;

        return result;
    }
 
    void render_control_points(std::ofstream &ofs){ 
        for(int i = 0; i < n+1; i++)
        {
            for(int j = 0; j < n; j++)
            {
                vec2f praster1;
                vec2f praster2;
                eval_min_max(controlPoints[(i*(n+1))+j]);
                eval_min_max(controlPoints[(i*(n+1))+j+1]);
                cam->compute_pixel_coordinates(controlPoints[(i*(n+1))+j], praster1);
                cam->compute_pixel_coordinates(controlPoints[(i*(n+1))+j+1], praster2);
                ofs << "    <rect x=\"" << praster1.x()-2 << "\" y=\"" << praster1.y()-2 << "\" width=\"4\" height=\"4\" style=\"fill:white;stroke:pink;stroke-width:0;opacity:1\" />\n";
                ofs << "    <rect x=\"" << praster2.x()-2 << "\" y=\"" << praster2.y()-2 << "\" width=\"4\" height=\"4\" style=\"fill:white;stroke:pink;stroke-width:0;opacity:1\" />\n";
                ofs << "    <line x1=\"" << praster1.x() << "\" y1=\"" << praster1.y() << "\" x2=\"" << praster2.x() << "\" y2=\"" << praster2.y() << "\" style=\"stroke:rgb(255,255,255);stroke-width:1.2\" />\n"; 
                if(i < n)
                {
                    vec2f praster3; 
                    vec2f praster4; 
                    cam->compute_pixel_coordinates(controlPoints[((i+1)*(n+1))+j], praster3);
                    cam->compute_pixel_coordinates(controlPoints[((i+1)*(n+1))+j+1], praster4);
                    ofs << "    <line x1=\"" << praster1.x() << "\" y1=\"" << praster1.y() << "\" x2=\"" << praster3.x() << "\" y2=\"" << praster3.y() << "\" style=\"stroke:rgb(255,255,255);stroke-width:1.2\" />\n"; 
                    ofs << "    <line x1=\"" << praster2.x() << "\" y1=\"" << praster2.y() << "\" x2=\"" << praster4.x() << "\" y2=\"" << praster4.y() << "\" style=\"stroke:rgb(255,255,255);stroke-width:1.2\" />\n"; 
                }
            }
        }
    }

    void render_curve(std::ofstream &ofs, int numSegments){
        for(int i = 0; i < numSegments - 1; i++)
        {
            float t = (i/(float)numSegments)*((n+1) - (order-1)) + (order-1) + 0.03;
            float t1 = ((i+1)/(float)numSegments)*((n+1) - (order-1)) + (order-1) + 0.03;
            
            for (int j = 0; j < numSegments; ++j)
            {
                float s1 = (j/(float)numSegments)*((n+1) - (order-1)) + (order-1) + 0.03;
                float s2 = ((j+1)/(float)numSegments)*((n+1) - (order-1)) + (order-1) + 0.03;

                if( (s1 <= knots[m-1] && s2 <= knots[m-1] && t <= knots[m-1]) )
                {
                    vec3 pt1 = eval_surface(order, t, s1);
                    vec3 pt2 = eval_surface(order, t, s2);
                    vec2f pr1;
                    vec2f pr2;
                    cam->compute_pixel_coordinates(pt1, pr1);
                    cam->compute_pixel_coordinates(pt2, pr2);
                    //ofs << "    <circle cx=\"" << pr1.x() << "\" cy=\"" << pr1.y() << "\" r=\"0.8\" stroke=\"black\" stroke-width=\"0.3\" fill=\"red\" />" << std::endl;
                    ofs << "    <line x1=\"" << pr1.x() << "\" y1=\"" << pr1.y() << "\" x2=\"" << pr2.x() << "\" y2=\"" << pr2.y() << "\" style=\"stroke:rgb(0,25,115);stroke-width:0.8\" />\n"; 

                    if(t1 <= knots[m-1])
                    {
                        vec3 pn1 = eval_surface(order, t1, s1);
                        vec3 pn2 = eval_surface(order, t1, s2);
                        vec2f prn1;
                        vec2f prn2;
                        cam->compute_pixel_coordinates(pn1, prn1);
                        cam->compute_pixel_coordinates(pn2, prn2);
                        ofs << "    <line x1=\"" << pr1.x() << "\" y1=\"" << pr1.y() << "\" x2=\"" << prn1.x() << "\" y2=\"" << prn1.y() << "\" style=\"stroke:rgb(0,25,115);stroke-width:0.8\" />\n"; 
                        ofs << "    <line x1=\"" << pr2.x() << "\" y1=\"" << pr2.y() << "\" x2=\"" << prn2.x() << "\" y2=\"" << prn2.y() << "\" style=\"stroke:rgb(0,25,115);stroke-width:0.8\" />\n"; 
                    }
                }
            }
        }     
    }

    void render_bounding_box(std::ofstream &ofs){
        //compute x
        /*min_max[MIN] = vec3(FMAX, FMAX, FMAX);
        min_max[MAX] = vec3(-9999999, -9999999, -9999999);
        for(int i = 0; i < (n+1)*(n+1); i++){
            eval_min_max(controlPoints[i]);
        }*/

        vec2f pZmin1;
        vec2f pZmin2;
        cam->compute_pixel_coordinates(vec3(min_max[MIN].x(), min_max[MIN].y(), min_max[MIN].z()), pZmin1);
        cam->compute_pixel_coordinates(vec3(min_max[MIN].x(), min_max[MIN].y(), min_max[MAX].z()), pZmin2);
        ofs << "    <line x1=\"" << pZmin1.x() << "\" y1=\"" << pZmin1.y() << "\" x2=\"" << pZmin2.x() << "\" y2=\"" << pZmin2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
        vec2f pZmax1;
        vec2f pZmax2;
        cam->compute_pixel_coordinates(vec3(min_max[MIN].x(), min_max[MAX].y(), min_max[MIN].z()), pZmax1);
        cam->compute_pixel_coordinates(vec3(min_max[MIN].x(), min_max[MAX].y(), min_max[MAX].z()), pZmax2);
        ofs << "    <line x1=\"" << pZmax1.x() << "\" y1=\"" << pZmax1.y() << "\" x2=\"" << pZmax2.x() << "\" y2=\"" << pZmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
        ofs << "    <line x1=\"" << pZmin1.x() << "\" y1=\"" << pZmin1.y() << "\" x2=\"" << pZmax1.x() << "\" y2=\"" << pZmax1.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        ofs << "    <line x1=\"" << pZmin2.x() << "\" y1=\"" << pZmin2.y() << "\" x2=\"" << pZmax2.x() << "\" y2=\"" << pZmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 

        vec2f pXmin1;
        vec2f pXmin2;
        cam->compute_pixel_coordinates(vec3(min_max[MIN].x(), min_max[MIN].y(), min_max[MIN].z()), pXmin1);
        cam->compute_pixel_coordinates(vec3(min_max[MAX].x(), min_max[MIN].y(), min_max[MIN].z()), pXmin2);
        ofs << "    <line x1=\"" << pXmin1.x() << "\" y1=\"" << pXmin1.y() << "\" x2=\"" << pXmin2.x() << "\" y2=\"" << pXmin2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
        vec2f pXmax1;
        vec2f pXmax2;
        cam->compute_pixel_coordinates(vec3(min_max[MIN].x(), min_max[MAX].y(), min_max[MIN].z()), pXmax1);
        cam->compute_pixel_coordinates(vec3(min_max[MAX].x(), min_max[MAX].y(), min_max[MIN].z()), pXmax2);
        ofs << "    <line x1=\"" << pXmax1.x() << "\" y1=\"" << pXmax1.y() << "\" x2=\"" << pXmax2.x() << "\" y2=\"" << pXmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
        ofs << "    <line x1=\"" << pXmin2.x() << "\" y1=\"" << pXmin2.y() << "\" x2=\"" << pXmax2.x() << "\" y2=\"" << pXmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
        vec2f pXZmax1;
        vec2f pXZmax2;
        cam->compute_pixel_coordinates(vec3(min_max[MAX].x(), min_max[MIN].y(), min_max[MAX].z()), pXZmax1);
        cam->compute_pixel_coordinates(vec3(min_max[MAX].x(), min_max[MAX].y(), min_max[MAX].z()), pXZmax2);
        ofs << "    <line x1=\"" << pZmin2.x() << "\" y1=\"" << pZmin2.y() << "\" x2=\"" << pXZmax1.x() << "\" y2=\"" << pXZmax1.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        ofs << "    <line x1=\"" << pZmax2.x() << "\" y1=\"" << pZmax2.y() << "\" x2=\"" << pXZmax2.x() << "\" y2=\"" << pXZmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
        ofs << "    <line x1=\"" << pXZmax1.x() << "\" y1=\"" << pXZmax1.y() << "\" x2=\"" << pXZmax2.x() << "\" y2=\"" << pXZmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 

        vec2f pYmax1;
        vec2f pYmax2;
        cam->compute_pixel_coordinates(vec3(min_max[MAX].x(), min_max[MIN].y(), min_max[MIN].z()), pYmax1);
        cam->compute_pixel_coordinates(vec3(min_max[MAX].x(), min_max[MAX].y(), min_max[MIN].z()), pYmax2);
        ofs << "    <line x1=\"" << pXZmax1.x() << "\" y1=\"" << pXZmax1.y() << "\" x2=\"" << pYmax1.x() << "\" y2=\"" << pYmax1.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        ofs << "    <line x1=\"" << pXZmax2.x() << "\" y1=\"" << pXZmax2.y() << "\" x2=\"" << pYmax2.x() << "\" y2=\"" << pYmax2.y() << "\" style=\"stroke:rgb(0,255,0);stroke-width:1.2\" />\n"; 
        
    }

    void rot_x(float deg){

        vec3 point = get_box_center();
        matrix44 tr(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -point.x(),-point.y(),-point.z(),1);
        matrix44 itr = tr.inverse();
        float sen = sin(deg*PI/180.0);
        float co = cos(deg*PI/180);
        matrix44 rot( 1,   0,   0,   0, 
                      0,  co,-sen,   0,
                      0, sen,  co,   0, 
                      0,   0,   0,   1);
        matrix44 result = (tr*rot)*itr;

        for(int i = 0; i < ((n+1)*(n+1)); ++i)
            result.multVecMatrix(controlPoints[i], controlPoints[i]);

        result.multVecMatrix(min_max[MIN], min_max[MIN]);
        result.multVecMatrix(min_max[MAX], min_max[MAX]);

    }

    void rot_y(float deg){

        vec3 point = get_box_center();
        matrix44 tr(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -point.x(),-point.y(),-point.z(),1);
        matrix44 itr = tr.inverse();
        float sen = sin(deg*PI/180.0);
        float co = cos(deg*PI/180);
        matrix44 rot( co,   0, sen, 0, 
                        0,   1,   0, 0,
                    -sen,   0,  co, 0, 
                        0,   0,   0, 1);
        matrix44 result = (tr*rot)*itr;

        for(int i = 0; i < ((n+1)*(n+1)); ++i){
            result.multVecMatrix(controlPoints[i], controlPoints[i]);
        }

        result.multVecMatrix(min_max[MIN], min_max[MIN]);
        result.multVecMatrix(min_max[MAX], min_max[MAX]);
    }

    void rot_z(float deg){

        vec3 point = get_box_center();
        matrix44 tr(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -point.x(),-point.y(),-point.z(),1);
        matrix44 itr = tr.inverse();
        float sen = sin(deg*PI/180.0);
        float co = cos(deg*PI/180);
        matrix44 rot( co, -sen,   0,   0, 
                         sen,   co,   0,   0,
                           0,    0,   1,   0, 
                           0,    0,   0,   1);
        matrix44 result = (tr*rot)*itr;

        for(int i = 0; i < ((n+1)*(n+1)); ++i){
            result.multVecMatrix(controlPoints[i], controlPoints[i]);
        }
    }
};

#endif