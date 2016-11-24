	#include <wx/wx.h>
#include "window.h"
#include <wx/image.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dc.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cmath>
#include <string>

using namespace std; 
static const wxChar *FILETYPES = _T("All files (*.*)|*.*");
IMPLEMENT_APP(BasicApplication)
bool BasicApplication::OnInit()
{
  wxInitAllImageHandlers();
  MyFrame *frame = new MyFrame(_("Basic Frame"), 50, 50, 700, 400);
  frame->Show(TRUE);
  SetTopWindow(frame);
  return TRUE;	
}

MyFrame::MyFrame(const wxString title, int xpos, int ypos, int width, int height): wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height)){

ROI = wxRegion();
UsingROI = false;

  fileMenu = new wxMenu;
  fileMenu->Append(LOAD_FILE_ID, _T("&Open file\tCTRL-O"));  
  fileMenu->AppendSeparator();
  fileMenu->Append(INVERT_IMAGE_ID, _T("&Invert image"));
  fileMenu->Append(SCALE_IMAGE_ID, _T("&Scale image"));
  fileMenu->Append(MY_read_raw, _T("&Read Raw Image"));	
  fileMenu->Append(MY_shift, _T("&Shift"));
  fileMenu->Append(MY_maskImage, _T("&Mask Image"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MY_saltPepper, _T("&Salt-and-Pepper"));
  fileMenu->Append(MY_minFiltering, _T("&Min Filtering"));
  fileMenu->Append(MY_maxFiltering, _T("&Max Filtering"));
  fileMenu->Append(MY_midFiltering, _T("&Midpoint Filtering"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MY_negativeLinearTransform, _T("&Negative Linear Transform"));
  fileMenu->Append(MY_logarithmicFunction, _T("&Logarithmic Function"));
  fileMenu->Append(MY_powerLaw, _T("&Power-Law"));
  fileMenu->Append(MY_randomLookUpTable, _T("&Random Look-up Table"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MY_histogram, _T("&Finding Histogram"));
  fileMenu->Append(MY_histogramNormalisation, _T("&Histogram Normalisation"));
  fileMenu->Append(MY_histogramEqualisation, _T("&Histogram Equalisation"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MY_meanAndStandardDeviation, _T("&Mean & Standard Deviation"));
  fileMenu->Append(MY_simpleThresholding, _T("&Simple Thresholding"));
  fileMenu->Append(MY_automatedThresholding, _T("&Automated Thresholding"));
  fileMenu->AppendSeparator();
  fileMenu->Append(MY_setRoi, _T("&Set ROI"));
  fileMenu->Append(MY_removeRoi, _T("&Remove ROI"));
  fileMenu->Append(MY_undo, _T("&Undo"));
  fileMenu->AppendSeparator();
  fileMenu->Append(SAVE_IMAGE_ID, _T("&Save image"));
  fileMenu->Append(EXIT_ID, _T("E&xit"));

  menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _T("&File"));
  
  SetMenuBar(menuBar);
  CreateStatusBar(3); 
  oldWidth = 0;
  oldHeight = 0;
  loadedImage = 0;

  imgWidth = imgHeight = 0;
  stuffToDraw = 0;
}

MyFrame::~MyFrame(){
/* release resources */
  if(loadedImage){
    loadedImage->Destroy();
    loadedImage = 0;
  }
}
void MyFrame::OnOpenFile(wxCommandEvent & event){
  wxFileDialog *openFileDialog = new wxFileDialog ( this, _T("Open file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);  
  if(openFileDialog->ShowModal() == wxID_OK){
    wxString filename = openFileDialog->GetFilename();
    wxString path = openFileDialog->GetPath();
    printf("Loading image form file...");    
    loadedImage = new wxImage(path); 
    if(loadedImage->Ok()){
      stuffToDraw = ORIGINAL_IMG;  
      printf("Done! \n");    
    }
    else {
      printf("error:...");
      loadedImage->Destroy();
      loadedImage = 0;
    }
    Refresh();
  }    
}
void MyFrame::readRawImage(wxCommandEvent & event){
    	int img;
        int size;
         wxFileDialog *openFileDialog = new wxFileDialog ( this, _T("Open file"), _T(""), _T(""), FILETYPES, wxOPEN, wxDefaultPosition);  
        if(openFileDialog->ShowModal() == wxID_OK){
                wxString filename = openFileDialog->GetFilename();
                wxString path = openFileDialog->GetPath();
                FILE* pInput = NULL;
                if(pInput = fopen(path.char_str(), "r")){
                    fseek(pInput,0,SEEK_END);
                    size = ftell(pInput);
                    fseek(pInput,0,SEEK_SET);
                    unsigned char* buf = new unsigned char[size];
                    unsigned char* nbuf = new unsigned char[size*3];
                    img =fread(buf,sizeof(char), size, pInput);
                     
                    int j=0;
                    for(int i = 0; i < size*3;i=i+3)
                    {
                        nbuf[i] = buf[j];
                        nbuf[i+1] = buf[j];
                        nbuf[i+2] = buf[j];
                        j++;
                    }
                   loadedImage = new wxImage(sqrt(size),sqrt(size),nbuf, false);
                   if(loadedImage->Ok()){
                        stuffToDraw = ORIGINAL_IMG;    
                        printf("Done! \n");    
                     }
                   fclose(pInput);
                }
        }
    	Refresh();
}

void MyFrame::OnInvertImage(wxCommandEvent & event){
    printf("Inverting Image");
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    for( int i=0;i<imgWidth;i++)
       for(int j=0;j<imgHeight;j++){
 				loadedImage->SetRGB(i,j,255-loadedImage->GetRed(i,j), 255-loadedImage->GetGreen(i,j), 255-loadedImage->GetBlue(i,j));
    }
    cout << "Finished Inverting Image.\n";
    Refresh();
}

void MyFrame::OnScaleImage(wxCommandEvent & event){
    cout <<"Starting to Scale Image";
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    wxString inputValue;
    double inputValueDouble;
    inputValue = wxGetTextFromUser(_T("Enter a value between 0 and 2 for the scale:"),_T(""),_T(""),NULL, -1, -1, true);	
   	inputValue.ToDouble(&inputValueDouble);

	if(inputValueDouble>2){
		inputValueDouble = 2;
	}else if(inputValueDouble<0){
		inputValueDouble = 0;
	}
	cout <<"Scaling Image by a factor of "<< inputValueDouble <<"\n";

    for( int i=0;i<imgWidth;i++)
       for(int j=0;j<imgHeight;j++){
 			loadedImage->SetRGB(i,j,inputValueDouble* loadedImage->GetRed(i,j), 
				inputValueDouble* loadedImage->GetGreen(i,j),
				inputValueDouble* loadedImage->GetBlue(i,j));
    } 
    cout << "Finished scaling image\n";
    Refresh();
}

void MyFrame::shift(wxCommandEvent & event){ 
	BackupImage(loadedImage);
	int inputValue = 0; 
	inputValue = wxGetNumberFromUser(_T("Enter a number between -255 and 255 for the image shift:"),_T(""),_T("Get Failback Interval"), 0, -255, 255, NULL, wxDefaultPosition);	
	if(inputValue>=255){
		cout << "Input value was greater than 255";
		inputValue = 255;
	}else if(inputValue <=-255){
		cout << "Input value was less than -255";
		inputValue = -255;
	}

	cout << "Shifting Image by " << inputValue;
	int red;
	int green;
	int blue;
	for(int i=0; i<imgWidth ; i++){
		for(int j=0; j<imgHeight ; j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				red = imageShiftConstraintHelper(loadedImage->GetRed(i,j) + inputValue);
				green = imageShiftConstraintHelper(loadedImage->GetGreen(i,j) + inputValue);
				blue = imageShiftConstraintHelper(loadedImage->GetBlue(i,j) + inputValue);
				loadedImage->SetRGB(i,j,red,green,blue);
			}
		}
	}
	cout << "Finished shifting.\n";
	Refresh();
}

//Helper Fucntion for shift
int MyFrame::imageShiftConstraintHelper(int val){
	if(val >255){ return 255;}
	else if(val < 0){ return 0;}
	else{return val;}
}

void MyFrame::saltPepper(wxCommandEvent & event){
	BackupImage(loadedImage);
	cout << "Adding Salt-and-Pepper Noise.\n";
	float black = 0;
	float white = 255;
	int random;
	for(int i =1; i< imgWidth-1; i++){
		for(int j =1; j< imgHeight-1; j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				random = rand()%100 + 1;
				if(random < 20){
					loadedImage->SetRGB(i,j,black,black,black);
				}else if(random < 50){
					loadedImage->SetRGB(i,j,white,white,white);
				}
			}
		}
	}
	cout << "Finished Salt-and-Pepper Noise.\n";
    Refresh();
}

void MyFrame::maxFiltering(wxCommandEvent & event){
	filtering("max");
}

void MyFrame::minFiltering(wxCommandEvent & event){
	filtering("min");
}

void MyFrame::midFiltering(wxCommandEvent & event){
	filtering("mid");
}
void MyFrame::filtering(string filterType){
	BackupImage(loadedImage);
	cout << "Adding Min Filtering.\n";
	int red = 255;
	int green = 255;
	int blue = 255;
	int minRedValue = 255;
	int minGreenValue = 255;
	int minBlueValue = 255;
	int maxRedValue = 0;
	int maxGreenValue = 0;
	int maxBlueValue = 0;
	int midRedValue = 255;
	int midGreenValue = 255;
	int midBlueValue = 255;
	int temp;
	wxImage *me = new wxImage(imgWidth,imgHeight,true);
	for(int i =1; i< imgWidth-1; i++){
		for(int j =1; j< imgHeight-1; j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				for(int k = -1; k < 2; k++){
					for(int l = -1; l < 2; l++){
						red = loadedImage->GetRed(i+k, j+l)*1;
						green = loadedImage->GetGreen(i+k, j+l)*1;
						blue = loadedImage->GetBlue(i+k, j+l)*1;
						//For Min
						if( red < minRedValue){
							minRedValue = red;
						}
						if( green < minGreenValue){ 
							minGreenValue = green;
						}
						if( blue < minBlueValue){
							minBlueValue = blue;
						}
						//For Max
						if( red > maxRedValue){
							maxRedValue = red;
						}
						if( green > maxGreenValue){ 
							maxGreenValue = green;
						}
						if( blue > maxBlueValue){
							maxBlueValue = blue;
						}
						//For Mid
						midRedValue = (maxRedValue + minRedValue)/2;
						midGreenValue = (maxGreenValue + minGreenValue)/2;
						midBlueValue = (maxBlueValue + minBlueValue)/2;
					}
				}
				if(filterType.compare("min")==0){
					me->SetRGB(i,j,minRedValue,minGreenValue,minBlueValue);
				}else if(filterType.compare("max")==0){
					me->SetRGB(i,j,maxRedValue,maxGreenValue,maxBlueValue);
				}else if(filterType.compare("mid")==0){
					me->SetRGB(i,j,midRedValue,midGreenValue,midBlueValue);
				}
				//Reset Values
				minRedValue = 255; minGreenValue = 255; minBlueValue = 255;
				maxRedValue = 0; maxGreenValue = 0; maxBlueValue = 0;
				midRedValue = 255; midGreenValue = 255; midBlueValue = 255;
			}
		}
	}
	printf(" Finished Filtering.\n");
	loadedImage = me;
    	Refresh();
}
void MyFrame::negativeLinearTransform(wxCommandEvent & event){
	BackupImage(loadedImage);
	cout << "Negative Linear Transformation in progress \n";
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    for( int i=0;i<imgWidth;i++){
       for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
		 		loadedImage->SetRGB(i,j,255-loadedImage->GetRed(i,j), 255-loadedImage->GetGreen(i,j), 255-loadedImage->GetBlue(i,j));
			}
    	}
    }
    cout << "Finished Negative Linear Transformation \n";
    Refresh();
}
void MyFrame::logarithmicFunction(wxCommandEvent & event){
	BackupImage(loadedImage);
	cout << "Logarithmic Function in progress \n";
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());
    for( int i=0;i<imgWidth;i++){
       	for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				int c = 255 / log(256);
		 		loadedImage->SetRGB(i,j,c * log(1 + loadedImage->GetRed(i,j)), c * log(1 + loadedImage->GetGreen(i,j)), c * log(1 + loadedImage->GetBlue(i,j)));
			}
    	}
     }
    cout << " Finished Logarithmic Function \n";
    Refresh();
}
void MyFrame::powerLaw(wxCommandEvent & event){
	BackupImage(loadedImage);
	cout << "Power Law in progress \n";
	free(loadedImage);
	loadedImage = new wxImage(bitmap.ConvertToImage());
	wxString inputValue;
	double  inputValueDouble;
	inputValue = wxGetTextFromUser(_T("Enter a value between 0.01 and 25 for the power law:"),_T(""),_T(""),NULL, -1, -1, true);	
   	inputValue.ToDouble(&inputValueDouble);
	if(inputValueDouble>=0.01 && inputValueDouble<=25){
		for( int i=0;i<imgWidth;i++){
			for(int j=0;j<imgHeight;j++){
				if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
					int c = 255 / log(256);
					loadedImage->SetRGB(i,j,c * pow(loadedImage->GetRed(i,j), inputValueDouble), 
								c * pow(loadedImage->GetGreen(i,j), inputValueDouble),
								c * pow(loadedImage->GetBlue(i,j), inputValueDouble));
				}
			}
		}
		cout << "Finished Power Law Function\n";
		Refresh();
	}
	else{
		cout << "Value is not within specified range.\n";
	}
}

