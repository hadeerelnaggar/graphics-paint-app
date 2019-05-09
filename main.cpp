#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>
#include <stack>
#include <queue>
#include <string>
#include <vector>
using namespace std;
#define ID_LINE_DDA 0
#define ID_LINE_PARAMETRIC 1
#define ID_LINE_MIDPOINT 2
#define ID_ELLIPSE_DIRECT 3
#define ID_ELLIPSE_MIDPOINT 4
#define ID_ELLIPSE_POLAR 5
#define ID_CURVE_HERMITE 6
#define ID_CURVE_BEZIER 7
#define ID_CURVE_SPLINES 8
#define ID_CLIPPING 9
#define ID_SAVE 10
#define ID_LOAD 11
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");
struct point
{
    int x;
    int y;
};
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Code::Blocks Template Windows App"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}
vector<point>pixels;
void setpixel2(HDC hdc,int x,int y,COLORREF c)
{
    point a;
    a.x=x;
    a.y=y;
    pixels.push_back(a);
    SetPixel(hdc,x,y,c);
}
int Round(double v)
{
    return (int)(v + 0.5);
}

void Swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}

void Line(HDC hdc, int xs, int ys, int xe, int ye)
{
    COLORREF color = RGB(0, 0, 0);
    int x, y;
    int dx = xe - xs;
    int dy = ye - ys;
    double dt = 1.0/sqrt(dx*dx + dy*dy);
    for(double t = 0 ; t <=1 ; t+= dt)
    {
        x = Round(xs + t*dx);
        y = Round(ys + t*dy);
        setpixel2(hdc, x, y, color);
    }
}
void DDA(HDC hdc, int xs, int ys, int xe, int ye)
{
    COLORREF color = RGB(255,0,0);
    int dx = xe - xs;
    int dy = ye - ys;
    if(abs(dy) <= abs(dx))
    {
        if(dx < 0)
        {
            Swap(xe,xs);
            Swap(ye,ys);
            dx = -dx;
            dy = -dy;
        }
        double m = 0;
        if(dx != 0)
        {
            m = (double)dy/dx;
        }
        setpixel2(hdc,xs,ys,color);
        int x = xs;
        double y = (double)ys;
        while(x<xe)
        {
            x += 1 ;
            y += m ;
            setpixel2(hdc,x,Round(y),color);
        }
    }
    else
    {
        if(dy < 0)
        {
            Swap(xe,xs);
            Swap(ye,ys);
            dx = -dx;
            dy = -dy;
        }
        double m = 0;
        double change = 0;
        if(dx != 0)
        {
            m = (double)dy/dx;
            change = 1.0/m;
        }
        setpixel2(hdc,xs,ys,color);
        int y = ys;
        double x = (double)xs;
        while(y<ye)
        {
            y += 1 ;
            x += change ;
            setpixel2(hdc,Round(x),y,color);
        }
    }
}

void Linemidpoint(HDC hdc, int xs, int ys, int xe, int ye,COLORREF color)
{

    int dx = xe - xs;
    int dy = ye - ys;
    if(abs(dy) <= abs(dx))
    {
        if(dx < 0)
        {
            Swap(xe,xs);
            Swap(ye,ys);
            dx = -dx;
            dy = -dy;
        }
        int d = dx - abs(2*dy) ;
        int d1 = 2*dx - abs(2*dy);
        int d2 = -2*abs(dy) ;
        setpixel2(hdc,xs,ys,color);
        int x = xs;
        int y = ys;
        while(x<xe)
        {
            x++;
            if(d<=0)
            {
                d+=d1;
                if(dy<=0)
                    y-=1;
                else
                    y+=1;
            }
            else
            {
                d+=d2;
            }
            setpixel2(hdc,x,y,color);
        }
    }
    else
    {
        if(dy < 0)
        {
            Swap(ye,ys);
            Swap(xe,xs);
            dy = -dy;
            dx = -dx;
        }
        int d = dy - abs(2*dx) ;
        int d1 = 2*dy - abs(2*dx);
        int d2 = -2*abs(dx) ;

        setpixel2(hdc,xs,ys,color);
        int y = ys;
        int x = xs;
        while(y<ye)
        {
            y++;
            if(d<=0)
            {
                d+=d1;
                if(dx<=0)
                    x-=1;
                else
                    x+=1;
            }
            else
            {
                d+=d2;
            }
            setpixel2(hdc,x,y,color);
        }
    }

}

