#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <math.h>

using namespace std;
string vector_file_name;
string scalar_file_name;
int MAX_NUM_ITER = 600;
int z_test = 50;


int z_key=0; // this variable is used to navigate up/down z values w.r.t the x-y values shown on graph

typedef float SCALAR[3];
SCALAR scalar_data[600][248][248]; // to store scalar data for 3 scalars: density, pressure & magnitude of Curl
SCALAR norm_scalar_data[600][248][248]; // to store normalized data
float scalar_min_max[2][3];// row 0 stores min, row 1 stores max

typedef float VECTOR[3];
VECTOR vector_data[600][248][248];
VECTOR vector_min_max[2];
float min_value=0.0f;
float max_value=0.0f;


static int menu_value = 0; // this is the value selected from the menu
static int window;
int scalar_index=0;
int integration_fn=0;

struct GridPoint{
    float coord_x, coord_y;
    float scalar_value;
};
struct Vec_GridPoint{
    float coord_x, coord_y,coord_z;
    VECTOR velocity;
};
struct LineSegment {
GridPoint point1, point2;
};
struct SaddleSegments {
    LineSegment segment_a, segment_b;
};
Vec_GridPoint seedPoints[10];

int extractScalarData(){

    int result = 0;
    float scalar[10];
    ifstream fin(scalar_file_name.c_str());

    if(!fin){
        cout<<"Could not open " << scalar_file_name << " for reading";
        return -2;
    }

    cout<<endl<<"Reading Scalar Data File : "<< scalar_file_name;

    for(int z=0;z<248 && z< z_test;z++){
        for(int y=0;y<248;y++){
            for(int x=0;x<600;x++){
                fin >> scalar[0] >> scalar[1] >>scalar[2] >>scalar[3] >>scalar[4] >>scalar[5] >>scalar[6] >>scalar[7] >>scalar[8] >>scalar[9];
                    if(x==0 && y==0 && z==0 ){
                        scalar_min_max[0][0]= scalar[0];scalar_min_max[0][1]= scalar[1];
                        scalar_min_max[1][0]= scalar[0];scalar_min_max[1][1]= scalar[1];
                    }
                    if(scalar[0] > scalar_min_max[1][0] ){
                        scalar_min_max[1][0] = scalar[0];
                    }
                    if(scalar[1] > scalar_min_max[1][1] ){
                        scalar_min_max[1][1] = scalar[1];
                    }
                    if(scalar[0] < scalar_min_max[0][0] ){
                        scalar_min_max[0][0] = scalar[0];
                    }
                    if(scalar[1] < scalar_min_max[0][1] ){
                        scalar_min_max[0][1] = scalar[1];
                    }
                    scalar_data[x][y][z][0] = scalar[0];
                    scalar_data[x][y][z][1] = scalar[1];

            }
        }
    }

    cout<<endl<<"Scalar Data Reading Complete";
    result = 1;
    fin.close();
return result;

}

int normalize_scalar_data(){

    int result=0;
    cout<<endl<<"Scalar Data Normalization begins"<<endl;
    for(int x=0; x<600; x++){
        for(int y=0; y<248; y++){
            for(int z=0; z<248 && z<z_test; z++){
                for(int i=0; i<3; i++){
                    norm_scalar_data[x][y][z][i] = (scalar_data[x][y][z][i] - scalar_min_max[0][i])/(scalar_min_max[1][i]-scalar_min_max[0][i]);
                }
            }
        }
    }
    result=1;
    cout<<endl<<"Normalization complete"<<endl;
    return result;
}


float getVectorMagnitude(float  fi, float fj, float fk){
    return sqrt(fi*fi+fj*fj+fk*fk);
}

