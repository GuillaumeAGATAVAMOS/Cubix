#include <iostream>
#include <utility>
#include <RQ_OBJECT.h>
#include "TH2.h"
#include "TString.h"
#include "TNamed.h"
#include "TObject.h"
#ifndef Cube_Player_H
#define Cube_Player_H



namespace Gw {

    class Cube_Player : public TNamed
    {
        RQ_OBJECT("Cube_Player")

    private:
        Int_t fLastX, fLastY;
        Int_t x;Int_t y;
        Int_t fLastMouseX;              // last x mouse position
        Int_t fLastMouseY;              // last y mouse position
        TH1F *Current_Spectra;
        TH1F *Current_Second_Spectra; //second spectra when Search polluant is active
        TObjArray *Current_Array;

    protected:

        void Draw_Rect_AND_Name_Z(Bool_t Mode=false);
        void Draw_Rect_AND_Name_A(Bool_t Mode=false);

    public:

        Cube_Player(const char* name, const char *title);
        virtual ~Cube_Player();

        TString ReturnZWithALatex(Int_t Zz,Int_t Aa);

        void Plot_Current_Spectra(Int_t z=34,Int_t a=88,const char *OPTION="N",const char * OPTION2="N",Float_t StrengthMin=0.,Bool_t Same=0,const char *Quality_Factor="0.5",const char *Gate="N 0",Float_t Gate_W=2.0,Int_t Max_Entries=1000000000);

        void SetCanvas();

        void Redraw();

        void Display_All_Nuclei();

        void XEventAction(Int_t event, Int_t px, Int_t py, TObject *obj);

        static Bool_t OpenContextMenu(const char* method, TObject* obj, const char* alt_method_name = "");

        void DrawSpectra(const char * Nuclei_Name="88Se",const char * Option="N",const char *OPTION2="N",Float_t StrengthMin=0,Bool_t Same=0,const char* Quality_Factor="0.5",const char *Gate="N 0",Float_t Gate_W=2.0,Int_t Max_Entries=1000000000);

        void Draw_Serie_Isotopique(Int_t z,Int_t AMin,Int_t AMax);

        Bool_t IsLsDraw(const char * Option="N");

        Bool_t IsVMode(const char * Option="N");

        signed short int *WhichMode(const char *Option);
        signed short int IsAMode(const char *Option);
        signed short int IsA3Mode(const char *Option);
        signed short int IsA4Mode(const char *Option);
        signed short int IsA2Mode(const char *Option);
        signed short int IsZMode(const char * OPTION2="N");

        void Plot_Main_Wastes(Int_t MCurrent);

        void Search_Coinc();

        void Draw_Arrow(Float_t Scale,Float_t Max_Position,Float_t Energy,Int_t Color,Int_t Zz,Int_t A);

        void Add_Gamma(const char* Name_Nuclei="86As");
        void Estimate_BackGround(Int_t Z,Int_t A,Float_t Energy_Ref,Float_t Height,Color_t Color);
        void Draw_Gated_Intensities(const char * Name_Nuclei="88Se",Float_t Energy_Ref=588.,Float_t Height=100,Color_t Color=kRed,Float_t Gate=588.,Bool_t SAME=false);
        void Draw_Double_Gated_Intensities(const char * Name_Nuclei="88Se",Float_t Energy_Ref=588,Float_t Height=100,Color_t Color=kRed,Float_t Gate1=0,Float_t Gate2=0,Bool_t SAME=false);


        ClassDef(Cube_Player,1)

            };

}//namespace

#endif
