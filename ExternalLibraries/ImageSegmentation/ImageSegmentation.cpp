
// Header files:
#include "stdafx.h"
#include "resource.h"

// Definitions:
#define MAX_MESSAGE                     (1024)
#define MAX_FILENAME                    (256)
#define MAX_LOADSTRING                  (100)
#define WINDOW_STYLE                    (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define DEFAULT_WINDOW_WIDTH            (320)
#define DEFAULT_WINDOW_HEIGHT           (240)

#define DEFAULT_PARAMETER_H             (8)
#define DEFAULT_PARAMETER_R             (8)
#define DEFAULT_PERFORM_RECONSTRUCTION  (true)
#define DEFAULT_GRADIENT_OPERATOR       (GO_Multiscale)
#define DEFAULT_ORDER_INVARIANT         (true)
#define DEFAULT_PROFILE_ITERATIONS      (10)
#define DEFAULT_ALWAYS_DOUBLE_SIZE      (true)
#define DEFAULT_CONNECTIVITY            (4)
#define DEFAULT_GRADIENT_TYPE           (GT_UShort)

#define DEFAULT_RIDGE_BLUE              (64)
#define DEFAULT_RIDGE_GREEN             (128)
#define DEFAULT_RIDGE_RED               (255)

#define DEFAULT_BOUNDARY_BLUE           (0)
#define DEFAULT_BOUNDARY_GREEN          (0)
#define DEFAULT_BOUNDARY_RED            (255)

enum TDisplayMode {
    DM_Source, DM_Gradient, DM_Gray
};

enum TProfileMode {
    PM_Multiscale, PM_Sobel, PM_Reconstruction, PM_Watershed, PM_Toboggan
};

enum TLatestAlgorithm {
    LA_Watershed,
    LA_Toboggan
};

enum TGradientOperator {
    GO_Sobel,
    GO_Multiscale,
    GO_None
};

enum TGradientType {
    GT_UShort,
    GT_Float,
    GT_Invalid
};

// ---------------------------------
class TGradientImage {
private:
    TGradientType GradientType;
    union {
        unsigned short *ushort_ptr;
        float *float_ptr;
    } Memory;

public:
    TGradientImage();
    ~TGradientImage();
    void New(const TGradientType NewType, const int Size);
    void Delete();
    void GetPointer(unsigned short* &Pointer);
    void GetPointer(float* &Pointer);
    void SetMemory(unsigned short* Poitner);
    void SetMemory(float* Pointer);
    bool IsNull() { return GradientType == GT_Invalid; }
};

TGradientImage::TGradientImage()
{
    GradientType = GT_Invalid;
}

TGradientImage::~TGradientImage()
{
    Delete();
}

void TGradientImage::Delete()
{
    switch(GradientType) {
    case GT_UShort:
        delete[] Memory.ushort_ptr; 
        break;
    case GT_Float:
        delete[] Memory.float_ptr;
        break;
    }
    GradientType = GT_Invalid;
}

void TGradientImage::New(TGradientType NewType, const int Size)
{
    Delete();
    switch(NewType) {
    case GT_UShort:
        Memory.ushort_ptr = new unsigned short[Size];
        if(!Memory.ushort_ptr) {
            throw "TGradientImage::New => cannot allocate memory";
        }
        break;
    case GT_Float:
        Memory.float_ptr = new float[Size];
        if(!Memory.float_ptr) {
            throw "TGradientImage::New => cannot allocate memory";
        }
        break;
    default:
        throw "TGradientImage::New => unknown gradient type";
    }
    GradientType = NewType;
}

void TGradientImage::GetPointer(unsigned short* &Pointer)
{
    if(GradientType == GT_UShort) {
        Pointer = Memory.ushort_ptr;
    }
    else {
        Pointer = NULL;
    }
}

void TGradientImage::GetPointer(float* &Pointer)
{
    if(GradientType == GT_Float) {
        Pointer = Memory.float_ptr;
    }
    else {
        Pointer = NULL;
    }
}

void TGradientImage::SetMemory(unsigned short* Pointer)
{
    Delete();
    if(Pointer) {
        Memory.ushort_ptr = Pointer;
        GradientType = GT_UShort;
    }
}

void TGradientImage::SetMemory(float* Pointer)
{
    Delete();
    if(Pointer) {
        Memory.float_ptr = Pointer;
        GradientType = GT_Float;
    }
}
// ---------------------------------

// Global variables:
HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

bool FileLoaded             = false;

int ParameterH              = DEFAULT_PARAMETER_H;
int ParameterR              = DEFAULT_PARAMETER_R;
bool PerformReconstruction  = DEFAULT_PERFORM_RECONSTRUCTION;
TGradientOperator GradientOperator 
                            = DEFAULT_GRADIENT_OPERATOR;
bool OrderInvariant         = DEFAULT_ORDER_INVARIANT;
int ProfileIterations       = DEFAULT_PROFILE_ITERATIONS;
int Connectivity            = DEFAULT_CONNECTIVITY;
TGradientType GradientType  = DEFAULT_GRADIENT_TYPE;

bool AlwaysDoubleSize       = DEFAULT_ALWAYS_DOUBLE_SIZE;
bool HideResults            = false;

HDC DisplayBuffer           = NULL;
HBITMAP DisplayBitmap       = NULL;
unsigned char* SourceImage  = NULL;
unsigned char* GrayImage    = NULL;
TGradientImage GradientImage;
int* LabelImage             = NULL;
unsigned char* DisplayBits;

int ImageWidth;
int ImageHeight;
int DisplayStepWidth;
TDisplayMode DisplayMode    = DM_Source;

RGBQUAD TempRidgeColor;
RGBQUAD TempBoundaryColor  ;

HWND   RidgeColorButton    = NULL;
HWND   BoundaryColorButton = NULL;
HBRUSH RidgeBrush          = NULL;
HBRUSH BoundaryBrush       = NULL;
RGBQUAD RidgeColor         = { DEFAULT_RIDGE_BLUE, DEFAULT_RIDGE_GREEN, DEFAULT_RIDGE_RED, 0 };
RGBQUAD BoundaryColor      = { DEFAULT_BOUNDARY_BLUE, DEFAULT_BOUNDARY_GREEN, DEFAULT_BOUNDARY_RED, 0 };

TLatestAlgorithm LatestAlgorithm = LA_Watershed;