void Ellipsedirectly(HDC hdc, int xc, int yc, int a, int b, COLORREF c)
{
    Ellipse(hdc, xc, yc, a, b);
}

void Ellipsepolar(HDC hdc, int xc, int yc, int a, int b)
{
    COLORREF c=RGB(255,0,0);
    int x, y;
    for(double theta = 0; theta <= 6.28; theta += .007)
    {
        x = round(xc + a * cos(theta));
        y = round(yc + b * sin(theta));
        setpixel2(hdc, x, y, c);
    }
}

void drawFourPoints(HDC hdc, int xc, int yc, int x, int y, COLORREF c)
{
    setpixel2(hdc,xc + x, yc + y,c);
    setpixel2(hdc,xc - x, yc + y,c);
    setpixel2(hdc,xc + x, yc - y,c);
    setpixel2(hdc,xc - x, yc - y,c);
}

void drawEllipseMidpoint(HDC hdc, int xc, int yc, int a, int b, COLORREF c)
{
    /// region 1
    int x = 0, y = b;
    drawFourPoints(hdc, xc, yc, x, y, c);
    int a2 = a * a;
    int b2 = b * b;
    int d = (b2) - (a2 * b) + round(a2 / 4.0);
    int d1 = (3 * b2) - (2 * b * a2) + (2 * a2);
    int d2 = (3 * b2);
    int delta1 = 2 * b2 + 2 * a2;
    int delta2 = 2 * b2;
    while(d1 < 2 * a * a + 3 * b * b)
    {
        if(d > 0)
            y--, d += d1, d1 += delta1, d2 += delta2;
        else
            d += d2, d1 += delta2, d2 += delta2;

        x++;
        drawFourPoints(hdc, xc, yc, x, y, c);
    }
    ///region 2
    d1 = -2 * a2 * y + 3 * a2;
    d2 = 2 * b2 * x + 2 + d1;
    delta1 = 2 * a2;
    delta2 = 2 * (b2 + a2);
    while(y >= 0)
    {
        if(d > 0)
            d += d1, d1 += delta1, d2 += delta1;
        else
            x++, d += d2, d1 += delta1, d2 += delta2;
        y--;
        drawFourPoints(hdc, xc, yc, x, y, c);
    }
}