int extractVectorData(){

    int result = 0;
    ifstream fin(vector_file_name.c_str());
    if(!fin){
        cout<<"Could not open " << vector_file_name << " for reading";
        return -2;
    }
    cout<<endl<<"Reading Vector Data File : "<< vector_file_name;
    float vec_magnitude;
    float r0, r1, r2;
    unsigned x,y,z;
    for(z=0;z<248 && z<z_test;z++){
        for(y=0;y<248;y++){
            for(x=0;x<600;x++){
                fin >> r0 >> r1 >>r2;

                    vec_magnitude = getVectorMagnitude(r0,r1,r2);
                    if(x==0 && y==0 && z==0){
                        min_value = vec_magnitude;
                        max_value = vec_magnitude;
                    }

                    if(vec_magnitude < min_value){
                        min_value = vec_magnitude;
                    }
                    if(vec_magnitude > max_value){
                        max_value = vec_magnitude;
                    }

                    vector_data[x][y][z][0]=r0;
                    vector_data[x][y][z][1]=r1;
                    vector_data[x][y][z][2]=r2;
            }
        }


    }
    fin.close();

    cout<<endl<<"Calculating curl"<<endl;
    for(z=0;z<248;z++){
    for(y=0;y<248;y++){
        for(x=0;x<600;x++){
            if((x==599) || (y==247)||(z==247)){
                scalar_data[x][y][z][2]=0.0;
            }else{
                VECTOR curl;
                curl[0] = (vector_data[x][y+1][z][2]-vector_data[x][y][z][2]-vector_data[x][y][z+1][1]+vector_data[x][y][z][1])/0.001;
                curl[1] = (vector_data[x][y][z+1][0]-vector_data[x][y][z][0]-vector_data[x+1][y][z][2]+vector_data[x][y][z][2])/0.001;
                curl[2] = (vector_data[x+1][y][z][1]-vector_data[x][y][z][1]-vector_data[x][y+1][z][0]+vector_data[x][y][z][0])/0.001;
                scalar_data[x][y][z][2]= sqrt(curl[0]*curl[0]+curl[1]*curl[1]+curl[2]*curl[2]);
            }

        }
    }
    }

    cout<<endl<<"Completed calculating Curl"<<endl<<"Vector Data File reading Complete"<<endl;

    result = 1;

    return result;
}


void processSpecialKeys(int key, int x, int y) {

 switch(key){

    case GLUT_KEY_UP:  if(z_key<247 && z_key < z_test){  z_key++; cout<<endl<<"z_key inc :"<<z_key<<endl;   }else{  z_key=0;    }
            break;
    case GLUT_KEY_DOWN: if(z_key>0 ){  z_key--;   cout<<endl<<"z_key dec :"<<z_key<<endl; }else{  z_key=z_test;    }
            break;

 }

  glutPostRedisplay();

}


void menu(int num){
    if(num == 'q'){
        glutDestroyWindow(window);
        exit(0);
    }else{
        menu_value = num;
    }
    glutPostRedisplay();
}
    //creating menu
void createMenu(void){

    int colorMapMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Density",0);
    glutAddMenuEntry("Temperature",1);
    glutAddMenuEntry("Magnitude.Curl",2);

    int heightMapMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Density",3);
    glutAddMenuEntry("Temperature",4);
    glutAddMenuEntry("Magnitude.Curl",5);

    int contourMapMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Density",6);
    glutAddMenuEntry("Temperature",7);
    glutAddMenuEntry("Magnitude.Curl",8);

    int scalarMenu = glutCreateMenu(menu);
    glutAddSubMenu("Color Map", colorMapMenu);
    glutAddSubMenu("Height Map", heightMapMenu );
    glutAddSubMenu("Contour Map", contourMapMenu );

    int streamLinesMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Euler's",10);
    glutAddMenuEntry("RK4",11);

    int vectorMenu = glutCreateMenu(menu);
    glutAddMenuEntry("HedgeHog",9);
    glutAddSubMenu("Stream Lines",streamLinesMenu);

    glutCreateMenu(menu);
    glutAddSubMenu("Scalar Viz", scalarMenu);
    glutAddSubMenu("Vector Viz", vectorMenu);
    glutAddMenuEntry("Quit",'q');
    glutAttachMenu(GLUT_RIGHT_BUTTON);

}



Vec_GridPoint cell[4];