// Function declarations:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    AboutProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    OptionProc(HWND, UINT, WPARAM, LPARAM);
VOID                ReleaseObjects();
VOID                ReleaseStaticObjects();
VOID                SetClientSize(HWND);
VOID                OnPaint(HWND);
VOID                OnChangeDisplayMode(HWND, TDisplayMode);
VOID                OnOpen(HWND);
VOID                OnSaveAs(HWND);
VOID                OnClose(HWND);
VOID                LoadImage(const char*);
VOID                SetDialogParameter(HWND);
VOID                GetDialogParameter(HWND);
VOID                SetDialogAttribute(HWND);
VOID                OnNewOption(HWND);
VOID                OnChangeColor(HWND, HBRUSH&, RGBQUAD&, HWND);
VOID                OnRemoveResult(HWND);
VOID                OnChangeAlwaysDoubleSize(HWND);
VOID                OnHideResults(HWND);
VOID                SaveScreen(const char*, const int, const int);
VOID                OnBlur(HWND);
VOID                OnResetGrayscale(HWND);

// Gradient-type dependent functions
VOID                UpdateDisplay();
VOID                GetGradient();
VOID                OnProfile(HWND, TProfileMode);
VOID                OnWatershed(HWND);
VOID                OnToboggan(HWND);

// -------------------------------------------------------------------------- //

inline int RGB_TO_Y(const int R, const int G, const int B)
{
    int Y_1024;
    Y_1024 = ((int)(1024 * 0.212671)) * R
           + ((int)(1024 * 0.715160)) * G
           + ((int)(1024 * 0.072169)) * B
           + 512;
    return Y_1024 >> 10;
}

inline COLORREF RGBQUAD_TO_COLORREF(RGBQUAD& Color)
{
    return Color.rgbRed + Color.rgbGreen * 0x100 + Color.rgbBlue * 0x10000;
}

inline RGBQUAD COLORREF_TO_RGBQUAD(COLORREF& Color)
{
    RGBQUAD x;
    x.rgbRed   = (BYTE) ((Color >>  0) & 0xff);
    x.rgbGreen = (BYTE) ((Color >>  8) & 0xff);
    x.rgbBlue  = (BYTE) ((Color >> 16) & 0xff);
    return x;
}

inline bool FinishExtension(char* FileName, char* Extension)
{
    int Length, eLength;
    Length = strlen(FileName);
    eLength = strlen(Extension);
    if(Length >= eLength) {
        if(_stricmp(FileName+Length-eLength, Extension) == 0) {
            return false;
        }
    }
    if(Length + eLength < MAX_FILENAME) {
        strcat(FileName, Extension);
    }
    return true;
}

// -------------------------------------------------------------------------- //

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_IMAGESEGMENTATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGESEGMENTATION);

	while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_IMAGESEGMENTATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = (LPCSTR)IDC_IMAGESEGMENTATION;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance;

    hWnd = CreateWindow(szWindowClass, szTitle, WINDOW_STYLE,
        CW_USEDEFAULT, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if(!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    RidgeBrush    = CreateSolidBrush(RGBQUAD_TO_COLORREF(RidgeColor));
    BoundaryBrush = CreateSolidBrush(RGBQUAD_TO_COLORREF(BoundaryColor));

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    try {
        switch(message) {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            switch (wmId) {
            // Menu<File>
            case IDM_OPEN:
                OnOpen(hWnd);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_CLOSE:
                OnClose(hWnd);
                break;
            case IDM_SAVE_AS:
                OnSaveAs(hWnd);
                break;
            // Menu<Edit>            
            case IDM_BLUR:
                OnBlur(hWnd);
                break;
            case IDM_RESET_GRAYSCALE:
                OnResetGrayscale(hWnd);
                break;
            case IDM_WATERSHED:
                OnWatershed(hWnd);
                break;
            case IDM_TOBOGGAN:
                OnToboggan(hWnd);
                break;
            case IDM_OPTION:
                if(DialogBox(hInst, (LPCTSTR)IDD_OPTION, hWnd, (DLGPROC)OptionProc) == IDOK) {
                    OnNewOption(hWnd);
                }
                break;
            // Menu<Display>
            case IDM_DISPLAY_SOURCE:
                OnChangeDisplayMode(hWnd, DM_Source);
                break;
            case IDM_DISPLAY_GRAY:
                OnChangeDisplayMode(hWnd, DM_Gray);
                break;
            case IDM_DISPLAY_GRADIENT:
                OnChangeDisplayMode(hWnd, DM_Gradient);
                break;
            case IDM_ALWAYS_DOUBLE_SIZE:
                OnChangeAlwaysDoubleSize(hWnd);
                break;
            case IDM_HIDE_RESULT:
                OnHideResults(hWnd);
                break;
            case IDM_REMOVE_RESULT:
                OnRemoveResult(hWnd);
                break;
            // Menu<Profile>
            case IDM_PROFILE_MULTISCALE:
                OnProfile(hWnd, PM_Multiscale);
                break;
            case IDM_PROFILE_SOBEL:
                OnProfile(hWnd, PM_Sobel);
                break;
            case IDM_PROFILE_RECONSTRUCTION:
                OnProfile(hWnd, PM_Reconstruction);
                break;
            case IDM_PROFILE_WATERSHED:
                OnProfile(hWnd, PM_Watershed);
                break;
            case IDM_PROFILE_TOBOGGAN:
                OnProfile(hWnd, PM_Toboggan);
                break;
            // Menu<Help>
            case IDM_ABOUT:
                DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)AboutProc);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
	        break;
        case WM_INITMENU:
            {
                HMENU hMenu = (HMENU) wParam;
                UINT uEnable = MF_BYCOMMAND | (FileLoaded ? MF_ENABLED : MF_GRAYED);
                
                EnableMenuItem(hMenu, IDM_SAVE_AS, uEnable);
                EnableMenuItem(hMenu, IDM_CLOSE, uEnable);
                EnableMenuItem(hMenu, IDM_BLUR, uEnable);
                EnableMenuItem(hMenu, IDM_RESET_GRAYSCALE, uEnable);
                EnableMenuItem(hMenu, IDM_WATERSHED, uEnable);
                EnableMenuItem(hMenu, IDM_TOBOGGAN, uEnable);
                EnableMenuItem(hMenu, IDM_PROFILE_MULTISCALE, uEnable);
                EnableMenuItem(hMenu, IDM_PROFILE_SOBEL, uEnable);
                EnableMenuItem(hMenu, IDM_PROFILE_RECONSTRUCTION, uEnable);
                EnableMenuItem(hMenu, IDM_PROFILE_WATERSHED, uEnable);
                EnableMenuItem(hMenu, IDM_PROFILE_TOBOGGAN, uEnable);

                CheckMenuItem(hMenu, IDM_DISPLAY_SOURCE, MF_BYCOMMAND |
                    (DisplayMode == DM_Source) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_DISPLAY_GRAY, MF_BYCOMMAND |
                    (DisplayMode == DM_Gray) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_DISPLAY_GRADIENT, MF_BYCOMMAND |
                    (DisplayMode == DM_Gradient) ? MF_CHECKED : MF_UNCHECKED);
                CheckMenuItem(hMenu, IDM_ALWAYS_DOUBLE_SIZE, MF_BYCOMMAND |
                    (AlwaysDoubleSize) ? MF_CHECKED : MF_UNCHECKED);

                CheckMenuItem(hMenu, IDM_HIDE_RESULT, MF_BYCOMMAND |
                    (HideResults) ? MF_CHECKED : MF_UNCHECKED);

                uEnable = MF_BYCOMMAND | ((FileLoaded && LabelImage) ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem(hMenu, IDM_REMOVE_RESULT, uEnable);
                EnableMenuItem(hMenu, IDM_HIDE_RESULT, uEnable);
;
            }
        case WM_PAINT:
            OnPaint(hWnd);
            break;
	    case WM_DESTROY:
            ReleaseObjects();
            ReleaseStaticObjects();
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    catch(const char* ErrorMessage) {
        MessageBox(hWnd, ErrorMessage, "Exception", MB_OK);
    }
    catch(...) {
        MessageBox(hWnd, "Unknown Error", "Exception", MB_OK);
    }
    return 0;
}

LRESULT CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK OptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        SetDialogParameter(hDlg);
        SetDialogAttribute(hDlg);
        RidgeColorButton = GetDlgItem(hDlg, IDC_RIDGE_COLOR);
        BoundaryColorButton = GetDlgItem(hDlg, IDC_BOUNDARY_COLOR);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDOK:
            GetDialogParameter(hDlg);
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        case IDC_APPLY:
            GetDialogParameter(hDlg);
            OnNewOption(GetParent(hDlg));
            SetFocus(hDlg);
            break;
        case IDC_CHECK_RECONSTRUCTION:
        case IDC_RADIO_NONE:
        case IDC_RADIO_SOBEL:
        case IDC_RADIO_MULTISCALE:
            SetDialogAttribute(hDlg);
            return TRUE;
        case IDC_RIDGE_COLOR:
            OnChangeColor(hDlg, RidgeBrush, TempRidgeColor, RidgeColorButton);
            return TRUE;
        case IDC_BOUNDARY_COLOR:
            OnChangeColor(hDlg, BoundaryBrush, TempBoundaryColor, BoundaryColorButton);
            return TRUE;
        }
        break;
    case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
            switch((UINT) wParam) {
            case IDC_RIDGE_COLOR:
            case IDC_BOUNDARY_COLOR:
                FillRect(lpdis->hDC, &(lpdis->rcItem), 
                    (IDC_RIDGE_COLOR == (UINT)wParam) ? RidgeBrush : BoundaryBrush);
                if(lpdis->itemState & ODS_FOCUS) {
                    lpdis->rcItem.left += 2;
                    lpdis->rcItem.right -= 2;
                    lpdis->rcItem.top += 2;
                    lpdis->rcItem.bottom -= 2;
                    DrawFocusRect(lpdis->hDC, &(lpdis->rcItem));
                }
                return TRUE;
            }
        }
        break;
    }

    return FALSE;
}