int xgetintersection(point point1,point point2,int x)
{
    int x1=point1.x;
    int y1=point1.y;
    int x2=point2.x;
    int y2=point2.y;
    return y1+(x-x1)*(y2-y1)/(x2-x1);
}
int ygetintersection(point point1,point point2,int y)
{
    int x1=point1.x;
    int y1=point1.y;
    int x2=point2.x;
    int y2=point2.y;
    return x1+(y-y1)*(x2-x1)/(y2-y1);
}
bool checkduplicate(point newpoint,vector <point> x)
{
    for(int i=0; i<x.size(); i++)
    {
        if(newpoint.x==x[i].x && newpoint.y==x[i].y)
        {
            return true;
        }
    }
    return false;
}
vector<point> cliptop(int ytop,vector<point> x)
{
    vector<point> y;
    for(int i=0; i<x.size(); i++)
    {
        int j=i+1;
        if(i==x.size()-1)
        {
            j=0;
        }
        cout<<"point is"<<x[i].x<<" "<<x[i].y<<" and "<<x[j].x<<" "<<x[j].y<<endl;
        if(x[i].y>=ytop && x[j].y>=ytop)
        {
            cout<<"in"<<endl;
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);
                cout<<"both in"<<x[j].x<<" "<<x[j].y<<endl;

            }
        }
        else if(x[i].y>=ytop && x[j].y<ytop)   ///first in second out
        {
            cout<<"first in second out"<<endl;
            point z;
            z.y=ytop;
            z.x=ygetintersection(x[i],x[j],ytop);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);
                cout<<"first in last out"<<z.x<<" "<<z.y<<endl;

            }
        }
        else if(x[i].y<ytop && x[j].y>=ytop)   ///first out second in
        {
            cout<<"first out second in"<<endl;
            point z;
            z.y=ytop;
            z.x=ygetintersection(x[i],x[j],ytop);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);
                cout<<"first out last in"<<z.x<<" "<<z.y<<endl;
            }
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);
                cout<<x[j].x<<" "<<x[j].y<<endl;

            }
        }
    }
    return y;
}
vector<point> clipbottom(int ybottom,vector<point> x)
{
    vector<point> y;
    for(int i=0; i<x.size(); i++)
    {
        int j=i+1;
        if(i==x.size()-1)
        {
            j=0;
        }
        if(x[i].y<=ybottom && x[j].y<=ybottom)
        {
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);

            }
        }
        else if(x[i].y<=ybottom && x[j].y>ybottom)   ///first in second out
        {
            point z;
            z.y=ybottom;
            z.x=ygetintersection(x[i],x[j],ybottom);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);

            }

        }
        else if(x[i].y>ybottom && x[j].y<=ybottom)   ///first out second in
        {
            point z;
            z.y=ybottom;
            z.x=ygetintersection(x[i],x[j],ybottom);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);

            }
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);

            }

        }
    }
    return y;
}
vector<point> clipleft(int xleft,vector<point> x)
{
    vector<point> y;
    for(int i=0; i<x.size(); i++)
    {
        int j=i+1;
        if(i==x.size()-1)
        {
            j=0;
        }
        if(x[i].x>=xleft && x[j].x>=xleft)
        {
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);
                cout<<"both in"<<x[j].x<<" "<<x[j].y<<endl;
            }
        }
        else if(x[i].x>=xleft && x[j].x<xleft)   ///first in second out
        {
            point z;
            z.x=xleft;
            z.y=xgetintersection(x[i],x[j],xleft);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);
                cout<<"first in last out "<<z.x<<" "<<z.y<<endl;
            }
        }
        else if(x[i].x<xleft && x[j].x>=xleft)   ///first out second in
        {
            point z;
            z.x=xleft;
            z.y=xgetintersection(x[i],x[j],xleft);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);
                cout<<"first out last in"<<z.x<<" "<<z.y<<endl;
            }
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);
                cout<<x[j].x<<" "<<x[j].y<<endl;
            }
        }
    }
    return y;
}
vector<point> clipright(int xright,vector<point> x)
{
    vector<point> y;
    for(int i=0; i<x.size(); i++)
    {
        int j=i+1;
        if(i==x.size()-1)
        {
            j=0;
        }
        if(x[i].x<=xright && x[j].x<=xright)
        {
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);

            }
        }
        else if(x[i].x<=xright && x[j].x>xright)   ///first in second out
        {
            point z;
            z.x=xright;
            z.y=xgetintersection(x[i],x[j],xright);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);

            }
        }
        else if(x[i].x>xright && x[j].x<=xright)   ///first out second in
        {
            point z;
            z.x=xright;
            z.y=xgetintersection(x[i],x[j],xright);
            if(!checkduplicate(z,y))
            {
                y.push_back(z);

            }
            if(!checkduplicate(x[j],y))
            {
                y.push_back(x[j]);

            }
        }
    }
    return y;
}
void drawclippingwindow(HDC hdc,int xleft,int xright,int ytop,int ybottom)
{
    Linemidpoint(hdc,xleft,ytop,xright,ytop,RGB(255,0,0));
    Linemidpoint(hdc,xleft,ytop,xleft,ybottom,RGB(255,0,0));
    Linemidpoint(hdc,xright,ytop,xright,ybottom,RGB(255,0,0));
    Linemidpoint(hdc,xleft,ybottom,xright,ybottom,RGB(255,0,0));
}
void drawpolygon(HDC hdc,vector<point>x,COLORREF c)
{
    for(int i=0; i<x.size(); i++)
    {
        int j=i+1;
        if(i==x.size()-1)
        {
            j=0;
        }
        Linemidpoint(hdc,x[i].x,x[i].y,x[j].x,x[j].y,c);
    }
}
void polygonclipping(HDC hdc,int xleft,int xright,int ytop,int ybottom, vector<point>x)
{
    drawclippingwindow(hdc,xleft,xright,ytop,ybottom);
    cout<<"before clipping:"<<endl;
    for(int i=0; i<x.size(); i++)
    {
        cout<<x[i].x<<" "<<x[i].y<<endl;
    }
    vector <point> y=cliptop(ytop,x);
    cout<<"after clipping top:"<<endl;
    for(int i=0; i<y.size(); i++)
    {
        cout<<y[i].x<<" "<<y[i].y<<endl;
    }
    y=clipleft(xleft,y);
    cout<<"after clipping left:"<<endl;
    for(int i=0; i<y.size(); i++)
    {
        cout<<y[i].x<<" "<<y[i].y<<endl;
    }
    y=clipbottom(ybottom,y);
    cout<<"after clipping bottom:"<<endl;
    for(int i=0; i<y.size(); i++)
    {
        cout<<y[i].x<<" "<<y[i].y<<endl;
    }
    y=clipright(xright,y);
    cout<<"after clipping right:"<<endl;
    for(int i=0; i<y.size(); i++)
    {
        cout<<y[i].x<<" "<<y[i].y<<endl;
    }
    drawpolygon(hdc,y,RGB(255,0,0));
    y.clear();
}
void beziercurve(HDC hdc,point point1,point point2,point point3,point point4,COLORREF c)
{
    double alpha1,alpha2,alpha0,alpha3;
    alpha0=point1.x;
    alpha1=(-3*point1.x)+(3*point2.x);
    alpha2=(3*point1.x)+(-6*point2.x)+(3*point3.x);
    alpha3=(-1*point1.x)+(3*point2.x)+(-3*point3.x)+(1*point4.x);
    double beta0,beta1,beta2,beta3;
    beta0=point1.y;
    beta1=(-3*point1.y)+(3*point2.y);
    beta2=(3*point1.y)+(-6*point2.y)+(3*point3.y);
    beta3=(-1*point1.y)+(3*point2.y)+(-3*point3.y)+(1*point4.y);
    int x=round(point1.x);
    int y=round(point1.y);
    setpixel2(hdc,x,y,c);
    while(x<point4.x)
    {
        for(double t=0; t<1; t+=0.001)
        {
            x=round(alpha0+(alpha1*t)+(alpha2*t*t)+(alpha3*t*t*t));
            y=round(beta0+(beta1*t)+(beta2*t*t)+(beta3*t*t*t));
            setpixel2(hdc,x,y,c);
        }
    }
}
void hermitecurve(HDC hdc,point point1,point point2,point point3,point point4,COLORREF c)
{
    double alpha1,alpha2,alpha0,alpha3;
    alpha0=point1.x;
    alpha1=point2.x;
    alpha2=(-3*point1.x)+(-2*point2.x)+(3*point3.x)+(-1*point4.x);
    alpha3=(2*point1.x)+(1*point2.x)+(-2*point3.x)+(1*point4.x);
    double beta0,beta1,beta2,beta3;
    beta0=point1.y;
    beta1=point2.y;
    beta2=(-3*point1.y)+(-2*point2.y)+(3*point3.y)+(-1*point4.x);
    beta3=(2*point1.y)+(1*point2.y)+(-2*point3.y)+(1*point4.y);
    int x=round(point1.x);
    int y=round(point1.y);
    setpixel2(hdc,x,y,c);
    while(x<=point4.x)
    {
        for(double t=0; t<1 && x<=point4.x; t+=0.001)
        {
            x=round(alpha0+(alpha1*t)+(alpha2*t*t)+(alpha3*t*t*t));
            y=round(beta0+(beta1*t)+(beta2*t*t)+(beta3*t*t*t));
            setpixel2(hdc,x,y,c);
        }
    }
}
void makemenu(HWND hwnd)
{
    HMENU hMenu, hSubMenu,hsubsubMenu1,hsubsubMenu2,hsubsubMenu3;
    HANDLE hIcon, hIconSm;

    hMenu = CreateMenu();

    hSubMenu = CreatePopupMenu();
    hsubsubMenu1=CreatePopupMenu();
    hsubsubMenu2=CreatePopupMenu();
    hsubsubMenu3=CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING | MF_POPUP, (UINT)hsubsubMenu1, "&Line");
    AppendMenu(hsubsubMenu1, MF_STRING, ID_LINE_DDA, "&DDA");
    AppendMenu(hsubsubMenu1, MF_STRING, ID_LINE_MIDPOINT, "&Midpoint");
    AppendMenu(hsubsubMenu1, MF_STRING, ID_LINE_PARAMETRIC, "&Parametric");
    AppendMenu(hSubMenu, MF_STRING | MF_POPUP, (UINT)hsubsubMenu2, "&Ellipse");
    AppendMenu(hsubsubMenu2, MF_STRING, ID_ELLIPSE_DIRECT, "&Direct");
    AppendMenu(hsubsubMenu2, MF_STRING, ID_ELLIPSE_MIDPOINT, "&Midpoint");
    AppendMenu(hsubsubMenu2, MF_STRING, ID_ELLIPSE_POLAR, "&Polar");
    AppendMenu(hSubMenu, MF_STRING, ID_CLIPPING, "&Clipping");
    AppendMenu(hSubMenu, MF_STRING | MF_POPUP, (UINT)hsubsubMenu3, "&Curve");
    AppendMenu(hsubsubMenu3, MF_STRING, ID_CURVE_HERMITE, "&Hermite");
    AppendMenu(hsubsubMenu3, MF_STRING, ID_CURVE_BEZIER, "&Bezier");
    AppendMenu(hsubsubMenu3, MF_STRING, ID_CURVE_SPLINES, "&Splines");
    AppendMenu(hSubMenu, MF_STRING, ID_SAVE, "&Save");
    AppendMenu(hSubMenu, MF_STRING, ID_LOAD, "&Load");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&draw");
    SetMenu(hwnd, hMenu);
}
void save()
{
    cout<<pixels.size()<<"size"<<endl;
    ofstream fout("data.txt", ios::out | ios::binary);
    for(int i=0; i<pixels.size(); i++)
    {
        cout<<pixels[i].x<<endl;
        fout<<pixels[i].x<<" "<<pixels[i].y<<" ";
    }
    fout.close();
}
void load(HDC hdc,COLORREF c)
{
    ifstream fout("data.txt", ios::in | ios::binary);
    point w;
    cout<<"hajsh"<<endl;
    while(!fout.eof())
    {
        fout>>w.x;
        fout>>w.y;
        pixels.push_back(w);
    }
    cout<<"hahaha"<<endl;
    for(int i=0; i<pixels.size(); i++)
    {
        SetPixel(hdc,pixels[i].x,pixels[i].y,c);
        //cout<<pixels[i].x<<" "<<pixels[i].y<<endl;
    }
    fout.close();
}
HWND button_line,button_ellipse,button_clipping,button_save,button_load;
PAINTSTRUCT p;
bool arr[12];
bool first=false;
vector<point> vec;
int X1,X2,X3,X4,Y1,Y2,Y3,Y4;
bool p1=false,p2=false,p3=false,p4=false;
point point1,point2,point3,point4;
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        makemenu(hwnd);
    }
    break;
    case WM_COMMAND:
        cout<<"loword:"<<LOWORD(wParam)<<" Hiword:"<<HIWORD(wParam)<<endl;
        if(LOWORD(wParam)>=0 && LOWORD(wParam)<=11)
        {
            for(int i=0; i<12; i++)
            {
                if(i==LOWORD(wParam))
                {
                    arr[i]=true;
                }
                else
                {
                    arr[i]=false;
                }
            }
        }
        break;
    case WM_LBUTTONDOWN:
        cout<<"lbuttondown"<<endl;
        for(int j=0; j<12; j++)
        {
            if(arr[j]==true)
            {
                switch(j)
                {
                case ID_LINE_DDA:
                case ID_LINE_MIDPOINT:
                case ID_LINE_PARAMETRIC:
                    cout<<"line"<<endl;
                    X1=LOWORD(lParam);
                    Y1=HIWORD(lParam);
                    p1=true;
                    break;
                case ID_ELLIPSE_DIRECT:
                    ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                    Ellipsedirectly(p.hdc,200,200,90,120,RGB(255,0,255));
                    break;
                case ID_ELLIPSE_MIDPOINT:
                    ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                    drawEllipseMidpoint(p.hdc,200,200,90,120,RGB(255,0,0));
                    break;
                case ID_ELLIPSE_POLAR:
                    ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                    Ellipsepolar(p.hdc,200,200,90,120);
                    break;
                case ID_CLIPPING:
                    if(!p1){
                      cout<<"getting first point in window"<<endl;
                      X1=LOWORD(lParam);
                      Y1=HIWORD(lParam);
                      cout<<"first point"<<X1<<" "<<Y1<<endl;
                      p1=true;
                      vec.clear();
                    }
                    if(p1 && p3){
                    cout<<"new point"<<endl;
                    point1.x=LOWORD(lParam);
                    point1.y=HIWORD(lParam);
                    cout<<"point:"<<point1.x<<" "<<point1.y<<endl;
                    vec.push_back(point1);}
                    break;
                case ID_CURVE_BEZIER:
                case ID_CURVE_HERMITE:
                case ID_CURVE_SPLINES:
                    if(!p1 && !p2 && !p3 && !p4)
                    {
                        point1.x=LOWORD(lParam);
                        point1.y=HIWORD(lParam);
                        p1=true;
                    }
                    else if(p1 && !p2 && !p3 && !p4)
                    {
                        point2.x=LOWORD(lParam);
                        point2.y=HIWORD(lParam);
                        p2=true;
                    }
                    else if(p1 && p2 && !p3 && !p4)
                    {
                        point3.x=LOWORD(lParam);
                        point3.y=HIWORD(lParam);
                        p3=true;
                    }
                    else if(p1 && p2 && p3 && !p4)
                    {
                        point4.x=LOWORD(lParam);
                        point4.y=HIWORD(lParam);
                        p4=true;
                    }
                    break;
                }
            }
        }
        break;
    case WM_LBUTTONUP:
                cout<<"button up"<<endl;
                for(int i=0; i<12; i++)
                {
                    if(arr[i]==true)
                    {
                        if(first==false)
                        {
                            first=true;
                        }
                        switch(i)
                        {
                        case ID_LINE_DDA:
                        case ID_LINE_MIDPOINT:
                        case ID_LINE_PARAMETRIC:
                            cout<<"line up"<<endl;
                            if(p1)
                            {
                                X2=LOWORD(lParam);
                                Y2=HIWORD(lParam);
                                p2=true;
                            }
                            break;
                        case ID_ELLIPSE_DIRECT:
                            ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                            Ellipsedirectly(p.hdc,200,200,90,120,RGB(255,0,255));
                            break;
                        case ID_ELLIPSE_MIDPOINT:
                            ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                            drawEllipseMidpoint(p.hdc,200,200,90,120,RGB(255,0,0));
                            break;
                        case ID_ELLIPSE_POLAR:
                            ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                            Ellipsepolar(p.hdc,200,200,90,120);
                            break;
                        case ID_CLIPPING:
                            if(p1 && !p3)
                            {
                                cout<<"second point in window"<<endl;
                                X2=LOWORD(lParam);
                                Y2=HIWORD(lParam);
                                cout<<"second point"<<X2<<" "<<Y2<<endl;
                                p3=true;
                            }
                            break;
                        }
                    }
                }
                break;
            case WM_RBUTTONDOWN:
        for(int i=0; i<12; i++)
        {
            if(arr[i]==true)
            {
                switch(i)
                {
                case ID_CLIPPING:
                    p2=true;
                    break;
                }
            }
        }

            case WM_PAINT:
                cout<<"paint"<<endl;
                for(int i=0; i<12; i++)
                {
                    if(arr[i]==true)
                    {
                        if(first==false)
                        {
                            BeginPaint(hwnd,&p);
                            first=true;
                            p1=false,p2=false,p3=false,p4=false;
                        }
                        switch(i)
                        {
                        case ID_LINE_DDA:
                            cout<<"painting DDA"<<endl;
                            ///bya5ood x1,y1,x2,y2
                            if(p1 && p2)
                                DDA(p.hdc,X1,Y1,X2,Y2);
                            p1=false;
                            p2=false;
                            break;
                        case ID_LINE_MIDPOINT:
                            cout<<"painting midpoint"<<endl;
                            ///bya5ood x1,y1,x2,y2
                            if(p1 && p2)
                                Linemidpoint(p.hdc,X1,Y1,X2,Y2,RGB(255,0,0));
                            p1=false;
                            p2=false;
                            X1=0,Y1=0,X2=0,Y2=0;
                            break;
                        case ID_LINE_PARAMETRIC:
                            ///bya5ood x1,y1,x2,y2
                            if(p1 && p2)
                                Line(p.hdc,X1,Y1,X2,Y2);
                            p1=false;
                            p2=false;
                            break;
                        case ID_ELLIPSE_DIRECT:
                            ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                            Ellipsedirectly(p.hdc,200,200,90,120,RGB(255,0,255));
                            break;
                        case ID_ELLIPSE_MIDPOINT:
                            ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                            drawEllipseMidpoint(p.hdc,200,200,90,120,RGB(255,0,0));
                            break;
                        case ID_ELLIPSE_POLAR:
                            ///bya5ood x,y,center w a el-length mn center l ymeen w b el-length mn center l foo2
                            Ellipsepolar(p.hdc,200,200,90,120);
                            break;
                        case ID_CLIPPING:
                            if(p1 && p2 && p3)
                            {
                                cout<<"clipping"<<endl;
                                cout<<X1<<" "<<X2<<" "<<Y1<<" "<<Y2<<endl;
                                polygonclipping(p.hdc,X1,X2,Y1,Y2,vec);
                                vec.clear();
                                p1=false;
                                p2=false;
                                p3=false;
                            }
                            break;
                        case ID_SAVE:
                            save();
                            break;
                        case ID_LOAD:
                            load(p.hdc,RGB(255,50,90));
                            break;
                        case ID_CURVE_BEZIER:
                            cout<<"painting curve"<<endl;
                            if(p1 && p2 && p3 && p4)
                            {
                                beziercurve(p.hdc,point1,point2,point3,point4,RGB(255,90,80));

                                p1=false,p2=false,p3=false,p4=false;
                            }
                            break;
                        case ID_CURVE_HERMITE:
                            if(p1 && p2 && p3 && p4)
                            {
                                hermitecurve(p.hdc,point1,point2,point3,point4,RGB(90,170,80));
                                p1=false,p2=false,p3=false,p4=false;
                            }
                            break;
                        case ID_CURVE_SPLINES:
                            if(p1 && p2 && p3 && p4)
                            {

                            }
                            p1=false,p2=false,p3=false,p4=false;
                            break;
                        }

                        break;
                        EndPaint(hwnd,&p);
                    }
                }
                break;
            case WM_DESTROY:
                PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
                break;
            default:                      /* for messages that we don't deal with */
                return DefWindowProc (hwnd, message, wParam, lParam);
            }

            return 0;
        }
