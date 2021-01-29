
#include "multiviewwindow.h"
#include "objectreader.h"


MultiviewWindow::MultiviewWindow(Activity chosenactivity)
{
     camera = vtkCamera::New();
     camera->SetPosition(0, 0, 20);
     camera->SetFocalPoint(0, 0, 0); 

     vtkwidget1 = new QVTKWidget;
     vtkwidget1->setFixedSize(540,320); 
     vtkwidget2 = new QVTKWidget;
     vtkwidget2->setFixedSize(540,320);

     renderWindow1 = vtkSmartPointer<vtkRenderWindow>::New();
     renderWindow2 = vtkSmartPointer<vtkRenderWindow>::New();

     renderer2 = vtkSmartPointer<vtkRenderer>::New();      
     renderWindow2->AddRenderer(renderer2);
     renderWindow2->SetSize(540,320); 
  
     double *xmins, *xmaxs, *ymins, *ymaxs; 
     int step_num = chosenactivity.endframe - chosenactivity.startframe + 1;

     xmins = new double [step_num];
     xmaxs = new double [step_num];
     ymins = new double [step_num];
     ymaxs = new double [step_num];

     renderWindow1->SetSize(540,320);  

     for (unsigned i = 0; i < step_num; i++)
     {
         xmins[i] = (double)(i)/step_num;
         xmaxs[i] = (double)(i)/step_num+1.0/step_num;
         ymins[i] = 0;
         ymaxs[i] = 1;   

         renderer1 = vtkSmartPointer<vtkRenderer>::New();      
         renderWindow1->AddRenderer(renderer1);
         renderer1->SetViewport(xmins[i],ymins[i],xmaxs[i],ymaxs[i]);
         renderer1->SetActiveCamera(camera);

         renderWindow2->AddRenderer(renderer2);

         int object_num = chosenactivity.timestep[i].object_num;
         int *objectlist = new int [object_num];
         for (int j=0; j<object_num; j++)
             objectlist[j] = chosenactivity.timestep[i].objectlist[j];

         polydata = vtkPolyData::New();
         polydata = object_reader(chosenactivity.filename[i], object_num, objectlist);

         // Visualize
         mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
         mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
         mapper1->SetInputConnection(polydata->GetProducerPort());
         mapper2->SetInputConnection(polydata->GetProducerPort());  
         actor1 = vtkSmartPointer<vtkActor>::New();
         actor1->SetMapper(mapper1);

         actor2 = vtkSmartPointer<vtkActor>::New();
         actor2->SetMapper(mapper2);

         int gap = (int)(270/step_num);
         actor2->SetPosition(gap*i, 0, 0);

         // Create the outline
         outline = vtkSmartPointer<vtkOutlineFilter>::New();
         #if VTK_MAJOR_VERSION <= 5
         outline->SetInput(polydata);
         #else
         outline->SetInputData(polydata);
         #endif

         outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
         outlineMapper->SetInputConnection(outline->GetOutputPort());
         outlineActor = outlineActor = vtkSmartPointer<vtkActor>::New();
         outlineActor->SetMapper(outlineMapper);
         outlineActor->GetProperty()->SetColor(0,0,0);

         renderer1->AddActor(actor1);
         renderer1->AddActor(outlineActor);
         renderer1->SetBackground(1,1,1); // Background color white
         renderer1->ResetCamera();
  
         renderer2->AddActor(actor2);
         renderer2->SetBackground(1,1,1); // Background color white
         renderer2->ResetCamera();
     }

     vtkwidget1->SetRenderWindow(renderWindow1); 
     vtkwidget2->SetRenderWindow(renderWindow2); 
     vlayout = new QVBoxLayout;
     vlayout->addWidget(vtkwidget1);
     vlayout->addWidget(vtkwidget2);
     this->setLayout(vlayout);

     vtkwidget1->GetRenderWindow()->AddRenderer(renderer1);
     vtkwidget1->GetRenderWindow()->Render();

     vtkwidget2->GetRenderWindow()->AddRenderer(renderer2);
     vtkwidget2->GetRenderWindow()->Render();
}
