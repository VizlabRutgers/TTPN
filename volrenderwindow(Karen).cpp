#include "volrenderwindow.h"
#include <ctime> 
#include <cerrno>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <vector>
#include <string>

using namespace std;

bool comparevalue(const element &a, const element &b )  
{  
     //return a.value < b.value;   //(ascending order)  
     return a.value > b.value;     //(descending order)  
}

VolRenderWindow::VolRenderWindow(Activity chosenactivity, int activity_ID)
{
     vlayout = new QVBoxLayout;
     
     camera = vtkCamera::New();
     camera->SetPosition(0, -1, 0);
     camera->SetFocalPoint(0, 0, 0);  


     axes = vtkSmartPointer<vtkAxesActor>::New();
     axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(100);
     axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetColor(0, 0, 0);
     axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->BoldOff();
     axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(100);
     axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetColor(0, 0, 0);
     axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->BoldOff();
     axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(100);
     axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetColor(0, 0, 0);
     axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->BoldOff(); 

     OrientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New(); 
     OrientationWidget->SetOutlineColor( 0.9300, 0.5700, 0.1300 );
     OrientationWidget->SetOrientationMarker( axes );








     vtkwidget = new QVTKWidget;
      
     renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
     renderer = vtkSmartPointer<vtkRenderer>::New();      
     renderWindow->AddRenderer(renderer);
     //renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();   //设置绘制交互操作窗口 
     //renderWindowInteractor->SetRenderWindow(renderWindow);    
    
     vtkwidget->setFixedSize(325, 305);
     renderWindow->SetSize(320, 300);   


     int step_num = chosenactivity.endframe - chosenactivity.startframe + 1;

     rows = 161;
     cols = 121;
     slices = 181;

     QFont fnt("Times", 16, QFont::Bold);
     QLabel *title = new QLabel;
     QString title_str = "Activity 5";
     title->setText(title_str);
     title->setFont(fnt);

     
     
     //unsigned char *data = new unsigned char [rows*cols*slices];
     float *data = new float [rows*cols*slices];
     float *valuemap = new float [rows*cols*slices];
     int *frequencymap = new int [rows*cols*slices];

     for (long ii=0; ii<rows*cols*slices; ii++)
     {
         data[ii] = 0;
         valuemap[ii] = 0;
         frequencymap[ii] = 0;
     } 
       
     unsigned long totalvolume = 0, averagevolume = 0;
/*
     for (unsigned i = 0; i < step_num-1; i++)              //根据Karen胖胖的建议，在计算平均时排除最后一个 
     {    
         int object_num = chosenactivity.timestep[i].object_num;
         int *objectlist = new int [object_num];
         for (int j=0; j<object_num; j++)
             objectlist[j] = chosenactivity.timestep[i].objectlist[j];
    
         string extension = ".uocd";
         string polyfilename = chosenactivity.filename[i];
         unsigned int pos = polyfilename.find_last_of(".");
         string uocdfilename = polyfilename.substr(0, pos) + extension;
         
         unsigned long thisvolume = volumereader(uocdfilename, object_num, objectlist, frequencymap, valuemap, rows, cols, slices);            
         totalvolume = totalvolume + thisvolume;
     }
*/


     QString eventFile = "/home/liliu/eventkaren.list"; 
     QByteArray ba = eventFile.toLatin1();
     char *name = ba.data();

     ifstream infile;
     infile.open(name);

     string event, temp;
     int *eventFrame = new int [300];
     int *eventObject = new int [300];
     int *eventPlace = new int [300];
     int num_eventFrame = 0;
     int num_eventObject = 0;
     int num_eventPlace = 0;

     while(!infile.eof())
     {
         getline(infile, event);
         stringstream os(event);
       
         int num = 0;
         while(os >> temp)
         {
             if (num%3==0)
             {
                eventObject[num_eventObject] = atoi(temp.c_str());    
                num_eventObject++;
             }            

             else if (num%3==1)
             {
                eventFrame[num_eventFrame] = atoi(temp.c_str());
                num_eventFrame++;  
              }         
               
             else if(num%3==2)  
             {          
                eventPlace[num_eventPlace] = atoi(temp.c_str());   
                num_eventPlace++; 
             }                     
             num++;
            
         }          
     } 

/*     //plume place2
     int group[11];
     group[0] = 55;
     group[1] = 56;
     group[2] = 57;
     group[3] = 58;
     group[4] = 59;
     group[5] = 60;
     group[6] = 61;
     group[7] = 62;
     group[8] = 63;
     group[9] = 64;
     group[10] = 65;


     int group2[11];
     group2[0] = 1;
     group2[1] = 2;
     group2[2] = 1;
     group2[3] = 1;
     group2[4] = 1;
     group2[5] = 1;
     group2[6] = 1;
     group2[7] = 1;
     group2[8] = 2;
     group2[9] = 2;
     group2[10] = 1;

     step_num = 0;

     for (int i=0; i<11; i++)
     {        
         step_num++;  
         int object_num = 1;        
         int *objectlist = new int [1];
         objectlist[0] = group2[i];
    
         string extension = ".uocd";
         stringstream ss;        
         ss << group[i]; 

         string uocdfilename = "/home/liliu/Plume/PResults/p" + ss.str() + extension;
               
         unsigned long thisvolume = volumereader(uocdfilename, object_num, objectlist, frequencymap, valuemap, rows, cols, slices);            
         totalvolume = totalvolume + thisvolume;              
     }
*/


/*  //plume place3
     int group[5];
     group[0] = 30;
     group[1] = 54;
     group[2] = 102;
     group[3] = 113;
     group[4] = 139;

     int group2[5];
     group2[0] = 1;
     group2[1] = 1;
     group2[2] = 3;
     group2[3] = 1;
     group2[4] = 1;

     step_num = 0;

     for (int i=0; i<5; i++)
     {        
         step_num++;  
         int object_num = 1;        
         int *objectlist = new int [1];
         objectlist[0] = group2[i];
    
         string extension = ".uocd";
         stringstream ss;        
         ss << group[i]; 

         string uocdfilename = "/home/liliu/Plume/PResults/p" + ss.str() + extension;
               
         unsigned long thisvolume = volumereader(uocdfilename, object_num, objectlist, frequencymap, valuemap, rows, cols, slices);            
         totalvolume = totalvolume + thisvolume;              
     }
*/



/*
     for (int i=0; i<num_eventFrame; i++)
     {
         if (eventPlace[i]==6)
         {
            step_num++;  
            int object_num = 1;        
            int *objectlist = new int [1];
            objectlist[0] = eventObject[i];
    
            string extension = ".uocd";
            stringstream ss;        
            ss << eventFrame[i]; 

            string uocdfilename = "/home/liliu/Plume/PResults/p" + ss.str() + extension;
         
            unsigned long thisvolume = volumereader(uocdfilename, object_num, objectlist, frequencymap, valuemap, rows, cols, slices);            
            totalvolume = totalvolume + thisvolume;
         }        
     }
*/



     int object_num = 1;
     int *objectlist = new int [object_num];
     objectlist[0] = 3;
     string uocdfilename = "/home/liliu/Plume/PResults/p149.uocd";
         
     unsigned long thisvolume = volumereader(uocdfilename, object_num, objectlist, frequencymap, valuemap, rows, cols, slices);            
         totalvolume = thisvolume;
     step_num = 1;

     averagevolume = totalvolume/step_num;
     cout << "Volume: " << averagevolume << endl; 


     vector< element > dataspace;  

     for (int i=0; i<rows; i++)
         for (int j=0; j<cols; j++)
             for (int k=0; k<slices; k++) 
     {
         int times = *(frequencymap + i + j*rows + k*rows*cols);  
         float value = *(valuemap + i + j*rows + k*rows*cols);   
         value = value/step_num;        
         dataspace.push_back( element(times, value, i, j, k) );  
     }

     sort(dataspace.begin(), dataspace.end(), comparevalue);

     for (unsigned long i=0; i<averagevolume; i++)
     {
         int x_temp = dataspace[i].x;
         int y_temp = dataspace[i].y;
         int z_temp = dataspace[i].z;

         float value = dataspace[i].value;

         data[x_temp + y_temp*rows +z_temp*rows*cols] = value;
     }

     
     FILE *write_ptr;
     write_ptr = fopen("averageactor.vol", "wb");  
     fwrite(data, rows*cols*slices, sizeof(float), write_ptr);
     fclose(write_ptr);         

     cout << "Average activity actor saved!" << endl;

     SaveVTKFile("averageactor.vol", rows, cols, slices);
      
/*
     vtkImageReader *reader = vtkImageReader::New();
     reader->SetFileName("averageactor.vol");
     reader->SetFileDimensionality(3);                      //设置显示图像的维数
     reader->SetDataScalarTypeToUnsignedChar();
     reader->SetDataExtent(0, rows-1, 0, cols-1, 0, slices-1);        

     reader->SetDataSpacing (1,1,1); 
     vtkImageCast *readerImageCast = vtkImageCast::New();   //数据类型转换
     readerImageCast->SetInputConnection(reader->GetOutputPort());
     readerImageCast->SetOutputScalarTypeToUnsignedChar();
     //readerImageCast->ClampOverflowOn();                 //阀值
  
     vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();     //一维分段函数变换
     opacityTransferFunction->AddPoint(20, 0.0);
     opacityTransferFunction->AddPoint(255, 1);
     
     vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
     colorTransferFunction->AddRGBPoint(   0, 1.0, 1.0, 1.0 );
     colorTransferFunction->AddRGBPoint( 125, 1.0, 0.0, 0.0 );
     colorTransferFunction->AddRGBPoint( 255, 1.0, 1.0, 0.0 );


     vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();  //设定一个体绘容器的属性                                              
     volumeProperty->SetColor(colorTransferFunction);               //设置颜色
     volumeProperty->SetScalarOpacity(opacityTransferFunction);     //不透明度
     volumeProperty->ShadeOn();                                     //影阴
     volumeProperty->SetInterpolationTypeToLinear();                //直线与样条插值之间逐发函数
     volumeProperty->SetAmbient(0.2);                               //环境光系数
     volumeProperty->SetDiffuse(0.9);                               //漫反射
     volumeProperty->SetSpecular(0.2);                              //高光系数
     volumeProperty->SetSpecularPower(10);                          //高光强度 
	 

     vtkVolumeRayCastCompositeFunction *compositeFunction =  vtkVolumeRayCastCompositeFunction::New();      //运行沿着光线合成
     vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();   //体绘制器
     volumeMapper->SetVolumeRayCastFunction(compositeFunction);              //载入绘制方法
     volumeMapper->SetInputConnection(readerImageCast->GetOutputPort());     //图像数据输入
     volumeMapper->SetNumberOfThreads(3);
      
     vtkVolume *volume = vtkVolume::New();       //表示透示图中的一组三维数据
     volume->SetMapper(volumeMapper);
     volume->SetProperty(volumeProperty);       //设置体属性

     renderer->AddVolume(volume);                    //将Volume装载到绘制类中
     renderer->SetBackground(1, 0, 0);
     renderer->SetBackground(255, 255, 255);



     vtkSmartPointer<vtkOutlineFilter> outlineGrid= vtkSmartPointer<vtkOutlineFilter>::New();
     outlineGrid->SetInputConnection(readerImageCast->GetOutputPort()); 
     outlineGrid->Update();
     vtkSmartPointer<vtkPolyDataMapper> outlineMapper= vtkSmartPointer<vtkPolyDataMapper>::New();
     outlineMapper->SetInput(outlineGrid->GetOutput());
     vtkSmartPointer<vtkActor> outlineActor= vtkSmartPointer<vtkActor>::New();
     outlineActor->SetMapper(outlineMapper);

     renderer->AddActor(outlineActor);


   	  
     //renderer->SetActiveCamera(camera); 

     vtkwidget->SetRenderWindow(renderWindow); 
     vtkwidget->GetRenderWindow()->AddRenderer(renderer);

     vtkwidget->GetRenderWindow()->Render();


     QFont fnt2("Times", 12, QFont::Bold);
     QLabel *lable1 = new QLabel;
     //lable1->setText("Volume: 5889");    //event29
     lable1->setText("Volume: 6159");
     lable1->setFont(fnt2);

     QLabel *lable2 = new QLabel;
     //lable2->setText("Centroid: [26.16, 59.23, 74.44]");    //event29
     lable2->setText("Centroid: [32.26, 55.05, 66.22]");
     lable2->setFont(fnt2);

     hlayout = new QHBoxLayout;
     hlayout->addWidget(lable1, Qt::AlignCenter);
     hlayout->addWidget(lable2, Qt::AlignCenter);
     
     //vlayout->addWidget(title);
     vlayout->addWidget(vtkwidget);
     vlayout->addLayout(hlayout);
     this->setLayout(vlayout);  
*/
}