void MyFrame::randomLookUpTable(wxCommandEvent & event){
	BackupImage(loadedImage);
	cout << "Starting random look-up table \n";
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());

	float redArray[256];
	float greenArray[256]; 
	float blueArray[256];
	float red; 
	float green;
	float blue;

	// Set the random values table
	for (int i = 0; i < 256; i++){
		redArray[i] = (rand() % 256);
		greenArray[i] = (rand() % 256);
		blueArray[i] = (rand() % 256);
 	}

	for( int i=0;i<imgWidth;i++){
		for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				red = redArray[(int)loadedImage->GetRed(i, j)];
				green = greenArray[(int)loadedImage->GetGreen(i, j)];
				blue = blueArray[(int)loadedImage->GetBlue(i, j)];
				loadedImage->SetRGB(i, j, red, green, blue);
			}
 		}
	}
	cout << "Finished random function \n";
	Refresh();
}

void MyFrame::histogram(wxCommandEvent & event){
	BackupImage(loadedImage);
	printf("Finding Histogram \n");
	float redArray [256];
	float greenArray [256];
	float blueArray [256];
	int red, green, blue;
	for( int i=0;i<imgWidth;i++){
		for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				redArray[loadedImage->GetRed(i, j)] = redArray[loadedImage->GetRed(i, j)] +1;
				greenArray [loadedImage->GetGreen(i, j)] = greenArray [loadedImage->GetGreen(i, j)] +1;
				blueArray [loadedImage->GetBlue(i, j)] = blueArray [loadedImage->GetBlue(i, j)] +1;
			}
 		}
	}
	for (int i = 0; i < 256; i++){
		cout << i << " red = " <<  redArray[i] << " blue = " << blueArray[i] << " green = " << greenArray[i] <<"\n";
 	}
}