VOID ReleaseObjects()
{
    FileLoaded = false;

    if(DisplayBuffer) {
        DeleteDC(DisplayBuffer);
        DisplayBuffer = NULL;
    }
    if(DisplayBitmap) {
        DeleteObject(DisplayBitmap);
        DisplayBitmap = NULL;
    }
    if(SourceImage) {
        delete[] SourceImage;
        SourceImage = NULL;
    }
    if(GrayImage) {
        delete[] GrayImage;
        GrayImage = NULL;
    }
    GradientImage.Delete();
    if(LabelImage) {
        delete[] LabelImage;
        LabelImage = NULL;
    }
}

VOID ReleaseStaticObjects()
{
    if(RidgeBrush) {
        DeleteObject(RidgeBrush);
        RidgeBrush = NULL;
    }
    if(BoundaryBrush) {
        DeleteObject(BoundaryBrush);
        BoundaryBrush = NULL;
    }
}

VOID SetClientSize(HWND hWnd)
{
    RECT Rect;
    int AddW, AddH;
    int ClientWidth;
    int ClientHeight;

    if(!FileLoaded) {
        return;
    }
    if((LabelImage && !HideResults) || AlwaysDoubleSize) {
        ClientWidth = ImageWidth * 2 - 1;
        ClientHeight = ImageHeight * 2 - 1;
    }
    else {
        ClientWidth = ImageWidth;
        ClientHeight = ImageHeight;
    }

    Rect.left = 0;
    Rect.top = 0;
    Rect.right = ClientWidth;
    Rect.bottom = ClientHeight;

    AdjustWindowRect(&Rect, WINDOW_STYLE, TRUE);
    SetWindowPos(hWnd, NULL, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, 
        SWP_NOMOVE | SWP_NOZORDER);

    GetClientRect(hWnd, &Rect);
    AddW = ClientWidth - (Rect.right - Rect.left);
    AddH = ClientHeight - (Rect.bottom - Rect.top);

    if(AddW || AddH) {
        Rect.left = 0;
        Rect.top = 0;
        Rect.right = ClientWidth + AddW;
        Rect.bottom = ClientHeight + AddH;

        AdjustWindowRect(&Rect, WINDOW_STYLE, TRUE);
        SetWindowPos(hWnd, NULL, 0, 0, Rect.right - Rect.left, Rect.bottom - Rect.top, 
            SWP_NOMOVE | SWP_NOZORDER);
    }
}

VOID OnPaint(HWND hWnd)
{
    HDC hDC;
    PAINTSTRUCT ps;

    hDC = BeginPaint(hWnd, &ps);
    if(FileLoaded) {
        BitBlt(hDC, ps.rcPaint.left, ps.rcPaint.top, 
            ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
            DisplayBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
    }
    EndPaint(hWnd, &ps);
}

VOID OnChangeDisplayMode(HWND hWnd, TDisplayMode NewMode)
{
    if(DisplayMode != NewMode) {
        DisplayMode = NewMode;
        if(FileLoaded) {
            UpdateDisplay();
            InvalidateRect(hWnd, NULL, FALSE);
            UpdateWindow(hWnd);
        }
    }
}

VOID OnOpen(HWND hWnd)
{
    static char FileName[MAX_FILENAME] = "";
    static OPENFILENAME OpenFileName = {
        sizeof(OPENFILENAME), NULL, NULL,
        "Bitmap Files (*.bmp;*.dib)\0*.BMP;*.DIB\0All Files (*.*)\0*.*\0",
        NULL, 0, NULL, FileName, MAX_FILENAME,
        NULL, 0, NULL, NULL, 
        OFN_FILEMUSTEXIST, 
        0, 0, NULL, NULL, NULL, NULL
    };

    OpenFileName.hwndOwner = hWnd;
    if(GetOpenFileName(&OpenFileName) == FALSE) {
        return;
    }

    ReleaseObjects();
    LoadImage(FileName);

    DisplayMode = DM_Source;
    FileLoaded = true;
    UpdateDisplay();

    SetClientSize(hWnd);
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);
}

