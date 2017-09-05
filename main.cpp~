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
string input_file_name;
string scalar_file_name;
float min_value=0.0f;
float max_value=0.0f;
typedef float VECTOR[3];
float curl_mag[600][248][248];

float data_array2D[600][248], norm_data_array2D[600][248];

struct GridPoint{
    float coord_x, coord_y;
    float scalar_value;
};

struct LineSegment {
    GridPoint point1, point2;
};

struct SaddleSegments {
    LineSegment segment_a, segment_b;
};

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
    counter+=0.005;
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
    float cut_off[10] = {5.0f, 20.0f,50.0f,100.0f, 500.0f, 1000.0f, 2000.f, 3000.0f, 4000.0f, 4500.0f};


    glBegin(GL_LINES);

    glColor3f(1.0f,1.0f,0.0f);
    for(int contour_index = 0; contour_index <10 ; contour_index++){
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

    glEnd();

}

void renderHedgeHog(){


}

void renderStreamLines(){
}
int extractScalarbyZ(int scalar_index, int z_value){

    int result = 0;
    float scalar[10];
    ifstream fin(input_file_name.c_str());

    if(fin==NULL){
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
    if(fin==NULL){
        cout<<"Could not open " << input_file_name << " for reading";
        return -2;
    }

    VECTOR row;
    VECTOR vel[600][248][248];
    unsigned x,y,z;
    for(z=0;z<248;z++){
        for(y=0;y<248;y++){
            for(x=0;x<600;x++){
                fin >> row[0] >> row[1] >> row[2];
                 vel[x][y][z]=row;
            }
        }

    }
    fin.close();

    //calculating curl

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
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

}

void initOpenGL(){

    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

}

int main(int argc, char **argv){

    input_file_name = argv[1];


    string scalarName = argv[2];

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
    if(scalarName="vectorfield"){
        scalar_index = 15;
    }

    string map_type = argv[3];

    cout<<endl<<input_file_name<<" " << map_type << " " << scalarName<<endl;
    int extract_done =0;
    if(extract_done!=1){

    }

    normalize_data(min_value, max_value);

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Assignment 1");
    initOpenGL();

    switch(scalar_index){

    case 0: extract_done = extractScalarbyZ(scalar_index,0);
            if(extract_done==1){
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
            }

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

    case 15:extract_done = subsetCurlMag(0);

            if(extract_done==1){
                if(map_type=="hedgehog"){
                    cout<<"\nGenerating ColorMap for Temperature"<<endl;
                    glutDisplayFunc(renderHedgeHog);
                    glutIdleFunc(renderHedgeHog);
                }
                if(map_type=="streamlines"){
                    glutDisplayFunc(renderStreamLines);
                    glutIdleFunc(renderStreamLines);
                }
            }else{
                cout<<"Unable to extract data";
            }
            break;

    default:    cout<<"Invalid MapType or Scalar/Vector Field Name."<<endl;
                cout<<"Valid Values Scalar Fields are:   Density,    Pressure    & Curl"<<endl;
                cout<<"Valid Values Scalar field Maps are:  ColorMap,   HeightMap   & ContourMap"<<endl;
                cout<<"Valid Values Vector Fields are:   Velocity"<<endl;
                cout<<"Valid Values MapType are:        HedgeHog    & StreamLines"<<endl;
                break;
    }


    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}