void MyFrame::histogramNormalisation(wxCommandEvent & event){
	BackupImage(loadedImage);
	printf("Finding Histogram Normalisation \n");
	float redArray [256];
	float greenArray [256];
	float blueArray [256];
	int red, green, blue, pixelCount;
	cout << imgWidth <<"\n";
	cout << imgHeight <<"\n";
	for( int i=0;i<imgWidth;i++){
		for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				pixelCount = pixelCount +1;
				redArray[loadedImage->GetRed(i, j)] = redArray[loadedImage->GetRed(i, j)] +1;
				greenArray [loadedImage->GetGreen(i, j)] = greenArray [loadedImage->GetGreen(i, j)] +1;
				blueArray [loadedImage->GetBlue(i, j)] = blueArray [loadedImage->GetBlue(i, j)] +1;
			}
 		}
	}
	for (int i = 0; i < 256; i++){
 		cout << i << " red = " <<  redArray[i] / pixelCount << " blue = " << blueArray[i] / pixelCount << " green = " << greenArray[i] / pixelCount<<"\n";
 	}
}

void MyFrame::histogramEqualisation(wxCommandEvent & event){
	BackupImage(loadedImage);
	printf("Finding Histogram Equalisation \n");
	float redArray [256];
	float greenArray [256];
	float blueArray [256];
	int red, green, blue, pixelCount;
	for( int i=0;i<imgWidth;i++){
		for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				pixelCount = pixelCount +1; 
				redArray[loadedImage->GetRed(i, j)] = redArray[loadedImage->GetRed(i, j)] +1;
				greenArray [loadedImage->GetGreen(i, j)] = greenArray [loadedImage->GetGreen(i, j)] +1;
				blueArray [loadedImage->GetBlue(i, j)] = blueArray [loadedImage->GetBlue(i, j)] +1;
			}
 		}
	}
	for (int i = 0; i < 256; i++){
		redArray[i] = redArray[i]/pixelCount;
		greenArray[i] = greenArray[i]/pixelCount;
		blueArray[i] = blueArray[i]/pixelCount;
 	}
	float cumulativeRedArray [256];
	float cumulativeGreenArray [256];
	float cumulativeBlueArray [256];
	float redCount, greenCount, blueCount;
	for (int i = 0; i < 256; i++){
		redCount += redArray[i];
  		greenCount += greenArray[i];
  		blueCount += blueArray[i];
		cumulativeRedArray[i] = redCount*256;
		cumulativeGreenArray[i] = greenCount*256;
		cumulativeBlueArray[i] = blueCount*256;
 	}

	for( int i=0;i<imgWidth;i++){
		for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				red = loadedImage->GetRed(i, j);
				green = loadedImage->GetGreen(i, j);
				blue = loadedImage->GetBlue(i, j);
				loadedImage->SetRGB(i, j, cumulativeRedArray[red], cumulativeGreenArray[green], cumulativeBlueArray[blue]);
			}
 		}
	}

	Refresh();
	cout << "Histogram Equalisation Complete";

}