VOID OnSaveAs(HWND hWnd)
{
    static char FileName[MAX_FILENAME] = "";
    static OPENFILENAME OpenFileName = {
        sizeof(OPENFILENAME), NULL, NULL,
        "Bitmap Files (*.bmp)\0*.BMP\0",
        NULL, 0, NULL, FileName, MAX_FILENAME,
        NULL, 0, NULL, NULL, 
        OFN_PATHMUSTEXIST, 
        0, 0, NULL, NULL, NULL, NULL
    };
    int ClientWidth, ClientHeight;


    if(!FileLoaded) {
        return;
    }

    OpenFileName.hwndOwner = hWnd;
    if(GetSaveFileName(&OpenFileName) == FALSE) {
        return;
    }
    FinishExtension(FileName, ".bmp");
    if((LabelImage && !HideResults) || AlwaysDoubleSize) {
        ClientWidth = ImageWidth * 2 - 1;
        ClientHeight = ImageHeight * 2 - 1;
    }
    else {
        ClientWidth = ImageWidth;
        ClientHeight = ImageHeight;
    }
    SaveScreen(FileName, ClientWidth, ClientHeight);
}

VOID OnClose(HWND hWnd)
{
    ReleaseObjects();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

VOID LoadImage(const char* FileName)
{
    fstream file;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader, DIBInfo;
    RGBQUAD ColorTable[256];
    bool Reverse;
    int StepWidth, x, y, idx;
    unsigned char red, green, blue, color_index;
    unsigned char *src_scan, *gray_scan;

    file.open(FileName, ios::nocreate|ios::in|ios::binary);
    if(!file.is_open()) {
        throw "LoadImage => cannot open file";
    }

    file.read((char*)(&FileHeader), sizeof(FileHeader));
    file.read((char*)(&InfoHeader), sizeof(InfoHeader));

    if( FileHeader.bfType != 'B' + 'M' * 0x100 ||
        FileHeader.bfReserved1 != 0 ||
        FileHeader.bfReserved2 != 0 ) {
        throw "LoadImage => incorrect file header";
    }

    if( InfoHeader.biWidth <= 0 ||
        InfoHeader.biHeight == 0 ||
        InfoHeader.biPlanes != 1 ||
       (InfoHeader.biBitCount != 32 &&
        InfoHeader.biBitCount != 24 &&
        InfoHeader.biBitCount != 8) ||
        InfoHeader.biCompression != BI_RGB ) {
        throw "LoadImage => invalid file format";
    }

    // Load the color table
    if(InfoHeader.biBitCount == 8) {
        int Size;
        Size = InfoHeader.biClrUsed;
        if(Size == 0) {
            Size = (1<<InfoHeader.biBitCount);
        }
        else {
            ZeroMemory(ColorTable, sizeof(RGBQUAD) * 256);
        }
        file.seekg(sizeof(FileHeader) + InfoHeader.biSize, ios::beg);
        file.read((char*)ColorTable, sizeof(RGBQUAD) * Size);
        for(idx=0; idx<Size; idx++) {
            ColorTable[idx].rgbReserved = (BYTE)
                RGB_TO_Y( (int)ColorTable[idx].rgbRed,
                          (int)ColorTable[idx].rgbGreen,
                          (int)ColorTable[idx].rgbBlue );
        }
    }

    // Initiate variables
    ImageWidth = InfoHeader.biWidth;
    ImageHeight = InfoHeader.biHeight;

    if(ImageHeight < 0) {
        Reverse = false;
        ImageHeight = - ImageHeight;
    }
    else {
        Reverse = true;
    }

    StepWidth = ImageWidth * (InfoHeader.biBitCount / 8);
    StepWidth += (4 - (StepWidth % 4)) % 4;

    ZeroMemory(&DIBInfo, sizeof(DIBInfo));
    DIBInfo.biSize = sizeof(DIBInfo);
    DIBInfo.biWidth = 2 * ImageWidth - 1;
    DIBInfo.biHeight = - (2 * ImageHeight - 1); // Top-down scan
    DIBInfo.biPlanes = 1;
    DIBInfo.biBitCount = 24;
    DIBInfo.biCompression = BI_RGB;

    DisplayStepWidth = DIBInfo.biWidth * 3;
    DisplayStepWidth += (4 - (DisplayStepWidth % 4)) % 4;

    DisplayBuffer = CreateCompatibleDC(NULL);
    if(DisplayBuffer == NULL) {
        throw "LoadImage => cannot create DC";
    }
    DisplayBitmap = CreateDIBSection(DisplayBuffer, (CONST BITMAPINFO*) (&DIBInfo), DIB_RGB_COLORS, 
        (VOID**) (&DisplayBits), NULL, 0);
    if(DisplayBitmap == NULL) {
        throw "LoadImage => cannot create DIB section";
    }
    SelectObject(DisplayBuffer, DisplayBitmap);

    SourceImage   = new unsigned char[ImageWidth * ImageHeight * 3];
    GrayImage     = new unsigned char[ImageWidth * ImageHeight];
    if(SourceImage == NULL || GrayImage == NULL) {
        throw "LoadImage => cannot allocate memory";
    }
    
    // Read image data
    src_scan = SourceImage;
    gray_scan = GrayImage;
    for(y=0; y<ImageHeight; y++) {
        if(Reverse) {
            file.seekg(FileHeader.bfOffBits + StepWidth * (ImageHeight - 1 - y), ios::beg);
        }
        else {
            file.seekg(FileHeader.bfOffBits + StepWidth * y, ios::beg);
        }
        switch(InfoHeader.biBitCount) {
        case 32:
            for(x=0; x<ImageWidth; x++) {
                src_scan[3*x+0] = blue  = (unsigned char) file.get();
                src_scan[3*x+1] = green = (unsigned char) file.get();
                src_scan[3*x+2] = red   = (unsigned char) file.get();
                file.get();
                gray_scan[x] = (unsigned char) RGB_TO_Y((int)red, (int)green, (int)blue);
            }
            break;
        case 24:
            for(x=0; x<ImageWidth; x++) {
                src_scan[3*x+0] = blue  = (unsigned char) file.get();
                src_scan[3*x+1] = green = (unsigned char) file.get();
                src_scan[3*x+2] = red   = (unsigned char) file.get();
                gray_scan[x] = (unsigned char) RGB_TO_Y((int)red, (int)green, (int)blue);
            }
            break;
        case 8:
            for(x=0; x<ImageWidth; x++) {
                color_index = file.get();
                src_scan[3*x+0] = ColorTable[color_index].rgbBlue;
                src_scan[3*x+1] = ColorTable[color_index].rgbGreen;
                src_scan[3*x+2] = ColorTable[color_index].rgbRed;
                gray_scan[x]    = ColorTable[color_index].rgbReserved;
            }
            break;
        }
        src_scan += ImageWidth * 3;
        gray_scan += ImageWidth;
    }

    file.close();
}

VOID SetDialogParameter(HWND hDlg)
{
    CheckDlgButton(hDlg, IDC_CHECK_ORDER_INVARIANT,
        OrderInvariant ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK_RECONSTRUCTION, 
        PerformReconstruction ? BST_CHECKED : BST_UNCHECKED);

    CheckDlgButton(hDlg, IDC_RADIO_SOBEL,
        (GradientOperator == GO_Sobel) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_RADIO_MULTISCALE,
        (GradientOperator == GO_Multiscale) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_RADIO_NONE,
        (GradientOperator == GO_None) ? BST_CHECKED : BST_UNCHECKED);

    CheckDlgButton(hDlg, IDC_RADIO_4CONN,
        (Connectivity == 4) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_RADIO_8CONN,
        (Connectivity == 8) ? BST_CHECKED : BST_UNCHECKED);

    CheckDlgButton(hDlg, IDC_RADIO_USHORT,
        (GradientType == GT_UShort) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_RADIO_FLOAT,
        (GradientType == GT_Float) ? BST_CHECKED : BST_UNCHECKED);

    SetDlgItemInt(hDlg, IDC_EDIT_PARAMETER_H, (UINT) ParameterH, FALSE);
    SetDlgItemInt(hDlg, IDC_EDIT_PARAMETER_R, (UINT) ParameterR, FALSE);
    SetDlgItemInt(hDlg, IDC_EDIT_PROFILE_ITERATIONS, (UINT) ProfileIterations, FALSE);

    TempRidgeColor = RidgeColor;
    TempBoundaryColor = BoundaryColor;
    if(RidgeBrush != NULL) {
        DeleteObject(RidgeBrush);
    }
    if(BoundaryBrush != NULL) {
        DeleteObject(BoundaryBrush);
    }
    RidgeBrush = CreateSolidBrush(RGBQUAD_TO_COLORREF(TempRidgeColor));
    BoundaryBrush = CreateSolidBrush(RGBQUAD_TO_COLORREF(TempBoundaryColor));
    
}

VOID GetDialogParameter(HWND hDlg)
{
    BOOL Translated;

    OrderInvariant = (IsDlgButtonChecked(hDlg, IDC_CHECK_ORDER_INVARIANT) == BST_CHECKED);
    PerformReconstruction = (IsDlgButtonChecked(hDlg, IDC_CHECK_RECONSTRUCTION) == BST_CHECKED);

    if(IsDlgButtonChecked(hDlg, IDC_RADIO_NONE) == BST_CHECKED) {
        GradientOperator = GO_None;
    }
    else if(IsDlgButtonChecked(hDlg, IDC_RADIO_SOBEL) == BST_CHECKED) {
        GradientOperator = GO_Sobel;
    }
    else if(IsDlgButtonChecked(hDlg, IDC_RADIO_MULTISCALE) == BST_CHECKED) {
        GradientOperator = GO_Multiscale;
    }
    else {
        GradientOperator = DEFAULT_GRADIENT_OPERATOR;
    }

    if(IsDlgButtonChecked(hDlg, IDC_RADIO_4CONN) == BST_CHECKED) {
        Connectivity = 4;
    }
    else if(IsDlgButtonChecked(hDlg, IDC_RADIO_8CONN) == BST_CHECKED) {
        Connectivity = 8;
    }
    else {
        Connectivity = DEFAULT_CONNECTIVITY;
    }

    if(IsDlgButtonChecked(hDlg, IDC_RADIO_USHORT) == BST_CHECKED) {
        GradientType = GT_UShort;
    }
    else if(IsDlgButtonChecked(hDlg, IDC_RADIO_FLOAT) == BST_CHECKED) {
        GradientType = GT_Float;
    }
    else {
        GradientType = DEFAULT_GRADIENT_TYPE;
    }

    ParameterH = (int) GetDlgItemInt(hDlg, IDC_EDIT_PARAMETER_H, &Translated, FALSE);
    ParameterR = (int) GetDlgItemInt(hDlg, IDC_EDIT_PARAMETER_R, &Translated, FALSE);
    ProfileIterations = (int) GetDlgItemInt(hDlg, IDC_EDIT_PROFILE_ITERATIONS, &Translated, FALSE);

    RidgeColor = TempRidgeColor;
    BoundaryColor = TempBoundaryColor;
}

VOID SetDialogAttribute(HWND hDlg)
{
    BOOL Enable;
    
    Enable = !IsDlgButtonChecked(hDlg, IDC_RADIO_NONE);
    EnableWindow(GetDlgItem(hDlg, IDC_CHECK_RECONSTRUCTION), Enable);

    Enable &= IsDlgButtonChecked(hDlg, IDC_CHECK_RECONSTRUCTION);
    EnableWindow(GetDlgItem(hDlg, IDC_EDIT_PARAMETER_H), Enable);
    EnableWindow(GetDlgItem(hDlg, IDC_EDIT_PARAMETER_R), Enable); 
    
    Enable = FileLoaded ? TRUE : FALSE;
    EnableWindow(GetDlgItem(hDlg, IDC_APPLY), Enable);
}

VOID OnNewOption(HWND hWnd)
{
    if(FileLoaded) {
        bool Update = false;
        if(!GradientImage.IsNull()) {
            GradientImage.Delete();
            if(DisplayMode == DM_Gradient) {
                Update = true;
            }
        }
        if(LabelImage) {

            switch(LatestAlgorithm) {
            case LA_Watershed:
                OnWatershed(hWnd);
                return;
            case LA_Toboggan:
                OnToboggan(hWnd);
                return;
            }

            delete[] LabelImage;
            LabelImage = NULL;
            SetClientSize(hWnd);
            Update = true;
        }
        if(Update) {
            UpdateDisplay();
            InvalidateRect(hWnd, NULL, FALSE);
            UpdateWindow(hWnd);
        }
    }
}

VOID OnChangeColor(HWND hWnd, HBRUSH& hBrush, RGBQUAD& Color, HWND hButton)
{
    static COLORREF CustColorTable[16] = { 
        ( DEFAULT_RIDGE_RED    + DEFAULT_RIDGE_GREEN    * 0x100 + DEFAULT_RIDGE_BLUE    * 0x10000),
        ( DEFAULT_BOUNDARY_RED + DEFAULT_BOUNDARY_GREEN * 0x100 + DEFAULT_BOUNDARY_BLUE * 0x10000)
    };
    static CHOOSECOLOR ColorInfo;

    ColorInfo.lStructSize  = sizeof(ColorInfo);
    ColorInfo.hwndOwner    = hWnd;
    ColorInfo.hInstance    = NULL;
    ColorInfo.rgbResult    = RGBQUAD_TO_COLORREF(Color);
    ColorInfo.lpCustColors = CustColorTable;
    ColorInfo.Flags        = CC_ANYCOLOR | CC_RGBINIT | CC_SOLIDCOLOR;

    if(ChooseColor(&ColorInfo) == FALSE) {
        return;
    }

    Color = COLORREF_TO_RGBQUAD(ColorInfo.rgbResult);
    if(hBrush != NULL) {
        DeleteObject(hBrush);
    }
    hBrush = CreateSolidBrush(RGBQUAD_TO_COLORREF(Color));

    InvalidateRect(hButton, NULL, TRUE);
    UpdateWindow(hButton);
}

VOID OnRemoveResult(HWND hWnd)
{
    if(!FileLoaded || !LabelImage) {
        return;
    }
    delete[] LabelImage;
    LabelImage = NULL;
    UpdateDisplay();
    SetClientSize(hWnd);
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);
}