Vec_GridPoint velocity_value_func(Vec_GridPoint ptxy){


    float s = (ptxy.coord_x - cell[0].coord_x)/(cell[1].coord_x - cell[0].coord_x);
    float t = (ptxy.coord_y - cell[0].coord_y)/(cell[3].coord_y - cell[0].coord_y);

    ptxy.velocity[0] =  (1-s)*(1-t  )* cell[0].velocity[0] +
                        (1-s)*(t    )* cell[3].velocity[0] +
                        (s  )*(t    )* cell[2].velocity[0] +
                        (s  )*(1-t  )* cell[1].velocity[0] ;

    ptxy.velocity[1] =  (1-s)*(1-t  )* cell[0].velocity[1] +
                        (1-s)*(t    )* cell[3].velocity[1] +
                        (s  )*(t    )* cell[2].velocity[1] +
                        (s  )*(1-t  )* cell[1].velocity[1] ;
    //}
//    ptxy.velocity[2] =  (1-s)*(1-t  )* cell[0].velocity[2] +
//                        (1-s)*(t    )* cell[3].velocity[2] +
//                        (s  )*(t    )* cell[2].velocity[2] +
//                        (s  )*(1-t  )* cell[1].velocity[2] ;

    return ptxy;

}

void locateGridCell(Vec_GridPoint seedPoint){


    int i = int(seedPoint.coord_x);
    int j = int(seedPoint.coord_y);
    int k = int(seedPoint.coord_z);

    cell[0].coord_x = i; cell[0].coord_y = j;
    cell[0].velocity[0] = vector_data[i][j][k][0];     cell[0].velocity[1] = vector_data[i][j][k][1];


    cell[1].coord_x = (i+1); cell[1].coord_y = (j);
    cell[1].velocity[0] = vector_data[i+1][j][k][0];     cell[1].velocity[1] = vector_data[i+1][j][k][1];


    cell[2].coord_x = (i+1); cell[2].coord_y = (j+1);
    cell[2].velocity[0] = vector_data[i+1][j+1][k][0];     cell[2].velocity[1] = vector_data[i+1][j+1][k][1];


    cell[3].coord_x = (i); cell[3].coord_y = (j+1);
    cell[3].velocity[0] = vector_data[i][j+1][k][0];     cell[3].velocity[1] = vector_data[i][j+1][k][1];


}
Vec_GridPoint EulersLI(Vec_GridPoint initial_pt){

    Vec_GridPoint final_pt;
    locateGridCell(initial_pt);

    Vec_GridPoint gp = velocity_value_func(initial_pt);

    float v_mag = sqrt(gp.velocity[0]*gp.velocity[0] + gp.velocity[1]*gp.velocity[1]);
    //cout<<"Magnitude:" << v_mag<<endl;
    float unit_vx = gp.velocity[0]/v_mag;
    float unit_vy = gp.velocity[1]/v_mag;
    //cout<<"Unit_VX:"<< unit_vx << " Unit_VY:"<<unit_vy<<endl;

    final_pt.coord_x = float(round(initial_pt.coord_x + 0.5 + unit_vx));
    final_pt.coord_y = float(round(initial_pt.coord_y + 0.5 + unit_vy));
    final_pt.coord_z = initial_pt.coord_z;

    return final_pt;

}