void MyFrame::meanAndStandardDeviation(wxCommandEvent & event){
	BackupImage(loadedImage);
   	cout << "Mean and Standard Deviation \n";
	wxImage grey = loadedImage->ConvertToGreyscale();	

	float mean = 0;
	int count = 0;
	float value = 0;
	float x = 0;
	float xsquared = 0;
	for (int i = 0; i < imgWidth; i++){
		for (int j = 0; j < imgHeight; j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
	  			count += 1;
	  			x += loadedImage->GetRed(i, j);
	  			xsquared += pow(loadedImage->GetRed(i, j),2);
	  			mean += loadedImage->GetRed(i, j);
			}
		}
	}

	mean = mean / count;

	cout << "\nMean is -> " << mean << "\n";
	cout << "x -> " << x << "\n";
	cout << "xsquared -> " << xsquared <<"\n";
	int parta = xsquared / count;

	int partc = pow(mean,2);
	int partd = sqrt(parta - partc);
	cout << "Standard Deviation -> " << partd << "\n";
}

void MyFrame::simpleThresholding(wxCommandEvent & event){
	BackupImage(loadedImage);
	int userThresholdVal;
	int overwriteVal = 255;
	cout << "\nPlease enter your threshold value: " << std::endl;
	cin >> userThresholdVal;
	int r;
	int g;
	int b;
	int greyscale;

	for(int i=0;i<imgWidth;i++) {
		for(int j=0;j<imgHeight;j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				r = loadedImage->GetRed(i,j);
				g = loadedImage->GetGreen(i,j);
				b = loadedImage->GetBlue(i,j);
				greyscale = (r+g+b)/3;
				if(greyscale > userThresholdVal){
	        		r = overwriteVal;
	      		} else{
	        		greyscale = 0;
	      		}
				loadedImage->SetRGB(i,j,greyscale,greyscale,greyscale);
			}
		}
  	}
  	Refresh();
}