VOID OnChangeAlwaysDoubleSize(HWND hWnd)
{
    AlwaysDoubleSize = ! AlwaysDoubleSize;
    if(FileLoaded && LabelImage) {
        UpdateDisplay();
        SetClientSize(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
    }        
}

VOID OnHideResults(HWND hWnd)
{
    HideResults = ! HideResults;
    if(FileLoaded && LabelImage) {
        UpdateDisplay();
        SetClientSize(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
    }        
}

VOID SaveScreen(const char* FileName, const int Width, const int Height)
{
    fstream file;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;
    int StepWidth;
    int x, y;

    StepWidth = Width * 3;
    StepWidth += (4 - (StepWidth % 4)) % 4;
    
    ZeroMemory(&FileHeader, sizeof(FileHeader));
    ZeroMemory(&InfoHeader, sizeof(InfoHeader));

    FileHeader.bfType = 'B' + 'M' * 0x100;
    FileHeader.bfSize = sizeof(FileHeader) + sizeof(InfoHeader) + StepWidth * Height;
    FileHeader.bfOffBits = sizeof(FileHeader) + sizeof(InfoHeader);

    InfoHeader.biSize = sizeof(InfoHeader);
    InfoHeader.biWidth = Width;
    InfoHeader.biHeight = Height;
    InfoHeader.biPlanes = 1;
    InfoHeader.biBitCount = 24;
    InfoHeader.biCompression = BI_RGB;

    file.open(FileName, ios::out | ios::trunc | ios::binary);
    if(!file.is_open()) {
        throw "SaveScreen => cannot open file";
    }

    file.write((char*)(&FileHeader), sizeof(FileHeader));
    file.write((char*)(&InfoHeader), sizeof(InfoHeader));

    for(y=Height-1; y>=0; y--) {
        file.write((char*)(DisplayBits + DisplayStepWidth * y), Width * 3);
        for(x=Width*3; x<StepWidth; x++) {
            file.put('\0');
        }
    }
    file.close();
}

VOID OnBlur(HWND hWnd)
{
    if(!FileLoaded) {
        return;
    }
    
    Blur<unsigned char>(GrayImage, ImageWidth, ImageHeight);
    OnNewOption(hWnd);

    UpdateDisplay();
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);
}

VOID OnResetGrayscale(HWND hWnd)
{
    int i, Size;
    if(!FileLoaded) {
        return;
    }
    Size = ImageWidth * ImageHeight;
    for(i=0; i<Size; i++) {
        GrayImage[i] =
            RGB_TO_Y( SourceImage[3*i+2],
                      SourceImage[3*i+1],
                      SourceImage[3*i+0] );
    }
    OnNewOption(hWnd);
    UpdateDisplay();
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);
}

