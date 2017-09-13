#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <math.h>

using namespace std;

float counter =-1;
int z_counter = 0;
string input_file_name;
string scalar_file_name;
float min_value=0.0f;
float max_value=0.0f;
typedef float VECTOR[3];
VECTOR vec_data[600][248][248];
float vec_mag[600][248];
VECTOR vel[600][248][248];
float curl_mag[600][248][248];
int first = 0;
float data_array2D[600][248], norm_data_array2D[600][248];
int MAX_NUM_ITER = 200;

struct GridPoint{
    float coord_x, coord_y;
    float scalar_value;
};

struct Vec_GridPoint{
    float coord_x, coord_y,coord_z;
    VECTOR velocity;
};


Vec_GridPoint velocity_value_func(Vec_GridPoint ptxy, Vec_GridPoint *gridCell){

    float s = (ptxy.coord_x - gridCell[0].coord_x)/(gridCell[1].coord_x-gridCell[0].coord_x);
    float t = (ptxy.coord_y - gridCell[0].coord_y)/(gridCell[1].coord_y-gridCell[0].coord_y);

    ptxy.velocity[0] =  (1-s)*(1-t  )* gridCell[0].velocity[0] +
                        (1-s)*(t    )* gridCell[3].velocity[0] +
                        (s  )*(t    )* gridCell[2].velocity[0] +
                        (s  )*(1-t  )* gridCell[1].velocity[0] ;

    ptxy.velocity[1] =  (1-s)*(1-t  )* gridCell[0].velocity[1] +
                        (1-s)*(t    )* gridCell[3].velocity[1] +
                        (s  )*(t    )* gridCell[2].velocity[1] +
                        (s  )*(1-t  )* gridCell[1].velocity[1] ;

//    ptxy.velocity[2] =  (1-s)*(1-t  )* gridCell[0].velocity[2] +
//                        (1-s)*(t    )* gridCell[3].velocity[2] +
//                        (s  )*(t    )* gridCell[2].velocity[2] +
//                        (s  )*(1-t  )* gridCell[1].velocity[2] ;

    return ptxy;

}


void locateGridCell(Vec_GridPoint seedPoint, Vec_GridPoint *cell){


    int i = int(seedPoint.coord_x);
    int j = int(seedPoint.coord_y);
    int k = int(seedPoint.coord_z);

    cell[0].coord_x = i; cell[0].coord_y = j;
    //cell[0].coord_z = float(k);
    /*float a,b,c;
    a = vec_data[i][j][k][0];
    b = vec_data[i][j][k][1];
    c = vec_data[i  ][j][k][2]; */
    cell[0].velocity[0] = vec_data[i][j][k][0];     cell[0].velocity[1] = vec_data[i][j][k][1];
    //cell[0].velocity[2] = vec_data[i][j][k][2];

    cell[1].coord_x = (i+1); cell[1].coord_y = (j);
    cell[1].velocity[0] = vec_data[i+1][j][k][0];     cell[1].velocity[1] = vec_data[i+1][j][k][1];

    cell[2].coord_x = (i+1); cell[2].coord_y = (j+1);
    cell[2].velocity[0] = vec_data[i+1][j+1][k][0];     cell[2].velocity[1] = vec_data[i+1][j+1][k][1];

    cell[3].coord_x = (i); cell[3].coord_y = (j+1);
    cell[3].velocity[0] = vec_data[i][j+1][k][0];     cell[3].velocity[1] = vec_data[i][j+1][k][1];

   // return cell;

}