void MyFrame::automatedThresholding(wxCommandEvent & event){
	BackupImage(loadedImage);
	wxImage grey = loadedImage->ConvertToGreyscale(); 
  	float red;
  	float green;
  	float blue;
	for(int i = 0; i < imgWidth; i++) {
		for(int j = 0; j < imgHeight; j++) {
	      red = grey.GetRed(i, j);
	      green = grey.GetGreen(i, j);
	      blue = grey.GetBlue(i, j);
	      loadedImage->SetRGB(i, j, red, green, blue);
	    }
	}
	float mean = 0;
	int count = 0;
	float value = 0;
	for(int i = 0; i < imgWidth; i++) {
	    for(int j = 0; j < imgHeight; j++){
	      count += 1;
	      mean += loadedImage->GetRed(i, j);
	    }
	}
	mean = mean / count;
	for (int i = 0; i < imgWidth; i++) {
	    for (int j = 0; j < imgHeight; j++){
	      	value = loadedImage->GetRed(i, j);
	      	if (value < mean) {
	        	loadedImage->SetRGB(i, j, 0, 0, 0);
	      	}
	    }
	}
	Refresh();
}

//The trick of image filtering is that you have a 2D filter matrix, 
//and the 2D image. Then, for every pixel of the image, take the sum of products. 
//Each product is the color value of the current pixel or a neighbor of it,
// with the corresponding value of the filter matrix. The center of the filter matrix has to be multiplied with the current pixel, 
//the other elements of the filter matrix with corresponding neighbor pixels.
//This operation where you take the sum of products of elements from two 2D functions, where you let one of the two functions move over every 
//element of the other function, is called Convolution or Correlation. The difference between Convolution and Correlation is that for Convolution 
//you have to mirror the filter matrix, but usually it's symmetrical anyway so there's no difference.
//The filters with convolution are relatively simple. More complex filters, that can use more fancy functions, exist as well, and can do much more complex things (for example the Colored Pencil filter in Photoshop), but such filters aren't discussed here.