// -------------------------------------------------------------------------- //

template<class T>
struct GradientTypeDependentFunctions {
    static VOID GetGradient();
    static VOID UpdateDisplay();
    static VOID OnProfile(HWND, TProfileMode);
    static VOID OnWatershed(HWND);
    static VOID OnToboggan(HWND);
};

VOID GetGradient()
{
    switch(GradientType) {
    case GT_UShort:
        GradientTypeDependentFunctions<unsigned short>::
            GetGradient();
            break;
    case GT_Float:
        GradientTypeDependentFunctions<float>::
            GetGradient();
            break;
    default:
        throw "GetGradient => unknown gradient type";
    }
}

VOID UpdateDisplay()
{
    switch(GradientType) {
    case GT_UShort:
        GradientTypeDependentFunctions<unsigned short>::
            UpdateDisplay();
            break;
    case GT_Float:
        GradientTypeDependentFunctions<float>::
            UpdateDisplay();
            break;
    default:
        throw "UpdateDisplay => unknown gradient type";
    }
}

VOID OnProfile(HWND hWnd, TProfileMode ProfileMode)
{
    switch(GradientType) {
    case GT_UShort:
        GradientTypeDependentFunctions<unsigned short>::
            OnProfile(hWnd, ProfileMode);
            break;
    case GT_Float:
        GradientTypeDependentFunctions<float>::
            OnProfile(hWnd, ProfileMode);
            break;
    default:
        throw "OnProfile => unknown gradient type";
    }
}

VOID OnWatershed(HWND hWnd)
{
    switch(GradientType) {
    case GT_UShort:
        GradientTypeDependentFunctions<unsigned short>::
            OnWatershed(hWnd);
            break;
    case GT_Float:
        GradientTypeDependentFunctions<float>::
            OnWatershed(hWnd);
            break;
    default:
        throw "OnWatershed => unknown gradient type";
    }
}

VOID OnToboggan(HWND hWnd)
{
    switch(GradientType) {
    case GT_UShort:
        GradientTypeDependentFunctions<unsigned short>::
            OnToboggan(hWnd);
            break;
    case GT_Float:
        GradientTypeDependentFunctions<float>::
            OnToboggan(hWnd);
            break;
    default:
        throw "OnToboggan => unknown gradient type";
    }
}

// -------------------------------------------------------------------------- //