Vec_GridPoint RK4(Vec_GridPoint initial_pt, float h){

    Vec_GridPoint final_pt;
    Vec_GridPoint delta1, delta2, delta3;


    float k1, k2, k3, k4;

    locateGridCell(initial_pt);

    Vec_GridPoint gp = velocity_value_func(initial_pt);

    k1 = h * (gp.velocity[0]);

    delta1.coord_x = initial_pt.coord_x + (0.5) * h;
    delta1.coord_y = initial_pt.coord_y + (0.5) * k1;
    delta1.coord_z = initial_pt.coord_z;

    //cell = locateGridCell(delta1);
    locateGridCell(delta1);

    k2 = h * velocity_value_func(delta1).velocity[0];

    delta2.coord_x = initial_pt.coord_x + (0.5) * h;
    delta2.coord_y = initial_pt.coord_y + (0.5) * k2;
    delta2.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta2);

    k3 = h * velocity_value_func(delta2).velocity[0];

    delta3.coord_x = initial_pt.coord_x + h;
    delta3.coord_y = initial_pt.coord_y + k3;
    delta3.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta3);

    k4 = h * velocity_value_func(delta3).velocity[0];

    final_pt.coord_x = initial_pt.coord_x +h;
    final_pt.velocity[0] = initial_pt.velocity[0] + (1.0/6.0)*(k1+ 2*k2 + 2*k3 + k4);
    //final_pt.velocity[0] =

    // RK4 in Y direction

    //cell =
    locateGridCell(initial_pt);

    k1 = h * velocity_value_func(initial_pt).velocity[1];

    delta1.coord_x = initial_pt.coord_x + (0.5) * k1;
    delta1.coord_y = initial_pt.coord_y + (0.5) * h;
    delta1.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta1);

    k2 = h * velocity_value_func(delta1).velocity[1];

    delta2.coord_x = initial_pt.coord_x + (0.5) * k2;
    delta2.coord_y = initial_pt.coord_y + (0.5) * h;
    delta2.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta2);

    k3 = h * velocity_value_func(delta2).velocity[1];

    delta3.coord_x = initial_pt.coord_x + k3;
    delta3.coord_y = initial_pt.coord_y + h;
    delta3.coord_z = initial_pt.coord_z;

    //cell =
    locateGridCell(delta3);

    k4 = h * velocity_value_func(delta3).velocity[1];

    final_pt.coord_y = initial_pt.coord_y +h;
    final_pt.velocity[1] = initial_pt.velocity[1] + (1.0/6.0)*(k1+ 2*k2 + 2*k3 + k4);

    final_pt.coord_z = initial_pt.coord_z;
    final_pt.velocity[2] = initial_pt.velocity[2];

    return final_pt;

}


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