unsigned long VolRenderWindow::volumereader(string uocdfile, int object_num, int *objectlist, int *frequencymap, float *valuemap, int rows, int cols, int slices)
{
     //cout << uocdfile << endl;

     ifstream fp;
     fp.open(uocdfile.c_str(), ios_base::in);
     if (!fp.good())
        cout << "ReadOct:cannot open uocd file " << uocdfile.c_str() << " to read\n";
    
     float time = 0.0;
     fp>>time;
     int numObjs = 0;
     fp>>numObjs;

     float mass = 0.0, cx=0.0, cy=0.0, cz=0.0, Ixx=0.0, Iyy=0.0, Izz=0.0, Ixy=0.0, Iyz=0.0, Izx=0.0;
     unsigned long numNodes = 0, vol = 0, objID = 0;
     register unsigned long i = 0, j = 0;
     unsigned long x = 0, y = 0 , z = 0,  xsize = 0, ysize = 0, zsize = 0, vertID = 0;
     float xPos = 0, yPos = 0, zPos = 0, val = 0, x1 = 0, y1 = 0, z1 = 0;
     unsigned long point_num = 0; 

     unsigned long volume = 0;
     for (int i=0; i<numObjs; i++)
     {
         bool match = false;
         for (int i1=0; i1<object_num; i1++)
         {
             if (objectlist[i1] == i)
                match = true;
         }

         if (match == true)
         {           
            fp>>objID>>vol>>mass>>cx>>cy>>cz>>Ixx>>Iyy>>Izz>>Ixy>>Iyz>>Izx>>vertID>>x1>>y1>>z1>>val;
        
            volume = volume + vol;
            for (int j=0; j<vol; j++)
            {
                fp>>vertID>>x1>>y1>>z1>>val;
                int coordx = (int)(x1+0.5);
                int coordy = (int)(y1+0.5);
                int coordz = (int)(z1+0.5);

                int times = *(frequencymap + coordx + coordy*rows + coordz*rows*cols);
                *(frequencymap + coordx + coordy*rows + coordz*rows*cols) = times + 1;

                float value = *(valuemap + coordx + coordy*rows + coordz*rows*cols);
                *(valuemap + coordx + coordy*rows + coordz*rows*cols) = value + val;
            }
         }
     }

     fp.close();
 
     return volume;
}