template<class T> VOID
GradientTypeDependentFunctions<T>::
GetGradient()
{
    int idx;
    T *Gradient;

    GradientImage.Delete();
    switch(GradientOperator) {
    case GO_None:
        GradientImage.New(GradientType, ImageWidth * ImageHeight);
        GradientImage.GetPointer(Gradient);
        for(idx=0; idx<ImageWidth*ImageHeight; idx++) {
            Gradient[idx] = GrayImage[idx];
        }
        return; // Never perform the gradient reconstruction...
    case GO_Multiscale:
        Gradient = MultiscaleGradient<T, unsigned char>(GrayImage, ImageWidth, ImageHeight);
        if(Gradient == NULL) {
            throw "GetGradient => MultiscaleGradient() returns NULL";
        }
        GradientImage.SetMemory(Gradient);
        break;
    case GO_Sobel:
        Gradient = SobelGradient<T, unsigned char>(GrayImage, ImageWidth, ImageHeight);
        if(Gradient == NULL) {
            throw "GetGradient => SobelGradient() returns NULL";
        }
        GradientImage.SetMemory(Gradient);
        break;
    default:
        throw "GetGradient => unknown gradient operator";
    }

    if(PerformReconstruction) {
        ClosingByReconstruction(Gradient, ImageWidth, ImageHeight, (const T)ParameterH, ParameterR);
    }
}

template<class T> VOID
GradientTypeDependentFunctions<T>::
UpdateDisplay()
{
    int StepWidth, Stride, x, y, idx, maxx;
    TDisplayMode Mode;
    unsigned char *scan;
    double Scale;
    T max_g, min_g, *Gradient;

    if(!FileLoaded) {
        return;
    }

    if((LabelImage && !HideResults) || AlwaysDoubleSize) {
        StepWidth = DisplayStepWidth * 2;
        Stride = 2;
        maxx = ImageWidth * 2 - 1;
    }
    else {
        StepWidth = DisplayStepWidth;
        Stride = 1;
        maxx = ImageWidth;
    }
    
    Mode = DisplayMode;
    if(Mode == DM_Gradient) {
        if(GradientImage.IsNull()) {
            GetGradient();
        }
        GradientImage.GetPointer(Gradient);
        max_g = min_g = Gradient[0];
        for(idx=1; idx<ImageWidth * ImageHeight; idx++) {
            T g = Gradient[idx];
            if(g > max_g) {
                max_g = g;
            }
            else if(g < min_g) {
                min_g = g;
            }
        }
        if(max_g == min_g) {
            Scale = 1;
        }
        else {
            Scale = 255.0 / (max_g - min_g);
        }
    }

    idx = 0;
    scan = DisplayBits;
    for(y=0; y<ImageHeight; y++) {
        switch(Mode) {
        case DM_Source:
            for(x=0; x<maxx; x+=Stride, idx++) {
                scan[3*x+0] = SourceImage[idx*3+0];
                scan[3*x+1] = SourceImage[idx*3+1];
                scan[3*x+2] = SourceImage[idx*3+2];
            }
            break;
        case DM_Gradient:
            for(x=0; x<maxx; x+=Stride, idx++) {
                T g = (T) ((Gradient[idx] - min_g) * Scale);
                scan[3*x+0] = scan[3*x+1] = scan[3*x+2] = (unsigned char) g;
            }
            break;
        case DM_Gray:
            for(x=0; x<maxx; x+=Stride, idx++) {
                scan[3*x+0] = scan[3*x+1] = scan[3*x+2] = GrayImage[idx];
            }
            break;
        }
        scan += StepWidth;
    }

    if(!LabelImage && !AlwaysDoubleSize) {
        return;
    }

    // Painting the ridges
    if(LabelImage && !HideResults) {
        idx = 0;
        scan = DisplayBits;
        for(y=0; y<ImageHeight; y++) {
            for(x=0; x<maxx; x+=Stride, idx++) {
                if(LabelImage[idx] < 0) {
                    scan[3*x+0] = RidgeColor.rgbBlue;
                    scan[3*x+1] = RidgeColor.rgbGreen;
                    scan[3*x+2] = RidgeColor.rgbRed;
                }
            }
            scan += StepWidth;
        }
    }

    // Horizontal interpolation
    maxx = ImageWidth - 1;
    scan = DisplayBits;
    for(y=0; y<ImageHeight; y++) {
        for(x=0; x<maxx; x++) {
            scan[6*x+3] = (scan[6*x+0] + scan[6*x+6])/2;
            scan[6*x+4] = (scan[6*x+1] + scan[6*x+7])/2;
            scan[6*x+5] = (scan[6*x+2] + scan[6*x+8])/2;
        }
        scan += DisplayStepWidth * 2;
    }

    // Vertical interpolation
    maxx = (ImageWidth * 2 - 1) * 3;
    scan = DisplayBits + DisplayStepWidth;
    for(y=0; y<ImageHeight-1; y++) {
        for(x=0; x<maxx; x++) {
            scan[x] = (scan[x-DisplayStepWidth] + scan[x+DisplayStepWidth])/2;
        }
        scan += DisplayStepWidth * 2;
    }

    if(!LabelImage || HideResults) {
        return;
    }

    // Drawing the boundaries
    idx = 0;
    scan = DisplayBits;
    for(y=0; y<ImageHeight-1; y++) {
        for(x=0; x<ImageWidth-1; x++, idx++) {
            // a b
            // c d
            int a, b, c, d, offset;
            a = LabelImage[idx];
            b = LabelImage[idx+1];
            c = LabelImage[idx+ImageWidth];
            d = LabelImage[idx+ImageWidth+1];
            offset = 6 * x + 3;
            if(a != b) {
                scan[offset+0] = BoundaryColor.rgbBlue;
                scan[offset+1] = BoundaryColor.rgbGreen;
                scan[offset+2] = BoundaryColor.rgbRed;
            }
            offset += DisplayStepWidth;
            if(a != d || b != c) {
                scan[offset+0] = BoundaryColor.rgbBlue;
                scan[offset+1] = BoundaryColor.rgbGreen;
                scan[offset+2] = BoundaryColor.rgbRed;
            }
            offset -= 3;
            if(a != c) {
                scan[offset+0] = BoundaryColor.rgbBlue;
                scan[offset+1] = BoundaryColor.rgbGreen;
                scan[offset+2] = BoundaryColor.rgbRed;
            }
        }
        idx++;
        scan += DisplayStepWidth * 2;
    }
}