Vec_GridPoint RK4(Vec_GridPoint initial_pt, float h){

    Vec_GridPoint final_pt;
    Vec_GridPoint delta1, delta2, delta3;

    Vec_GridPoint cell[4];

    float k1, k2, k3, k4;

    locateGridCell(initial_pt, cell);

    Vec_GridPoint gp = velocity_value_func(initial_pt,cell);

    k1 = h * (gp.velocity[0]);

    delta1.coord_x = initial_pt.coord_x + (0.5) * h;
    delta1.coord_y = initial_pt.coord_y + (0.5) * k1;
    delta1.coord_z = initial_pt.coord_z;

    //cell = locateGridCell(delta1);
    locateGridCell(delta1, cell);

    k2 = h * velocity_value_func(delta1, cell).velocity[0];

    delta2.coord_x = initial_pt.coord_x + (0.5) * h;
    delta2.coord_y = initial_pt.coord_y + (0.5) * k2;
    delta2.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta2,cell);

    k3 = h * velocity_value_func(delta2, cell).velocity[0];

    delta3.coord_x = initial_pt.coord_x + h;
    delta3.coord_y = initial_pt.coord_y + k3;
    delta3.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta3,cell);

    k4 = h * velocity_value_func(delta3,cell).velocity[0];

    final_pt.coord_x = initial_pt.coord_x +h;
    final_pt.velocity[0] = initial_pt.velocity[0] + (1.0/6.0)*(k1+ 2*k2 + 2*k3 + k4);
    //final_pt.velocity[0] =

    // RK4 in Y direction

    //cell =
    locateGridCell(initial_pt,cell);

    k1 = h * velocity_value_func(initial_pt,cell).velocity[1];

    delta1.coord_x = initial_pt.coord_x + (0.5) * k1;
    delta1.coord_y = initial_pt.coord_y + (0.5) * h;
    delta1.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta1,cell);

    k2 = h * velocity_value_func(delta1, cell).velocity[1];

    delta2.coord_x = initial_pt.coord_x + (0.5) * k2;
    delta2.coord_y = initial_pt.coord_y + (0.5) * h;
    delta2.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta2,cell);

    k3 = h * velocity_value_func(delta2, cell).velocity[1];

    delta3.coord_x = initial_pt.coord_x + k3;
    delta3.coord_y = initial_pt.coord_y + h;
    delta3.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta3,cell);

    k4 = h * velocity_value_func(delta3,cell).velocity[1];

    final_pt.coord_y = initial_pt.coord_y +h;
    final_pt.velocity[1] = initial_pt.velocity[1] + (1.0/6.0)*(k1+ 2*k2 + 2*k3 + k4);

    final_pt.coord_z = initial_pt.coord_z;
    final_pt.velocity[2] = initial_pt.velocity[2];

    return final_pt;

}