void VolRenderWindow::SaveVTKFile(string FileName, int x_dim, int y_dim, int z_dim)
{  
    FILE *pFile;
    float *Xarray, *Yarray, *Zarray;   
    float *OriginalData = new float [x_dim*y_dim*z_dim];

    pFile = fopen(FileName.c_str(), "r");
    unsigned long point_num = fread(OriginalData, sizeof(float), x_dim*y_dim*z_dim, pFile);

    vtkFloatArray *coords[3] = {0, 0, 0};   

    coords[0] = vtkFloatArray::New();
    coords[0]->SetNumberOfTuples((vtkIdType) x_dim);  
    float *xarray = (float *) coords[0]->GetVoidPointer(0);
    for (int i=0; i < x_dim; i++)
        xarray[i] = i;
      
    coords[1] = vtkFloatArray::New();
    coords[1]->SetNumberOfTuples((vtkIdType) y_dim);
    float *yarray = (float *) coords[1]->GetVoidPointer(0);
    for (int i=0; i < y_dim; i++)
        yarray[i] = i;
 
    coords[2] = vtkFloatArray::New();
    coords[2]->SetNumberOfTuples((vtkIdType) z_dim);
    float *zarray = (float *) coords[2]->GetVoidPointer(0);
    for (int i=0; i < z_dim; i++)
        zarray[i] = i;

    vtkRectilinearGrid *rgrid = vtkRectilinearGrid::New();
    rgrid->SetDimensions(x_dim, y_dim, z_dim);
    
    rgrid->SetXCoordinates(coords[0]);
    coords[0]->Delete();
    rgrid->SetYCoordinates(coords[1]);
    coords[1]->Delete();
    rgrid->SetZCoordinates(coords[2]);
    coords[2]->Delete();

    vtkDataSet *mesh; 
    mesh = rgrid;
     
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();   
    scalars->SetNumberOfTuples ((vtkIdType) point_num);
    float *ptr = (float *) scalars->GetVoidPointer (0);
    memcpy (ptr, OriginalData, sizeof (float) * point_num);
    
    vtkDataArray *var = scalars;      
    mesh->GetPointData()->SetScalars(var);

    // Write file
    ofstream mywritefile;
    mywritefile.open("averageactor.vtk");
	
    mywritefile<<"# vtk DataFile Version 3.0 "<<endl;
    mywritefile<<"vtk output "<<endl;
    mywritefile<<"ASCII \nDATASET RECTILINEAR_GRID \nDIMENSIONS ";
    mywritefile<<x_dim<<" "<<y_dim<<" "<<z_dim<<endl; 

    mywritefile<<"X_COORDINATES "<<x_dim<<" float"<<endl;
    for (int i = 0; i < x_dim; i++)
        mywritefile << i << endl;
    mywritefile << endl;

    mywritefile<<"Y_COORDINATES "<<y_dim<<" float"<<endl;	
    for (int i = 0; i < y_dim; i++)
        mywritefile << i << endl;
    mywritefile<<endl;
        
    mywritefile<<"Z_COORDINATES "<<z_dim<<" float"<<endl;	
    for (int i = 0; i < z_dim; i++)
        mywritefile << i << endl;
    mywritefile<<endl;

    mywritefile<<"POINT_DATA "<<x_dim*y_dim*z_dim<<endl;
    mywritefile<<"SCALARS scalars float \nLOOKUP_TABLE default"<<endl;

    for (int i = 0; i < x_dim*y_dim*z_dim; i++)
        mywritefile << OriginalData[i] << endl;
	
    mywritefile.close();

    delete [] OriginalData;
    fclose(pFile);
}