void MyFrame::maskImage(wxCommandEvent & event){
	BackupImage(loadedImage);
	int option; 
    	cout << "Pick A Command";
    	cout << "1 - Averageing";
    	cout << "2 - Weighted Averaging";
    	cout << "3 - 4 Neighbour Laplacian";              
    	cout << "4 - 8 Neighbour Laplacian";              
    	cout << "5 - 4 Neighbour Laplacian Enhancement";
    	cout << "6 - 8 Neighbour Laplacian Enhancement";
    	cout << "7 - Roberts 1\n";
    	cout << "8 - Roberts 2\n";
    	cout << "9 - Sobel X\n";
    	cout << "10 - Sobel Y\n";

	cin >> option;
	int Avarage [3][3] = {{1,1,1},{1,1,1},{1,1,1}};
	int weightedAv [3][3] = {{1,2,1},{2,4,2},{1,2,1}};
	int fourNeighbourL [3][3] = {{0,-1,0},{-1,4,-1},{0,-1,0}};
	int eightNeighbourL [3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
	int fourNeighbourLE [3][3] = {{0,-1,0},{-1,5,-1},{0,-1,0}};
	int eightNeighbourLE [3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};
	int roberts1 [3][3] = {{0,0,0},{0,0,-1},{0,1,0}};
	int roberts2 [3][3] = {{0,0,0},{0,-1,0},{0,0,1}};
	int sobelX [3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
	int sobelY [3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
	

	float mask [3][3];
	float factora = 1;
	float factorb = 1;
	float factorc = 1; 

	if(option == 1){
		std::copy(&Avarage[0][0], &Avarage[0][0]+3*3,&mask[0][0]);
		factora = 1;
		factorb = 9;
		factorc = factora/factorb;
	}
	else if(option == 2){
		std::copy(&weightedAv[0][0], &weightedAv[0][0]+3*3,&mask[0][0]);
		factora = 1;
		factorb = 16;
		factorc = factora/factorb;
	}

	else if(option == 3){
		std::copy(&fourNeighbourL[0][0], &fourNeighbourL[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 4){
		std::copy(&eightNeighbourL[0][0], &eightNeighbourL[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 5){
		std::copy(&fourNeighbourLE[0][0], &fourNeighbourLE[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 6){
		std::copy(&eightNeighbourLE[0][0], &eightNeighbourLE[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 7){
		std::copy(&roberts1[0][0], &roberts1[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 8){
		std::copy(&roberts2[0][0], &roberts2[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 9){
		std::copy(&sobelX[0][0], &sobelX[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else if(option == 10){
		std::copy(&sobelY[0][0], &sobelY[0][0]+3*3,&mask[0][0]);
		factorc = 1;
	}
	else{
		return;
	}

	for(int i=0; i< 3; i++){
		for(int j=0; j< 3; j++){
			cout<<mask[i][j]<<"  ";
		} cout<<endl;
	}
	int red;
	int green;
	int blue;

	for(int i =1; i< imgWidth-1; i++){
		for(int j =1; j< imgHeight-1; j++){
			if (!UsingROI || (UsingROI && (ROI.Contains(i, j) == 2))){
				red=loadedImage->GetRed(i-1,j-1)*mask[0][0]*factorc+
				loadedImage->GetRed(i-1,j)*mask[1][0]*factorc +
				loadedImage->GetRed(i-1,j+1)*mask[2][0]*factorc +

				loadedImage->GetRed(i,j-1)*mask[0][1]*factorc +
				loadedImage->GetRed(i,j)*mask[1][1]*factorc +
				loadedImage->GetRed(i,j+1)*mask[2][1]*factorc + 

				loadedImage->GetRed(i+1,j-1)*mask[0][2]*factorc +
				loadedImage->GetRed(i+1,j)*mask[1][2]*factorc +
				loadedImage->GetRed(i+1,j+1)*mask[2][2]*factorc;
			

				green=loadedImage->GetGreen(i-1,j-1)*mask[0][0]*factorc+
				loadedImage->GetGreen(i-1,j)*mask[1][0]*factorc+
				loadedImage->GetGreen(i-1,j+1)*mask[2][0]*factorc +

				loadedImage->GetGreen(i,j-1)*mask[0][1]*factorc +
				loadedImage->GetGreen(i,j)*mask[1][1]*factorc +
				loadedImage->GetGreen(i,j+1)*mask[2][1]*factorc + 

				loadedImage->GetGreen(i+1,j-1)*mask[0][2]*factorc +
				loadedImage->GetGreen(i+1,j)*mask[1][2]*factorc +
				loadedImage->GetGreen(i+1,j+1)*mask[2][2]*factorc;


				blue=loadedImage->GetBlue(i-1,j-1)*mask[0][0]*factorc +
				loadedImage->GetBlue(i-1,j)*mask[1][0]*factorc +
				loadedImage->GetBlue(i-1,j+1)*mask[2][0]*factorc +

				loadedImage->GetBlue(i,j-1)*mask[0][1]*factorc+
				loadedImage->GetBlue(i,j)*mask[1][1]*factorc +
				loadedImage->GetBlue(i,j+1)*mask[2][1]*factorc + 

				loadedImage->GetBlue(i+1,j-1)*mask[0][2]*factorc +
				loadedImage->GetBlue(i+1,j)*mask[1][2]*factorc +
				loadedImage->GetBlue(i+1,j+1)*mask[2][2]*factorc;


				if(red <0){red = 0;}
				if(green <0){green = 0;}
				if(blue <0){blue = 0;}

				if(red >255){red = 255;}
				if(green >255){green = 255;}
				if(blue >255){blue = 255;}
				loadedImage -> SetRGB(i,j,red,green,blue);
			}
		}
	}

	printf(" Finished comand.\n");
    	Refresh();
}

void MyFrame::setRoi(wxCommandEvent & event)
{
	int x, y, X, Y;
	cout << "Enter x coordinates for top left starting position ";
	cin  >> x;
	cout << "Enter y coordinates for top left starting position";
	cin  >> y;
	cout << "Enter the width value of the window ";
	cin  >> X;
	cout << "Enter the height value of the window  ";
	cin  >> Y;
	ROI = wxRegion(x, y, X, Y);
	UsingROI = true;
}
void MyFrame::removeRoi(wxCommandEvent & event)
{
	ROI = wxRegion();
	UsingROI = false;
}
void MyFrame::undo(wxCommandEvent & event)
{
	loadedImage = backupImage;
	Refresh();
}
void MyFrame::BackupImage(wxImage *src)
{
	backupImage = new wxImage(src->GetWidth(), src->GetHeight());
	float red, green, blue;
	for (int i = 0; i < src->GetWidth(); i++){
		for (int j = 0; j < src->GetHeight(); j++){
			red = src->GetRed(i, j);
			green = src->GetGreen(i, j);
			blue = src->GetBlue(i, j);
			backupImage->SetRGB(i, j, red, green, blue);
		}
	}	
}

void MyFrame::OnSaveImage(wxCommandEvent & event){

    printf("Saving image...");
    free(loadedImage);
    loadedImage = new wxImage(bitmap.ConvertToImage());

    loadedImage->SaveFile(wxT("Saved_Image.bmp"), wxBITMAP_TYPE_BMP);

    printf("Finished Saving.\n");
}

void MyFrame::OnExit (wxCommandEvent & event){
  Close(TRUE);
}

void MyFrame::OnPaint(wxPaintEvent & event){
  wxPaintDC dc(this);
  int cWidth, cHeight;  
  GetSize(&cWidth, &cHeight);
  if ((back_bitmap == NULL) || (oldWidth != cWidth) || (oldHeight != cHeight)) {
    if (back_bitmap != NULL)
      delete back_bitmap;
    back_bitmap = new wxBitmap(cWidth, cHeight);
    oldWidth = cWidth;
    oldHeight = cHeight;
  }
  wxMemoryDC *temp_dc = new wxMemoryDC(&dc);
  temp_dc->SelectObject(*back_bitmap);
  temp_dc->Clear();
  if(loadedImage)
    temp_dc->DrawBitmap(wxBitmap(*loadedImage), 0, 0, false);
       
  switch(stuffToDraw){
     case NOTHING:
        break;
     case ORIGINAL_IMG:
       bitmap.CleanUpHandlers;
       bitmap = wxBitmap(*loadedImage);
       break;
  }

imgWidth  = (bitmap.ConvertToImage()).GetWidth();
imgHeight = (bitmap.ConvertToImage()).GetHeight();

 temp_dc->SelectObject(bitmap); 

  dc.Blit(0, 0, cWidth, cHeight, temp_dc, 0, 0);
  delete temp_dc;
}

BEGIN_EVENT_TABLE (MyFrame, wxFrame)
  EVT_MENU ( LOAD_FILE_ID,  MyFrame::OnOpenFile)
  EVT_MENU ( EXIT_ID,  MyFrame::OnExit)
  EVT_MENU ( INVERT_IMAGE_ID,  MyFrame::OnInvertImage)
  EVT_MENU ( SCALE_IMAGE_ID,  MyFrame::OnScaleImage)
  EVT_MENU ( SAVE_IMAGE_ID,  MyFrame::OnSaveImage)
  EVT_MENU ( MY_read_raw,  MyFrame::readRawImage)
  EVT_MENU ( MY_shift,  MyFrame::shift)
  EVT_MENU ( MY_maskImage,  MyFrame::maskImage)
  EVT_MENU ( MY_saltPepper,  MyFrame::saltPepper)
  EVT_MENU ( MY_minFiltering,  MyFrame::minFiltering)
  EVT_MENU ( MY_maxFiltering,  MyFrame::maxFiltering)
  EVT_MENU ( MY_midFiltering,  MyFrame::midFiltering)
  EVT_MENU ( MY_negativeLinearTransform,  MyFrame::negativeLinearTransform)
  EVT_MENU ( MY_logarithmicFunction,  MyFrame::logarithmicFunction)
  EVT_MENU ( MY_powerLaw,  MyFrame::powerLaw)
  EVT_MENU ( MY_randomLookUpTable,  MyFrame::randomLookUpTable)
  EVT_MENU ( MY_histogram,  MyFrame::histogram)
  EVT_MENU ( MY_histogramNormalisation,  MyFrame::histogramNormalisation)
  EVT_MENU ( MY_meanAndStandardDeviation,  MyFrame::meanAndStandardDeviation)
  EVT_MENU ( MY_simpleThresholding,  MyFrame::simpleThresholding)
  EVT_MENU ( MY_automatedThresholding,  MyFrame::automatedThresholding)
  EVT_MENU ( MY_setRoi,  MyFrame::setRoi)
  EVT_MENU ( MY_removeRoi,  MyFrame::removeRoi)
  EVT_MENU ( MY_undo,  MyFrame::undo)
  EVT_PAINT (MyFrame::OnPaint)
  END_EVENT_TABLE()