struct LineSegment {
GridPoint point1, point2;
};
struct SaddleSegments {
    LineSegment segment_a, segment_b;
};
SaddleSegments getSaddleSegments(int case_number, GridPoint *gp_array ){
    SaddleSegments segments;

    switch (case_number) {
        case 5: segments.segment_a.point1.coord_x = (gp_array[3].coord_x + gp_array[2].coord_x)/2;
                segments.segment_a.point1.coord_y = (gp_array[3].coord_y + gp_array[2].coord_y)/2;

                segments.segment_a.point2.coord_x = (gp_array[3].coord_x + gp_array[0].coord_x)/2;
                segments.segment_a.point2.coord_y = (gp_array[3].coord_y + gp_array[0].coord_y)/2;

                segments.segment_b.point1.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                segments.segment_b.point1.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;

                segments.segment_b.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                segments.segment_b.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;

            break;
        case 10:segments.segment_a.point1.coord_x = (gp_array[3].coord_x + gp_array[2].coord_x)/2;
                segments.segment_a.point1.coord_y = (gp_array[3].coord_y + gp_array[2].coord_y)/2;

                segments.segment_a.point2.coord_x = (gp_array[2].coord_x + gp_array[1].coord_x)/2;
                segments.segment_a.point2.coord_y = (gp_array[2].coord_y + gp_array[1].coord_y)/2;

                segments.segment_b.point1.coord_x = (gp_array[3].coord_x + gp_array[0].coord_x)/2;
                segments.segment_b.point1.coord_y = (gp_array[3].coord_y + gp_array[0].coord_y)/2;

                segments.segment_b.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                segments.segment_b.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;

            break;

                break;
        default:
                break;
    }

    return segments;
}
LineSegment getLineSegmentByCase(int case_number, GridPoint *gp_array ){
    LineSegment a;

    switch (case_number) {
        //case 0:
        case 1: a.point1.coord_x = (gp_array[3].coord_x + gp_array[0].coord_x)/2;
                a.point1.coord_y = (gp_array[3].coord_y + gp_array[0].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;
                break;

        case 2: a.point1.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;
                break;
        case 3: a.point1.coord_x = (gp_array[0].coord_x + gp_array[3].coord_x)/2;
                a.point1.coord_y = (gp_array[0].coord_y + gp_array[3].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;
                break;

        case 4: a.point1.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[2].coord_x + gp_array[1].coord_x)/2;
                a.point2.coord_y = (gp_array[2].coord_y + gp_array[1].coord_y)/2;

                break;

        case 6: a.point1.coord_x = (gp_array[3].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[3].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;

                break;
        case 7: a.point1.coord_x = (gp_array[3].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[3].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[3].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[3].coord_y + gp_array[0].coord_y)/2;

                break;
        case 8: a.point1.coord_x = (gp_array[3].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[3].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[3].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[3].coord_y + gp_array[0].coord_y)/2;

                break;
        case 9: a.point1.coord_x = (gp_array[3].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[3].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;

                break;
        case 11:a.point1.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[2].coord_x + gp_array[1].coord_x)/2;
                a.point2.coord_y = (gp_array[2].coord_y + gp_array[1].coord_y)/2;

                break;
        case 12:a.point1.coord_x = (gp_array[0].coord_x + gp_array[3].coord_x)/2;
                a.point1.coord_y = (gp_array[0].coord_y + gp_array[3].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;

                break;
        case 13:a.point1.coord_x = (gp_array[1].coord_x + gp_array[2].coord_x)/2;
                a.point1.coord_y = (gp_array[1].coord_y + gp_array[2].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;
                break;
        case 14:a.point1.coord_x = (gp_array[3].coord_x + gp_array[0].coord_x)/2;
                a.point1.coord_y = (gp_array[3].coord_y + gp_array[0].coord_y)/2;

                a.point2.coord_x = (gp_array[1].coord_x + gp_array[0].coord_x)/2;
                a.point2.coord_y = (gp_array[1].coord_y + gp_array[0].coord_y)/2;
                break;
        //case 15:break;

        default:
            break;
    }

    return a;
}
int getCaseNumberByGridPointsCutoff(float cutoff, GridPoint *gp_array){

    int caseNumber = 0;

    if(gp_array[3].scalar_value>=cutoff) {caseNumber = caseNumber | 8;}
    if(gp_array[2].scalar_value>=cutoff) {caseNumber = caseNumber | 4;}
    if(gp_array[1].scalar_value>=cutoff) {caseNumber = caseNumber | 2;}
    if(gp_array[0].scalar_value>=cutoff) {caseNumber = caseNumber | 1;}

    return caseNumber;

}
void renderColorMap(){

    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_QUADS);

    for(int y=0;y<(248-1);y++){
        for(int x=0;x<(600-1);x++){
            //for vertex(x,y)
            glColor3f(norm_data_array2D[x][y],0.0f,0.0f);
            glVertex2f(-0.9f+x*0.003,-0.375f+y*0.003);

            //for vertex (x+1,y)
            glColor3f(norm_data_array2D[x+1][y],0.0f,0.0f);
            glVertex2f(-0.9f+(x+1)*0.003,-0.375f+y*0.003);

            //for vertex (x+1,y+1)
            glColor3f(norm_data_array2D[x+1][y+1],0.0f,0.0f);
            glVertex2f(-0.9f+(x+1)*0.003,-0.375f+(y+1)*0.003);

            //for vertex (x,y+1)
            glColor3f(norm_data_array2D[x][y+1],0.0f,0.0f);
            glVertex2f(-0.9f+x*0.003,-0.375f+(y+1)*0.003);

            }
        }

        glEnd();
    glutSwapBuffers();

}
void renderHeightMap(){

    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(-0.1,0.5, 0.5,  /* eye is at (-0.1,0.5, 0.5) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);

    glRotatef( counter , 1.0, 1.0 , 1.0 );
    counter+=0.01;
    if(counter>=1)counter=-1;


    glBegin(GL_TRIANGLE_STRIP);

    for(int x=0;x<(600-1);x++){
        for(int y=0;y<(248-1);y++){
            //for vertex (x,y)
            glColor3f(0.0f,0.0f,norm_data_array2D[x][y]);
            glVertex3f(x*0.001,norm_data_array2D[x][y],y*0.001);

            //for vertex (x+1,y)
            glColor3f(0.0f,0.0f,norm_data_array2D[(x+1)][y]);
            glVertex3f((x+1)*0.001,norm_data_array2D[(x+1)][y],y*0.001);

            //for vertex (x,y+1)
            glColor3f(0.0f,0.0f,norm_data_array2D[x][(y+1)]);
            glVertex3f(x*0.001,norm_data_array2D[x][(y+1)],(y+1)*0.001);

            //for vertex (x+1,y+1)
            glColor3f(0.0f,0.0f,norm_data_array2D[(x+1)][(y+1)]);
            glVertex3f((x+1)*0.001,norm_data_array2D[(x+1)][(y+1)],(y+1)*0.001);

        }
    }

    glEnd();



    glutSwapBuffers();

}
void renderContourMap(){

    glClearColor(0.0f, 0.0f,0.0f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cout<<"Rendering Quads";
    glBegin(GL_QUADS);

    for(int y=0;y<(248-1);y++){
        for(int x=0;x<(600-1);x++){
            //for vertex (x,y)
            glColor3f(norm_data_array2D[x][y],0.0f,0.0f);
            glVertex2f(-0.9f+x*0.003,-0.375f+y*0.003);

            //for vertex (x+1,y)
            glColor3f(norm_data_array2D[x+1][y],0.0f,0.0f);
            glVertex2f(-0.9f+(x+1)*0.003,-0.375f+y*0.003);

            //for vertex (x+1,y+1)
            glColor3f(norm_data_array2D[x+1][y+1],0.0f,0.0f);
            glVertex2f(-0.9f+(x+1)*0.003,-0.375f+(y+1)*0.003);

            //for vertex (x,y+1)
            glColor3f(norm_data_array2D[x][y+1],0.0f,0.0f);
            glVertex2f(-0.9f+x*0.003,-0.375f+(y+1)*0.003);

        }
    }

    glEnd();



    GridPoint gp_array[4];
    LineSegment ls;
    SaddleSegments ss;


    int case_num =0;
    float cut_off[10] = {500.0f, 1000.0f, 2000.f, 3000.0f, 4000.0f};

    cout<<"rendering lines";
    glBegin(GL_LINES);

    glColor3f(1.0f,1.0f,0.0f);

    for(int contour_index = 0; contour_index <5 ; contour_index++){
        for(int y=0;y<(248-1);y++){
            for(int x=0;x<(600-1);x++){

                gp_array[0].coord_x=x;
                gp_array[0].coord_y=y;
                gp_array[0].scalar_value=data_array2D[x][y];

                gp_array[1].coord_x=(x+1);
                gp_array[1].coord_y=y;
                gp_array[1].scalar_value=data_array2D[(x+1)][y];

                gp_array[2].coord_x=(x+1);
                gp_array[2].coord_y=(y+1);
                gp_array[2].scalar_value=data_array2D[(x+1)][(y+1)];

                gp_array[3].coord_x=x;
                gp_array[3].coord_y=(y+1);
                gp_array[3].scalar_value=data_array2D[x][(y+1)];

                case_num = getCaseNumberByGridPointsCutoff(cut_off[contour_index], gp_array);
                if(first==0){
                    cout<<"Case:" <<case_num<<endl;
                }

                if(case_num!=0 && case_num!=15){
                    if(case_num!=5 && case_num!=10){

                        ls = getLineSegmentByCase(case_num, gp_array);
                        glVertex2f(-0.9f+(ls.point1.coord_x)*0.003,-0.375f+(ls.point1.coord_y)*0.003);
                        glVertex2f(-0.9f+(ls.point2.coord_x)*0.003,-0.375f+(ls.point2.coord_y)*0.003);

                    }else{
                            ss = getSaddleSegments(case_num, gp_array);
                            glVertex2f(-0.9f+(ss.segment_a.point1.coord_x)*0.003,-0.375f+(ss.segment_a.point1.coord_y)*0.003);
                            glVertex2f(-0.9f+(ss.segment_a.point2.coord_x)*0.003,-0.375f+(ss.segment_a.point2.coord_y)*0.003);

                            glVertex2f(-0.9f+(ss.segment_b.point1.coord_x)*0.003,-0.375f+(ss.segment_b.point1.coord_y)*0.003);
                            glVertex2f(-0.9f+(ss.segment_b.point2.coord_x)*0.003,-0.375f+(ss.segment_b.point2.coord_y)*0.003);
                        }

                }

            }
        }
    }

    if(first==0)first++;
    glEnd();

    glutSwapBuffers();

}
float getVectorMagnitude(float  fi, float fj, float fk){
    return sqrt(fi*fi+fj*fj+fk*fk);
}
int extractVectorData(){

    int result = 0;
    ifstream fin(input_file_name.c_str());
    if(!fin){
        cout<<"Could not open " << input_file_name << " for reading";
        return -2;
    }

    float vec_magnitude;
    float r0, r1, r2;
    unsigned x,y;
    for(int z=0;z<1;z++){
        for(y=0;y<248;y++){
            for(x=0;x<600;x++){
                fin >> r0 >> r1 >>r2;

                    vec_magnitude = getVectorMagnitude(r0,r1,r2);
                    if(x==0 && y==0){
                        min_value = vec_magnitude;
                        max_value = vec_magnitude;
                    }

                    if(vec_magnitude < min_value){
                        min_value = vec_magnitude;
                    }
                    if(vec_magnitude > max_value){
                        max_value = vec_magnitude;
                    }

                    vec_data[x][y][z][0]=r0;vec_data[x][y][z][1]=r1;vec_data[x][y][z][2]=r2;
                    //vec_mag[x][y] = vec_magnitude;



            }
        }


    }
    fin.close();

    result = 1;

    return result;
}
int extractVectorbyZ(int z_value){

    int result = 0;
    ifstream fin(input_file_name.c_str());
    if(!fin){
        cout<<"Could not open " << input_file_name << " for reading";
        return -2;
    }

    float vec_magnitude;
    float r0, r1, r2;
    unsigned x,y;
    for(int z=0;z<248;z++){
        for(y=0;y<248;y++){
            for(x=0;x<600;x++){
                fin >> r0 >> r1 >>r2;
                 if(z==z_value){
                    vec_magnitude = getVectorMagnitude(r0,r1,r2);
                    if(x==0 && y==0){
                        min_value = vec_magnitude;
                        max_value = vec_magnitude;
                    }

                    if(vec_magnitude < min_value){
                        min_value = vec_magnitude;
                    }
                    if(vec_magnitude > max_value){
                        max_value = vec_magnitude;
                    }

                    vec_data[x][y][z][0]=r0;vec_data[x][y][z][1]=r1;vec_data[x][y][z][2]=r2;
                    //vec_mag[x][y] = vec_magnitude;
                 }


            }
        }

        if(z==z_value)break;

    }
    fin.close();

    result = 1;

    return result;
}
void renderHedgeHog(){


   // extractVectorbyZ(z_counter++);
    cout<<"\t z = "<<z_counter;

    if(z_counter>=247){
        z_counter=0;

    }

    glClearColor(0.0f, 0.0f,0.0f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();


    //cout<<"Rendering Lines";
    glBegin(GL_LINES);

    float norm_vec_mag = 0.0;
    float vec_mag = 0.0;
    float step = 1.0f;

    for(int y=0;y<248;y+=10){
        for(int x=0;x<600;x+=10){
            //for vertex (x,y)
            vec_mag = getVectorMagnitude(vec_data[x][y][z_counter][0],vec_data[x][y][z_counter][1],vec_data[x][y][z_counter][2]);
            norm_vec_mag = (vec_mag-min_value)/(max_value-min_value);
//                vec_length = sqrt(vec_data[x][y][0]*vec_data[x][y][0]+vec_data[x][y][1]*vec_data[x][y][1]);
//                x_c = (vec_data[x][y][0])/vec_mag[x][y];
//                y_c = (vec_data[x][y][1])/vec_mag[x][y];

            glColor3f(0.0f,0.2f,norm_vec_mag);

//                glVertex2f(-0.9f+(x+x_c)*0.003,y*0.003);
//                glVertex2f(-0.9f+(x+vec_data[x][y][0])*0.003,(y+vec_data[x][y][1])*0.003);
//
//                glVertex2f(-0.9f+(x+vec_data[x][y][0])*0.003,(y+vec_data[x][y][1])*0.003);
//                glVertex2f(-0.9f+(x)*0.003,(y+y_c)*0.003);

            glVertex2d(x*step, y*step);
            glVertex2d((x+vec_data[x][y][z_counter][0])*step, (y+vec_data[x][y][z_counter][1])*step);
//
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y-(vec_length/10)+0.5)*0.1);
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//                glVertex2d(-0.9f + (x-(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//
//                glVertex2d(-0.9f + (x-(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y-(vec_length/10)+0.5)*0.1);


        }
    }

//        vec_length = sqrt(0.5f);
//        for(int x = 0; x<6; x++ ){
//            for(int y=0; y<3; y++){
//
//                glColor3f(0.0f, 0.5f, 0.5f);
//
//
//                glVertex2d(-0.9f + x*0.1, y*0.1);
//                glVertex2d(-0.9f + (x+0.5)*0.1, (y+0.5)*0.1);
//
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y-(vec_length/10)+0.5)*0.1);
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//                glVertex2d(-0.9f + (x-(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//
//                glVertex2d(-0.9f + (x-(vec_length/10)+0.5)*0.1, (y+(vec_length/10)+0.5)*0.1);
//                glVertex2d(-0.9f + (x+(vec_length/10)+0.5)*0.1, (y-(vec_length/10)+0.5)*0.1);
//
//
//            }
//        }
//
    z_counter++;
    glEnd();
    glutSwapBuffers();

}
void renderStreamLines(){

    float h = 0.5;
    int num_iter = 0;
    Vec_GridPoint seedPoints[2];

    seedPoints[0].coord_x = 350.0;
    seedPoints[0].coord_y = 124.0;
    seedPoints[0].coord_z = 0.0;

    seedPoints[0].velocity[0] = vec_data[350][124][0][0];
    seedPoints[0].velocity[1] = vec_data[350][124][0][1];
    seedPoints[0].velocity[2] = vec_data[350][124][0][2];
    //set seedPoints : Make this a function instead

    Vec_GridPoint final_pt, initial_pt;

    //glBegin(GL_LINES);
    cout<<"Prinitng stream Lines"<<endl;

    cout<<"(350,124) ("<< vec_data[int(seedPoints[0].coord_x)][int(seedPoints[0].coord_y)][int(seedPoints[0].coord_z)][0]<<","<<vec_data[350][124][0][1]<<endl;

    for(int i=0; i<1 ; i++){
        cout<<"In i = "<< i << " loop"<<endl;
        initial_pt = seedPoints[i];
//        initial_pt.coord_x = seedPoints[i].coord_x;
//        initial_pt.coord_y = seedPoints[i].coord_y;
//        initial_pt.coord_z = seedPoints[i].coord_z;
        final_pt.coord_x = 2.0;
        final_pt.coord_y = 2.0;
        while(num_iter < MAX_NUM_ITER && (final_pt.coord_x!=initial_pt.coord_x && final_pt.coord_y!=initial_pt.coord_y)
              && final_pt.coord_x < 599 && final_pt.coord_x > 0 && final_pt.coord_y < 247 && final_pt.coord_y > 0){

                cout<<"Initial Point ("<< initial_pt.coord_x << " , "<<initial_pt.coord_y<<") Values ("
                <<initial_pt.velocity[0]<<" , " << initial_pt.velocity[1]<<" )" << endl;
                final_pt = RK4(initial_pt, h);
                //glColor3f(0.0f,0.5f,0.5f);

                //glVertex2d(initial_pt.coord_x, initial_pt.coord_y);
                //glVertex2d(final_pt.coord_x, final_pt.coord_y);

                initial_pt = final_pt;

                num_iter++;

        }


    }

    //glEnd();

    //glutSwapBuffers();


}
int extractScalarbyZ(int scalar_index, int z_value){

    int result = 0;
    float scalar[10];
    ifstream fin(input_file_name.c_str());

    if(!fin){
        cout<<"Could not open " << input_file_name << " for reading";
        return -2;
    }

    if(scalar_index<0 || scalar_index>9){
        cout<<"Invalid scalar_index for multifield file. Valid Values :  0<= scalar_index <=9 .";
        return -1;
    }

    if(z_value<0 || z_value>247){
        cout<<"Invalid z_value. Valid Values :  0<= z_value <=247 .";
    }

    for(int z=0;z<248;z++){
        for(int y=0;y<248;y++){
            for(int x=0;x<600;x++){
                fin >> scalar[0] >> scalar[1] >>scalar[2] >>scalar[3] >>scalar[4] >>scalar[5] >>scalar[6] >>scalar[7] >>scalar[8] >>scalar[9];
                if(z==z_value){
                    if(x==0 && y==0 ){
                        max_value = scalar[scalar_index];
                        min_value = scalar[scalar_index];
                    }
                    if(scalar[scalar_index] > max_value ){
                        max_value = scalar[scalar_index];
                    }
                    if(scalar[scalar_index] < min_value ){
                        min_value = scalar[scalar_index];
                    }
                    data_array2D[x][y]=scalar[scalar_index];
                }
            }
        }
        if(z==z_value)break;
    }

    result = 1;
    fin.close();

return result;
}
int extractMagOfCurl(){

    int result = 0;
    ifstream fin(input_file_name.c_str());
    if(!fin){
        cout<<"Could not open " << input_file_name << " for reading";
        return -2;
    }else{
        cout<<"Able to Open file: "<< input_file_name.c_str();
    }


    float r0,r1,r2;
    unsigned x,y,z;
    cout<<"reading file"<<endl;
    for(z=0;z<248;z++){
        for(y=0;y<248;y++){
            for(x=0;x<600;x++){
                fin >> r0 >> r1 >> r2;
                 vel[x][y][z][0]=r0;
                 vel[x][y][z][1]=r1;
                 vel[x][y][z][2]=r2;
            }
        }

    }
    fin.close();

    cout<<"Calculating curl"<<endl;
    for(z=0;z<248;z++){
    for(y=0;y<248;y++){
        for(x=0;x<600;x++){
            if((x==599) || (y==247)||(z==247)){
                curl_mag[x][y][z]=0.0;
            }else{
                VECTOR curl;
                curl[0] = (vel[x][y+1][z][2]-vel[x][y][z][2]-vel[x][y][z+1][1]+vel[x][y][z][1])/0.001;
                curl[1] = (vel[x][y][z+1][0]-vel[x][y][z][0]-vel[x+1][y][z][2]+vel[x][y][z][2])/0.001;
                curl[2] = (vel[x+1][y][z][1]-vel[x][y][z][1]-vel[x][y+1][z][0]+vel[x][y][z][0])/0.001;
                curl_mag[x][y][z]= sqrt(curl[0]*curl[0]+curl[1]*curl[1]+curl[2]*curl[2]);
            }

        }
    }
    }

    cout<<"Compeleted calculating Curl";
    result = 1;

    return result;

}
int subsetCurlMag(int z_value){

    for(int z=0;z<248;z++){
        for(int y=0;y<248;y++){
            for(int x=0;x<600;x++){
                if(z==z_value){
                    if(x==0 && y==0 ){
                        max_value = curl_mag[x][y][z];
                        min_value = curl_mag[x][y][z];
                    }
                    if(curl_mag[x][y][z] > max_value ){
                        max_value = curl_mag[x][y][z];
                    }
                    if(curl_mag[x][y][z] < min_value ){
                        min_value = curl_mag[x][y][z];
                    }
                    data_array2D[x][y]=curl_mag[x][y][z];
                }
            }
        }
        if(z==z_value)break;
    }

    return 1;

}
int normalize_data(float min_val, float max_val){

    int result = 0;

    for(int x=0;x<600;x++){
        for(int y=0;y<248;y++){
            norm_data_array2D[x][y] = (data_array2D[x][y] - min_val)/(max_value-min_val);
        }
    }

    return result;
}
void reshape(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glOrtho(-700.0, 700.0, -300.0, 300.0, -300.0, 300.0);
    glMatrixMode(GL_MODELVIEW);

}
void initOpenGL(){

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

}
int main(int argc, char **argv){

    input_file_name = argv[1];
    //input_file_name = "../velocity.0100.txt";

    string scalarName = argv[2];
    //string scalarName = "velocity";

    int scalar_index=0;

    if(scalarName=="density"){
        scalar_index=0;
    }
    if(scalarName=="temperature"){
        scalar_index=1;
    }
    if(scalarName=="curl"){
        scalar_index=10;
    }
    if(scalarName=="velocity"){
        scalar_index = 15;
    }

    string map_type = argv[3];
    //string map_type = "streamlines";

    cout<<endl<<input_file_name<<" " << scalarName<< " " << map_type <<endl;
    int extract_done =0;



//    glutInit(&argc,argv);
//    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
//    glutInitWindowSize(500,500);
//    glutInitWindowPosition(100,100);
//    glutCreateWindow("Assignment 1");
//    initOpenGL();

    switch(scalar_index){

    case 0: extract_done = extractScalarbyZ(scalar_index,0);
            cout<<"Extract Completed. Normalization started"<<endl;
            normalize_data(min_value, max_value);
            cout<<"Normalization complete"<<endl;

            if(extract_done==1){
                normalize_data(min_value, max_value);
                if(map_type=="colormap"){
                    cout<<"\nGenerating ColorMap for Density"<<endl;
                    glutDisplayFunc(renderColorMap);
                    glutIdleFunc(renderColorMap);
                }
                if(map_type=="heightmap"){
                    glutDisplayFunc(renderHeightMap);
                    glutIdleFunc(renderHeightMap);
                }
                if(map_type=="contourmap"){
                    glutDisplayFunc(renderContourMap);
                    glutIdleFunc(renderContourMap);
                }
            }else{
                cout<<"Unable to extract data";
            }
            break;

    case 1: extract_done = extractScalarbyZ(scalar_index,0);
            cout<<"Extract Completed. Normalization started"<<endl;
            normalize_data(min_value, max_value);
            cout<<"Normalization complete"<<endl;
            if(extract_done==1){
                if(map_type=="colormap"){
                    cout<<"\nGenerating ColorMap for Temperature"<<endl;
                    glutDisplayFunc(renderColorMap);
                    glutIdleFunc(renderColorMap);
                }
                if(map_type=="heightmap"){
                    glutDisplayFunc(renderHeightMap);
                    glutIdleFunc(renderHeightMap);
                }
                if(map_type=="contourmap"){
                    glutDisplayFunc(renderContourMap);
                    glutIdleFunc(renderContourMap);
                }
            }else{
                cout<<"Unable to extract data";
            }
            break;

    case 10:if(extractMagOfCurl()==1){
                extract_done = subsetCurlMag(0);
                normalize_data(min_value, max_value);
            }

            if(extract_done==1){
                if(map_type=="colormap"){
                    cout<<"\nGenerating ColorMap for Curl magnitude"<<endl;
                    glutDisplayFunc(renderColorMap);
                    glutIdleFunc(renderColorMap);
                }
                if(map_type=="heightmap"){
                    glutDisplayFunc(renderHeightMap);
                    glutIdleFunc(renderHeightMap);
                }
                if(map_type=="contourmap"){
                    glutDisplayFunc(renderContourMap);
                    glutIdleFunc(renderContourMap);
                }
            }else{
                cout<<"Unable to extract data";
            }
            break;

    case 15:
            extract_done = extractVectorData();

            cout<<"Extract Done = "<< extract_done<<endl<<"Min_value "<< min_value <<endl<<"Max_value "<< max_value;

            if(extract_done==1){
                if(map_type=="hedgehog"){
                    cout<<"\nGenerating Hedgehog for Velocity"<<endl;
                    glutDisplayFunc(renderHedgeHog);
                    glutIdleFunc(renderHedgeHog);
                }
                if(map_type=="streamlines"){
                    //glutDisplayFunc(renderStreamLines);
                    //glutIdleFunc(renderStreamLines);
                    renderStreamLines();
                }
            }else{
                cout<<"Unable to extract data";
            }
            break;

    default:    cout<<"Invalid MapType or Scalar/Vector Field Name."<<endl;
                cout<<"Valid Scalar Fields are:   density,    pressure    & curl"<<endl;
                cout<<"Valid Scalar field Maps are:  colormap,   heightmap   & contourmap"<<endl;
                cout<<"Valid Vector Fields are:   velocity"<<endl;
                cout<<"Valid Vector Fields MapType are:        hedgehog    & streamlines"<<endl;
                break;
    }


   // glutReshapeFunc(reshape);
    //glutMainLoop();
    return 0;
}

