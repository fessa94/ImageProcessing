
class BasicApplication : public wxApp {
 public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {    
 protected:
  wxMenuBar  *menuBar;
  wxMenu     *fileMenu;
  wxBitmap *back_bitmap;
  wxToolBar *toolbar;
  int oldWidth, oldHeight;
	wxRegion ROI;
	bool UsingROI;
	wxImage  *backupImage;    
  wxBitmap bitmap;  
  wxImage *loadedImage; 
  int imgWidth, imgHeight; 
  int stuffToDraw; 

  void OnInvertImage(wxCommandEvent & event);
  void OnScaleImage(wxCommandEvent & event);
  void OnSaveImage(wxCommandEvent & event);
  void readRawImage(wxCommandEvent & event); 
  void shift(wxCommandEvent & event);
  int imageShiftConstraintHelper(int val);
  void maskImage(wxCommandEvent & event);
  void saltPepper(wxCommandEvent & event);
  void filtering(std::string filterType);
  void minFiltering(wxCommandEvent & event);
  void maxFiltering(wxCommandEvent & event);
  void midFiltering(wxCommandEvent & event);
  void negativeLinearTransform(wxCommandEvent & event);
  void logarithmicFunction(wxCommandEvent & event);
  void powerLaw(wxCommandEvent & event);
  void randomLookUpTable(wxCommandEvent & event);
  void histogram(wxCommandEvent & event);
  void histogramNormalisation(wxCommandEvent & event);
  void histogramEqualisation(wxCommandEvent & event);
  void meanAndStandardDeviation(wxCommandEvent & event);
  void simpleThresholding(wxCommandEvent & event);
  void automatedThresholding(wxCommandEvent & event);
  void setRoi(wxCommandEvent & event);
  void removeRoi(wxCommandEvent & event);
  void undo(wxCommandEvent & event);
  void BackupImage(wxImage *src);
 public:
    MyFrame(const wxString title, int xpos, int ypos, int width, int height);
    virtual ~MyFrame();
    
    void OnExit(wxCommandEvent & event);       
    void OnOpenFile(wxCommandEvent & event);       
    void OnPaint(wxPaintEvent & event);	
    
    DECLARE_EVENT_TABLE()
	
};

enum { NOTHING = 0,
       ORIGINAL_IMG,
};

enum { EXIT_ID = wxID_HIGHEST + 1,      
       LOAD_FILE_ID,
       INVERT_IMAGE_ID,
       SCALE_IMAGE_ID,
       SAVE_IMAGE_ID,
       MY_read_raw,
       MY_shift,
       MY_maskImage,
       MY_saltPepper,
       MY_minFiltering,
       MY_maxFiltering,
       MY_midFiltering,
       MY_negativeLinearTransform,
       MY_logarithmicFunction,
       MY_powerLaw,
       MY_randomLookUpTable,
       MY_histogram,
       MY_histogramNormalisation,
       MY_histogramEqualisation,
       MY_meanAndStandardDeviation,
       MY_simpleThresholding,
       MY_automatedThresholding,
       MY_setRoi,
       MY_removeRoi,
       MY_undo,
};
