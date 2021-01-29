
#include "multiviewwindow.h"
#include "objectreader.h"


MultiviewWindow::MultiviewWindow(Activity chosenactivity, int activity_ID)
{
     hlayout = new QHBoxLayout;
     glayout1 = new QGridLayout;
     glayout2 = new QGridLayout;

     camera = vtkCamera::New();
     //camera->SetPosition(0, 0, 20);
     //camera->SetFocalPoint(0, 0, 0); 

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

     vtkwidget1 = new QVTKWidget;
      
     renderWindow1 = vtkSmartPointer<vtkRenderWindow>::New();
     renderer1 = vtkSmartPointer<vtkRenderer>::New();      
     renderWindow1->AddRenderer(renderer1);

     int step_num = chosenactivity.endframe - chosenactivity.startframe + 1;

     QFont fnt("Times", 12, QFont::Bold);
     QLabel *title = new QLabel;
     QString title_str = "Activity " + QString::number(activity_ID+1);
     //title->setText("Activity 152");
     title->setText(title_str);
     title->setFont(fnt);
     hlayout->addWidget(title, (int)(step_num/2), Qt::AlignCenter);

     vtkwidget1->setFixedSize(620, 500);
     renderWindow1->SetSize(600, 480);    

     for (unsigned i = 0; i < step_num; i++)
     {    
         renderWindow1->AddRenderer(renderer1);
         int object_num = chosenactivity.timestep[i].object_num;
         int *objectlist = new int [object_num];
         for (int j=0; j<object_num; j++)
             objectlist[j] = chosenactivity.timestep[i].objectlist[j];

         polydata = vtkPolyData::New();
         polydata = object_reader(chosenactivity.filename[i], object_num, objectlist, QColor(255, 255, 255), false);

         string extension = ".uocd";
         string polyfilename = chosenactivity.filename[i];
         unsigned int pos = polyfilename.find_last_of(".");
         string uocdfilename = polyfilename.substr(0, pos) + extension;
         cout << uocdfilename << endl;

         // Visualize
         mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
         mapper1->SetInputConnection(polydata->GetProducerPort());  
         actor1 = vtkSmartPointer<vtkActor>::New();
         actor1->SetMapper(mapper1);
         actor1->GetProperty()->SetOpacity(0.1 + i*0.9/(step_num-1));
         
         actor1->SetPosition(i*10, 10*i, 0);

         renderer1->AddActor(actor1);
         renderer1->SetBackground(1,1,1); // Background color white
         renderer1->ResetCamera();
     }

     vtkwidget1->SetRenderWindow(renderWindow1); 
     vlayout = new QVBoxLayout;
     vlayout->addLayout(hlayout);
     vlayout->addWidget(vtkwidget1);
     this->setLayout(vlayout);

     vtkwidget1->GetRenderWindow()->AddRenderer(renderer1);

     renderWindowInteractor = vtkwidget1->GetRenderWindow()->GetInteractor();
     OrientationWidget->SetInteractor( renderWindowInteractor );
     OrientationWidget->SetViewport( 0.0, 0.0, 0.3, 0.3 );
     OrientationWidget->SetEnabled( 1 );
     OrientationWidget->InteractiveOn();     

     vtkwidget1->GetRenderWindow()->Render();
}
