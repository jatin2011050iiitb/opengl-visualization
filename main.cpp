#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <iomanip>


using namespace std;

float counter =-1;
string input_file_name;
string scalar_file_name;
float min_value=0.0f;
float max_value=0.0f;

float data_array2D[600][248], norm_data_array2D[600][248];

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
    gluLookAt(-0.1,0.5, 0.5,  /* eye is at (0,0,5) */
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

    fin.close();

return result;
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


void display(){

    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(-0.1,0.5, 0.5,  /* eye is at (0,0,5) */
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
//glVertex3f(x*0.001,y*0.001,norm_data_array2D[x][y]);

                //for vertex (x+1,y)
                glColor3f(0.0f,0.0f,norm_data_array2D[(x+1)][y]);
                glVertex3f((x+1)*0.001,norm_data_array2D[(x+1)][y],y*0.001);
//glVertex3f((x+1)*0.001,y*0.001,norm_data_array2D[(x+1)][y]);


                //for vertex (x,y+1)
                glColor3f(0.0f,0.0f,norm_data_array2D[x][(y+1)]);
                glVertex3f(x*0.001,norm_data_array2D[x][(y+1)],(y+1)*0.001);
//glVertex3f(x*0.001,(y+1)*0.001,norm_data_array2D[x][(y+1)]);
                //for vertex (x+1,y+1)
                glColor3f(0.0f,0.0f,norm_data_array2D[(x+1)][(y+1)]);
                glVertex3f((x+1)*0.001,norm_data_array2D[(x+1)][(y+1)],(y+1)*0.001);
//glVertex3f((x+1)*0.001,(y+1)*0.001,norm_data_array2D[(x+1)][(y+1)]);

            }
        }

        glEnd();



    glutSwapBuffers();
}

void reshape(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective( 60, float(w)/float(h),1,10);
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

    string map_type = argv[3];

    cout<<endl<<input_file_name<<" " << map_type << " " << scalarName<<endl;
    int extract_done = extractScalarbyZ(scalar_index,0);
    normalize_data(min_value, max_value);

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Assignment 1");
    initOpenGL();

    switch(scalar_index){

    case 0: if(extract_done==0){
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

    case 1:  if(extract_done==0){
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

    case 10: if(extractScalarbyZ(scalar_index,0)==0){
                if(map_type=="colormap"){    renderColorMap();   }
                if(map_type=="heightmap"){   renderHeightMap();  }
                if(map_type=="contourmap"){  renderContourMap(); }
            }else{
                cout<<"Unable to extract data";
            }
            break;

    default:    cout<<"Invalid MapType or Scalar Name."<<endl;
                cout<<"Valid Values Scalar Names are:   Density,    Pressure    & Curl"<<endl;
                cout<<"Valid Values MapType are:        ColorMap,   HeightMap   & ContourMap"<<endl;
                break;
    }


    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