void drawText(const char *text, int length, int x, int y){

    glMatrixMode(GL_PROJECTION);
    double matrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();

//    float x_t = -1.0 + float(x+300)/500.0;
//    float y_t = -1.0 + float(y+300)/500.0;

    float x_t = float(x+300)/1000.0;
    float y_t = float(y+300)/1000.0;


    glRasterPos2i(0.9,-0.9);

    for(int i=0; i< length;i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)text[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();
}

void renderColorMap(){

    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glBegin(GL_LINES);

    glColor3f(0.0f,0.0f,1.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(649.0,0.0);

    glColor3f(0.0f,1.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,299.0);

    glEnd();
    glPopMatrix();

//    glPushMatrix();
//
//    string text;
//    text="X";
//    glColor3f(1,0,0);
//    drawText(text.data(),text.size(),650,0);
//    glPopMatrix();
//    glPushMatrix();
//    text="Y";
//    glColor3f(1,0,0);
//    drawText(text.data(),text.size(),0,300);
//
//    glPopMatrix();

    glBegin(GL_QUADS);

    for(int y=0;y<(248-1);y++){
        for(int x=0;x<(600-1);x++){
            //for vertex(x,y)
            glColor3f(norm_scalar_data[x][y][z_key][scalar_index],0.0f,0.0f);
            glVertex2f(x,y);

            //for vertex (x+1,y)
            glColor3f(norm_scalar_data[(x+1)][y][z_key][scalar_index],0.0f,0.0f);
            glVertex2f((x+1),y);

            //for vertex (x+1,y+1)
            glColor3f(norm_scalar_data[(x+1)][(y+1)][z_key][scalar_index],0.0f,0.0f);
            glVertex2f((x+1),(y+1));

            //for vertex (x,y+1)
            glColor3f(norm_scalar_data[x][(y+1)][z_key][scalar_index],0.0f,0.0f);
            glVertex2f(x,(y+1));

            }
        }

    glEnd();

    glBegin(GL_QUADS);

        glColor3f(0.0f,0.0f,0.0f);
        glVertex2f(150.0,-120.0);
        glColor3f(0.0f,0.0f,0.0f);
        glVertex2f(150.0,-150.0);
        glColor3f(1.0f,0.0f,0.0f);
        glVertex2f(450.0,-150.0);
        glColor3f(1.0f,0.0f,0.0f);
        glVertex2f(450.0,-120.0);



    glEnd();

    glutSwapBuffers();

}
void renderHeightMap(){


    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
//    gluLookAt(-0.1,0.5, 0.5,  /* eye is at (-0.1,0.5, 0.5) */
//    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
//    0.0, 1.0, 0.);
//
//    glRotatef( counter , 1.0, 1.0 , 1.0 );
//    counter+=0.01;
//    if(counter>=1)counter=-1;
    glPushMatrix();
    glBegin(GL_LINES);

    glColor3f(1.0f,0.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(649.0,0.0);

    glColor3f(0.0f,1.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,299.0);

    glEnd();
    glPopMatrix();

    glBegin(GL_TRIANGLE_STRIP);

    for(int x=0;x<(600-1);x++){
        for(int y=0;y<(248-1);y++){
            //for vertex (x,y)
            glColor3f(0.0f,0.0f,norm_scalar_data[x][y][z_key][scalar_index]);
            glVertex3f(x,250*norm_scalar_data[x][y][z_key][scalar_index],y);

            //for vertex (x+1,y)
            glColor3f(0.0f,0.0f,norm_scalar_data[(x+1)][y][z_key][scalar_index]);
            glVertex3f((x+1),250*norm_scalar_data[(x+1)][y][z_key][scalar_index],y);

            //for vertex (x,y+1)
            glColor3f(0.0f,0.0f,norm_scalar_data[x][(y+1)][z_key][scalar_index]);
            glVertex3f(x,250*norm_scalar_data[x][(y+1)][z_key][scalar_index],(y+1));

            //for vertex (x+1,y+1)
            glColor3f(0.0f,0.0f,norm_scalar_data[(x+1)][(y+1)][z_key][scalar_index]);
            glVertex3f((x+1),250*norm_scalar_data[(x+1)][(y+1)][z_key][scalar_index],(y+1));

        }
    }

    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.0f,0.0f,0.0f);
        glVertex2f(150.0,-120.0);
        glColor3f(0.0f,0.0f,0.0f);
        glVertex2f(150.0,-150.0);
        glColor3f(0.0f,0.0f,1.0f);
        glVertex2f(450.0,-150.0);
        glColor3f(0.0f,0.0f,1.0f);
        glVertex2f(450.0,-120.0);
    glEnd();

    glutSwapBuffers();

}
void renderContourMap(){

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glBegin(GL_LINES);

    glColor3f(0.0f,0.0f,1.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(649.0,0.0);

    glColor3f(0.0f,1.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,299.0);

    glEnd();
    glPopMatrix();


    glBegin(GL_QUADS);
    for(int y=0;y<(248-1);y++){
        for(int x=0;x<(600-1);x++){
            //for vertex(x,y)
            glColor3f(norm_scalar_data[x][y][z_key][scalar_index],0.0f,0.0f);
            glVertex2f(x,y);

            //for vertex (x+1,y)
            glColor3f(norm_scalar_data[(x+1)][y][z_key][scalar_index],0.0f,0.0f);
            glVertex2f((x+1),y);

            //for vertex (x+1,y+1)
            glColor3f(norm_scalar_data[(x+1)][(y+1)][z_key][scalar_index],0.0f,0.0f);
            glVertex2f((x+1),(y+1));

            //for vertex (x,y+1)
            glColor3f(norm_scalar_data[x][(y+1)][z_key][scalar_index],0.0f,0.0f);
            glVertex2f(x,(y+1));

            }
        }

    glEnd();



    GridPoint gp_array[4];
    LineSegment ls;
    SaddleSegments ss;


    int case_num =0;

    float cut_off[5] = {500.0f, 1000.0f, 2000.f, 3000.0f, 4000.0f};

    glBegin(GL_LINES);



    for(int contour_index = 0; contour_index <5 ; contour_index++){

        switch(contour_index){
        case 0  :   glColor3f(0.0f,0.0f,0.0f); break;
        case 1  :   glColor3f(0.5f,0.5f,0.0f); break;
        case 2  :   glColor3f(0.0f,0.5f,0.5f); break;
        case 3  :   glColor3f(0.5f,0.0f,0.5f); break;
        case 4  :   glColor3f(0.5f,0.5f,0.5f); break;
        }

        for(int y=0;y<(248-1);y++){
            for(int x=0;x<(600-1);x++){

                gp_array[0].coord_x=x;
                gp_array[0].coord_y=y;
                gp_array[0].scalar_value=scalar_data[x][y][z_key][scalar_index];

                gp_array[1].coord_x=(x+1);
                gp_array[1].coord_y=y;
                gp_array[1].scalar_value=scalar_data[(x+1)][y][z_key][scalar_index];

                gp_array[2].coord_x=(x+1);
                gp_array[2].coord_y=(y+1);
                gp_array[2].scalar_value=scalar_data[(x+1)][(y+1)][z_key][scalar_index];

                gp_array[3].coord_x=x;
                gp_array[3].coord_y=(y+1);
                gp_array[3].scalar_value=scalar_data[x][(y+1)][z_key][scalar_index];

                case_num = getCaseNumberByGridPointsCutoff(cut_off[contour_index], gp_array);

                if(case_num!=0 && case_num!=15){
                    if(case_num!=5 && case_num!=10){

                        ls = getLineSegmentByCase(case_num, gp_array);
                        glVertex2f(ls.point1.coord_x,ls.point1.coord_y);
                        glVertex2f(ls.point2.coord_x,ls.point2.coord_y);

                    }else{
                            ss = getSaddleSegments(case_num, gp_array);
                            glVertex2f(ss.segment_a.point1.coord_x,ss.segment_a.point1.coord_y);
                            glVertex2f(ss.segment_a.point2.coord_x,ss.segment_a.point2.coord_y);

                            glVertex2f(ss.segment_b.point1.coord_x,ss.segment_b.point1.coord_y);
                            glVertex2f(ss.segment_b.point2.coord_x,ss.segment_b.point2.coord_y);
                        }

                }

            }
        }
    }

    glEnd();
    glutSwapBuffers();

}


void renderHedgeHog(){


    //glClearColor(0.0f, 0.0f,0.0f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glBegin(GL_LINES);

    glColor3f(1.0f,0.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(649.0,0.0);

    glColor3f(0.0f,1.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,299.0);

    glEnd();
    glPopMatrix();


    glBegin(GL_LINES);

    float norm_vec_mag = 0.0;
    float vec_mag = 0.0;

    for(int y=0;y<248;y+=10){
        for(int x=0;x<600;x+=10){

            vec_mag = getVectorMagnitude(vector_data[x][y][z_key][0],vector_data[x][y][z_key][1],vector_data[x][y][z_key][2]);
            norm_vec_mag = (vec_mag-min_value)/(max_value-min_value);


            glColor3f(0.0f,0.2f,norm_vec_mag);

            glVertex2d(x, y);
            glVertex2d((x+vector_data[x][y][z_key][0]), (y+vector_data[x][y][z_key][1]));

        }
    }

    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.0f,0.2f,0.0f);
        glVertex2f(150.0,-120.0);
        glColor3f(0.0f,0.2f,0.0f);
        glVertex2f(150.0,-150.0);
        glColor3f(0.0f,0.2f,1.0f);
        glVertex2f(450.0,-150.0);
        glColor3f(0.0f,0.2f,1.0f);
        glVertex2f(450.0,-120.0);
    glEnd();
    glutSwapBuffers();

}
void renderStreamLines(){

    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glBegin(GL_LINES);

    glColor3f(1.0f,0.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(649.0,0.0);

    glColor3f(0.0f,1.0f,0.0f);
    glVertex2f(0.0,0.0);
    glVertex2f(0.0,299.0);

    glEnd();
    glPopMatrix();
    float h = 0.5;
    int num_iter = 0;

    Vec_GridPoint final_pt, initial_pt;

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f,0.0f,0.5f);
    cout<<"Prinitng stream Lines"<<endl;

    cout<<"(350,124) ("<< vector_data[int(seedPoints[0].coord_x)][int(seedPoints[0].coord_y)][int(seedPoints[0].coord_z)][0]<<","<<vector_data[350][124][0][1]<<endl;

 for(int i=0; i<10 ; i++){

        initial_pt = seedPoints[i];
        initial_pt.coord_z = z_key;
        glBegin(GL_POINT);
            glPointSize(500.0);
            glColor3f(1.0,0.0,0.0);
            glVertex2f(initial_pt.coord_x,initial_pt.coord_y);

        glEnd();

        final_pt.coord_x = 2.0;
        final_pt.coord_y = 2.0;
        glBegin(GL_LINE_STRIP);
        glColor3f(0.0f,0.0f,1.0f);
        while(num_iter < MAX_NUM_ITER && (final_pt.coord_x!=seedPoints[i].coord_x && final_pt.coord_y!=seedPoints[i].coord_y)
              && final_pt.coord_x < 599 && final_pt.coord_x > 0 && final_pt.coord_y < 247 && final_pt.coord_y > 0){

              if(integration_fn==0){
                    final_pt = EulersLI(initial_pt);
                }else{
                    final_pt = RK4(initial_pt, h);
                }

                glVertex2d(initial_pt.coord_x, initial_pt.coord_y);
                glVertex2d(final_pt.coord_x, final_pt.coord_y);
//                cout<<"Line Point1: ("<< initial_pt.coord_x <<","<<initial_pt.coord_y<<")"<<endl;
//                cout<<"Line Point2: ("<< final_pt.coord_x <<","<<final_pt.coord_y<<")"<<endl;
                initial_pt = final_pt;

                num_iter++;

        }


    }


    glEnd();

    glutSwapBuffers();


}

void reshape(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glOrtho(-300.0, 700.0, -300.0, 700.0, -700.0, 700.0);
    glMatrixMode(GL_MODELVIEW);

}
void initOpenGL(){

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

}

//switching will happen based on the menu value selected
void displaySwitch(){


  if(menu_value<=8){

    int switch_val = menu_value/3;
    scalar_index=menu_value%3;
    switch (switch_val){

        case 0: renderColorMap();
                break;
        case 1: renderHeightMap();
                break;
        case 2: renderContourMap();
                break;
    }

  } else if(menu_value>8 && menu_value<=11) {



        switch (menu_value){

        case 9:     renderHedgeHog();
                    break;
        case 10:    integration_fn = 0;
                    renderStreamLines();
                    break;
        case 11:    integration_fn = 1;
                    renderStreamLines();
                    break;

        }

  }

}
/* executed when program is idle */
void idle() {

}
/* executed when button 'button' is put into state 'state' at screen position ('x', 'y') */
void mouseClick(int button, int state, int x, int y) {

}

/* executed when the mouse moves to position ('x', 'y') */
void mouseMotion(int x, int y) {

}

int main(int argc, char **argv){

    scalar_file_name = argv[1];
    vector_file_name = argv[2];

    if(argc>3){
        z_test = stoi(argv[3]);
        cout<<"z_test "<<z_test<<endl;
    }

    int scalar_extract = extractScalarData();
    if(scalar_extract==0){
        cout<<endl<<"Could not extract Scalar Data"<<endl;
        return -1;
    }

    int vector_extract = extractVectorData();
    if(vector_extract==0){
        cout<<endl<<"Could not extract Vector Data"<<endl;
        return -1;
    }

    int norm_data_done = normalize_scalar_data();

    if(scalar_extract!=0 && vector_extract !=0 && norm_data_done !=0){


    cout<<"Setting 10 SeedPoints for Vector Viz"<<endl;

    for(int j=0;j<10;j++){
            seedPoints[j].coord_x = float(rand()%598);
            seedPoints[j].coord_y = float(rand()%246);
            seedPoints[j].coord_z = 0.0;
            cout<<"SeedPoint "<< j<<" ("<< seedPoints[j].coord_x <<","<< seedPoints[j].coord_y<<")"<<endl;
    }

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    window=glutCreateWindow("PH2017001_A1");

    glutSpecialFunc(processSpecialKeys);

    glutIdleFunc(idle);
    glutIgnoreKeyRepeat(true); // ignore keys held down

    createMenu();


    initOpenGL();
    glutDisplayFunc(displaySwitch);
    glutReshapeFunc(reshape);
    glutMainLoop();
    }
    return 0;
}
