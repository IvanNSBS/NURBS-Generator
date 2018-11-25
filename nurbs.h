#ifndef NURBSH
#define NURBSH

#include "vec2i.h"
#include "vec3.h"
#include "camera.h"
#include <fstream>
#include <vector>

class NURBS
{
public: 
    int n, order, m;
    vec3 *controlPointsA;
    vec3 *controlPointsB;
    float *weights;
    float *knots;
    camera *cam;

    NURBS(const int &nn = 0, const int &ord = 0, 
          vec3 *cps = 0, float *w = 0, float *kts = 0):
          n(nn), order(ord), m(nn + ord + 1), controlPointsA(cps), weights(w), knots(kts)
    {
        if(cps == 0)
            controlPointsA = new vec3[nn+1];

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

    vec3 deBoor(int n, int k, float t){
        vec3 result(0.0, 0.0, 0.0);
        float denom = 0;
        for(int i = 1; i <= n+1; i++){
            float w = S(i, k, t);

            vec3 numerator = w * controlPointsA[i-1] * weights[i-1];
            //std::cout << "w: " << w << std::endl; 
            float denominator = w * weights[i-1];

            denom += denominator;
            result += numerator;
        }
        //std::cout << "denom: " << result << std::endl;
        result /= denom;

        return result;
    }
 
    void render_control_points(std::ofstream &ofs){ 
        for(int i = 0; i < n; i++){
            vec2f praster1;
            vec2f praster2;
            cam->compute_pixel_coordinates(controlPointsA[i], praster1);
            cam->compute_pixel_coordinates(controlPointsA[i+1], praster2);
            ofs << "    <rect x=\"" << praster1.x()-2 << "\" y=\"" << praster1.y()-2 << "\" width=\"4\" height=\"4\" style=\"fill:white;stroke:pink;stroke-width:0;opacity:1\" />\n";
            ofs << "    <rect x=\"" << praster2.x()-2 << "\" y=\"" << praster2.y()-2 << "\" width=\"4\" height=\"4\" style=\"fill:white;stroke:pink;stroke-width:0;opacity:1\" />\n";
            ofs << "    <line x1=\"" << praster1.x() << "\" y1=\"" << praster1.y() << "\" x2=\"" << praster2.x() << "\" y2=\"" << praster2.y() << "\" style=\"stroke:rgb(255,255,255);stroke-width:1.2\" />\n"; 
        }
    }

    void render_curve(std::ofstream &ofs, int numSegments){
        for (int i = 0; i < numSegments; ++i)
        {
            float x1 = (i/(float)numSegments)*((n+1) - (order-1)) + (order-1) + 0.05;
            float x2 = ((i+1)/(float)numSegments)*((n+1) - (order-1)) + (order-1) + 0.05;

            if(x1 <= knots[m-1] && x2 <= knots[m-1])
            {
                vec3 pt1 = deBoor(n, order, x1);
                vec3 pt2 = deBoor(n, order, x2);
                vec2f pr1;
                vec2f pr2;
                cam->compute_pixel_coordinates(pt1, pr1);
                cam->compute_pixel_coordinates(pt2, pr2);
                //ofs << "    <circle cx=\"" << pr1.x() << "\" cy=\"" << pr1.y() << "\" r=\"0.8\" stroke=\"black\" stroke-width=\"0.3\" fill=\"red\" />" << std::endl;
                ofs << "    <line x1=\"" << pr1.x() << "\" y1=\"" << pr1.y() << "\" x2=\"" << pr2.x() << "\" y2=\"" << pr2.y() << "\" style=\"stroke:rgb(0,25,115);stroke-width:0.8\" />\n"; 
            }
        }
    }
};

#endif