template<class T> VOID
GradientTypeDependentFunctions<T>::
OnProfile(HWND hWnd, TProfileMode ProfileMode)
{
    __int64 beginning, end, dummy, counts_per_second;
    long double average_time;
    T *WorkGradient, *Gradient;
    int *WorkLabel;
    int i;
    char* DataType;

    OSVERSIONINFO VersionInfo;
    bool IsWinNT;
    char Message[MAX_MESSAGE];
    bool SavePerformReconstruction;

    if(!FileLoaded || ProfileIterations <= 0) {
        return;
    }

    // Check the operating system platform
    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
    GetVersionEx(&VersionInfo);
    switch(VersionInfo.dwPlatformId) {
    case VER_PLATFORM_WIN32_WINDOWS:
        IsWinNT = false;
        break;
    case VER_PLATFORM_WIN32_NT:
        IsWinNT = true;
        break;
    default:
        throw "OnPorfile => unsupported operating system platform";
    }

    SavePerformReconstruction = PerformReconstruction;
    // Initialization
    switch(ProfileMode) {
    case PM_Multiscale:
    case PM_Sobel:
        WorkGradient = NULL;
        WorkLabel = NULL;
        break;
    case PM_Reconstruction:
        PerformReconstruction = false;
    case PM_Watershed:
    case PM_Toboggan:
        WorkGradient = new T[ImageWidth* ImageHeight];
        if(!WorkGradient) {
            throw "OnProfile => cannot allocate memory";
        }
        WorkLabel = NULL;
        if(GradientImage.IsNull()) {
            GetGradient();
        }
        GradientImage.GetPointer(Gradient);
        PerformReconstruction = SavePerformReconstruction;
        break;
    }

    // Fetch the time of the beginning
    if(IsWinNT) {
        counts_per_second = 10000000; // 1 / 100-nanosecond 
        GetProcessTimes(GetCurrentProcess(), (LPFILETIME)&dummy, (LPFILETIME)&dummy,
            (LPFILETIME)&dummy, (LPFILETIME)&beginning);
    }
    else {
        QueryPerformanceFrequency((LARGE_INTEGER*)&counts_per_second);
        QueryPerformanceCounter((LARGE_INTEGER*)&beginning);
    }

    // Profile
    switch(ProfileMode) {
    case PM_Multiscale:
        for(i=0; i<ProfileIterations; i++) {
            if(WorkGradient) {
                delete[] WorkGradient;
            }
            WorkGradient = MultiscaleGradient<T, unsigned char>(GrayImage, ImageWidth, ImageHeight);
        }
        break;
    case PM_Sobel:
        for(i=0; i<ProfileIterations; i++) {
            if(WorkGradient) {
                delete[] WorkGradient;
            }
            WorkGradient = SobelGradient<T, unsigned char>(GrayImage, ImageWidth, ImageHeight);
        }
        break;
    case PM_Reconstruction:
        for(i=0; i<ProfileIterations; i++) {
            CopyMemory(WorkGradient, Gradient, sizeof(T) * ImageWidth * ImageHeight);
            ClosingByReconstruction(WorkGradient, ImageWidth, ImageHeight, (const T)ParameterH, ParameterR);
        }
        break;
    case PM_Watershed:
        CopyMemory(WorkGradient, Gradient, sizeof(T) * ImageWidth * ImageHeight);
        for(i=0; i<ProfileIterations; i++) {
            if(WorkLabel) {
                delete[] WorkLabel;
            }
            WorkLabel = Watershed(WorkGradient, ImageWidth, ImageHeight, OrderInvariant, Connectivity);
        }
        break;
    case PM_Toboggan:
        CopyMemory(WorkGradient, Gradient, sizeof(T) * ImageWidth * ImageHeight);
        for(i=0; i<ProfileIterations; i++) {
            if(WorkLabel) {
                delete[] WorkLabel;
            }
            WorkLabel = Toboggan(WorkGradient, ImageWidth, ImageHeight, OrderInvariant, Connectivity);
        }
        break;
    }

    // Fetch the time of the end
    if(IsWinNT) {
        GetProcessTimes(GetCurrentProcess(), (LPFILETIME)&dummy, (LPFILETIME)&dummy,
            (LPFILETIME)&dummy, (LPFILETIME)&end);
    }
    else {
        QueryPerformanceCounter((LARGE_INTEGER*)&end);
    }

    // Free memory
    if(WorkGradient) {
        delete[] WorkGradient;
    }
    if(WorkLabel) {
        delete[] WorkLabel;
    }

    // Show message
    switch(GradientType) {
    case GT_UShort:
        DataType = "unsigned short";
        break;
    case GT_Float:
        DataType = "float";
        break;
    default:
        DataType = "???";
    }
    average_time = ((long double)(end - beginning))
                 / ((long double)(counts_per_second))
                 / ((long double)(ProfileIterations))
                 ;
    sprintf(Message, "Average execution time = %.3lf ms, data type = %s",
        floor(.5+(double)(1000 * average_time)), DataType);
    switch(ProfileMode) {
    case PM_Multiscale:
        MessageBox(hWnd, Message, "Profile - Multiscale Gradient", MB_OK);
        break;
    case PM_Sobel:
        MessageBox(hWnd, Message, "Profile - Sobel Gradient", MB_OK);
        break;
    case PM_Reconstruction:
        MessageBox(hWnd, Message, "Profile - Reconstruction", MB_OK);
        break;
    case PM_Watershed:
        MessageBox(hWnd, Message, "Profile - Watershed Algorithm", MB_OK);
        break;
    case PM_Toboggan:
        MessageBox(hWnd, Message, "Profile - Toboggan Algorithm", MB_OK);
        break;
    }

    // Recovering
    if(ProfileMode == PM_Reconstruction && PerformReconstruction == true) {
        GetGradient();        
    }
}

template<class T> VOID
GradientTypeDependentFunctions<T>::
OnWatershed(HWND hWnd)
{
    T *Gradient;

    if(!FileLoaded) {
        return;
    }
    if(GradientImage.IsNull()) {
        GetGradient();
    }
    if(LabelImage) {
        delete[] LabelImage;
    }

    GradientImage.GetPointer(Gradient);
    LabelImage = Watershed(Gradient, ImageWidth, ImageHeight, OrderInvariant, Connectivity);
    HideResults = false;

    SetClientSize(hWnd);
    UpdateDisplay();
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);

    LatestAlgorithm = LA_Watershed;
}

template<class T> VOID
GradientTypeDependentFunctions<T>::
OnToboggan(HWND hWnd)
{
    T* Gradient;

    if(!FileLoaded) {
        return;
    }
    if(GradientImage.IsNull()) {
        GetGradient();
    }
    if(LabelImage) {
        delete[] LabelImage;
    }

    GradientImage.GetPointer(Gradient);
    LabelImage = Toboggan(Gradient, ImageWidth, ImageHeight, OrderInvariant, Connectivity);
    HideResults = false;

    SetClientSize(hWnd);
    UpdateDisplay();
    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);

    LatestAlgorithm = LA_Toboggan;
}

