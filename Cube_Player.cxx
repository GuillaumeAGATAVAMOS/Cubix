#include "includes_cube.h"

#include "KeySymbols.h"

#ifndef ROOT_TStyle
#include "TStyle.h"
#endif

#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif

#ifndef ROOT_TSpectrum
#include "TSpectrum.h"
#endif

#ifndef ROOT_TFrame
#include "TFrame.h"
#endif

#ifndef ROOT_TROOT
#include "TROOT.h"
#endif

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
#include "TMath.h"
#endif

#ifndef Cube_Player_H
#include "Cube_Player.h"
#endif

#include "ENSDF_DATAe680.h"
#include "Build_LS.h"
#include "Spectrum_Loader.h"

#include "TContextMenu.h"
#include "TArrow.h"
#include "TObjString.h"
#include "TObjArray.h"

using namespace std;
using namespace Gw;

Cube_Player::Cube_Player(const char* name, const char *title) :
        TNamed(name,title)
{

}


Cube_Player::~Cube_Player()
{
C_Chart->Disconnect();
C_Chart->DisconnectWidget();
delete Current_Spectra;
}


void Cube_Player::SetCanvas()
{
    // to connect the current canvas
    TVirtualPad *pad = TVirtualPad::Pad(); TCanvas *c = NULL;

    if ( pad ) {
        c = pad->GetCanvas();
    }

    c->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
               "Gw::Cube_Player",this,"XEventAction(Int_t,Int_t,Int_t,TObject*)");

    cout<<endl;InfoMessage("Canvas is connected; you can select nuclei and see gamma spectra");cout<<endl;
    InfoMessage("Type 'r' to begin moving on the chart");cout<<endl;
    InfoMessage("Type 'h' to display help");cout<<endl;
}

void Cube_Player::XEventAction(Int_t event, Int_t px, Int_t py, TObject */*sel*/)
{
    // Do action depending on X actions

    TVirtualPad *pad = gROOT->GetSelectedPad(); if ( pad == NULL ) return;



    bool CTRL ;
    if(px==0 && py==4129){CTRL=true;}else{CTRL=false;}


    bool LastCTRL ;
    if(fLastX==0 && fLastY==4129){LastCTRL=true;}else{LastCTRL=false;}

    EKeySym Key = (EKeySym)py;

    switch (event) {
    case kMouseMotion:
        // to keep track of the last mouse position
        fLastX = px; fLastY = py;
        break;



    case kKeyPress:
        {


            Int_t key = px, keysym = py;
            switch ((EKeySym)keysym) {
            case kKey_Left:
                cout<<"left arrow"<<endl;
                break;
            case kKey_Right:
                cout<<"rightarrow"<<endl;
                break;
            case kKey_Down:
                cout<<"down arrow"<<endl;
                break;
            case kKey_h:
                cout<<endl;
                cout<<endl;
                WarningMessage("*********************************************************************************************");cout<<endl;
                WarningMessage("***************************HELP AND PROGRAM Last UPDATE : 12/06/2016*************************");cout<<endl;
                WarningMessage("*********************************************************************************************");cout<<endl;
                InfoMessage("HELP : Keys used to interact with nucleide chart:");cout<<endl;
                InfoMessage("      h->Show Help");cout<<endl;
                ErrorMessage("      CTRL+ h->Show Special Help for Gated Spectra");cout<<endl;
                InfoMessage("      r->Reset coordinates of the chart");cout<<endl;
                InfoMessage("      z->Increase by one proton");cout<<endl;
                InfoMessage("      s->Decrease by one proton");cout<<endl;
                InfoMessage("      d->Increase by one neutron");cout<<endl;
                InfoMessage("      q->Increase by one neutron");cout<<endl;
                InfoMessage("      Pave_Num_Enter->Show Spectrum of the selected nucleus");cout<<endl;
                InfoMessage("      Ctrl+a -> Draw All Nuclei");cout<<endl;
                InfoMessage("      Ctrl+r -> Reset Drawing");cout<<endl;
                ErrorMessage("-------------------------------------------------------------------");cout<<endl;
                InfoMessage("      Ctrl+d -> Draw Spectrum of one Nuclei ex: 100Zr. Option : L(Draw LS), V(Verbose Mode) ");cout<<endl;
                InfoMessage("                OPTION2(a) : Z+1;Z-1;Z+- -> Draw Energies of the Z+(-)(+and-) Nuclei");cout<<endl;
                InfoMessage("                OPTION2(b) : A+1;A-1;A+- -> Draw Energies of the A+(-)(+and-) Nuclei");cout<<endl;
                InfoMessage("                OPTION2(c) : A++;A--;A++-- -> Draw Energies of the A+2(-2)(+2and-2) Nuclei");cout<<endl;
                InfoMessage("                OPTION2(d) : A+++;A---;A+++--- -> Draw Energies of the A+3(-3)(+3and-3) Nuclei");cout<<endl;
                InfoMessage("                OPTION2(e) : A+4;A-4;A++++---- -> Draw Energies of the A+4(-4)(+4and-4) Nuclei");cout<<endl;
                InfoMessage("       All Options (a),(b),(c) can be called together separated with comma");cout<<endl;
                InfoMessage("       Ex: if one want display all contaminants, he should write :");cout<<endl;
                InfoMessage("       Z+- A+- A++-- A+++--- A++++-----");cout<<endl;
                InfoMessage("       Accepted forms : 'Z+1,'z+1'','Z+,'z+,'Z-1,'z-1,'Z-,'z-,'z+-,'Z+-,'Z-+,'z-+''");
                InfoMessage("       Accepted forms : 'A+1,'a+1,'A+,'a+,'A-1,'a-1,'A-,'a-,'A+-,'a+-,'A-+,'a-+");
                InfoMessage("       Accepted forms : 'A+2,'a+2,'A++'','a++,'A-2,'a-2,'A--,'a--,'A++--','a++--,'A--++,'a--++'");
                InfoMessage("       Accepted forms : 'A+3,'a+3,'A+++','a+++,'A-3,'a-3,'A---,'a---,'A+++---','a+++---,'A---+++,'a---+++'");
                InfoMessage("       Accepted forms : 'A+4,'a+4,'A++++','a++++,'A-4,'a-4,'A----,'a----,'A++++----','a++++----,'A----++++,'a----++++'");cout<<endl;
                InfoMessage("                StrengthMin : Display only intensities above this limit");cout<<endl;
                InfoMessage("                SAME : Display new nuclei on the same Pad, one can also displays energies of new contaminants");cout<<endl;
                InfoMessage("                Quality_Factor : Trust Factor of the particule identification");cout<<endl;
                InfoMessage("       Accepted forms : 'X/Y' Where X is Z_ID and Y M_ID in case of gated/raw spectra per isotope");
                InfoMessage("       Accepted forms : 'X'or 'X/Y' Where X is M_ID in case of  gated/raw per Mass and Z_ID/M_ID in case of E versus M/Z Matrix.Y is not taken into account.");cout<<endl;
                InfoMessage("                GATE : Set of gates ( see the special Help )");cout<<endl;
                InfoMessage("                GATE_W : Gate Width");cout<<endl;
                InfoMessage("                Max_Entries : Limit the entries readed");cout<<endl;cout<<endl;
                ErrorMessage("-------------------------------------------------------------------");cout<<endl;
                InfoMessage("      Ctrl+i -> Draw Spectra of an isotopic set between Amin and Amax");cout<<endl;
                InfoMessage("      Ctrl+w -> Draw Distribution of possible polluants weighted by probabilities");cout<<endl;cout<<endl;
                InfoMessage("      Ctrl+e -> Draw Level Scheme dertermined  in this work.");cout<<endl;
                InfoMessage("                All the GLS keyboard shortcuts are availaible to modify the Level scheme");cout<<endl;
                InfoMessage("                You can save the LS in the current directory, and it will be reused to draw arrow of possible contaminants");cout<<endl;
                InfoMessage("      Ctrl+k -> Connect the canvas selected in order to use special BackGround substraction");cout<<endl;
                InfoMessage("      Ctrl+b -> Draw Theoritical raw intensities determined in this work by Loading a LvlScheme");cout<<endl;
                InfoMessage("                Energy_Ref -> Energy used as a reference in order to display other intensities");cout<<endl;
                InfoMessage("                Height -> Height of the Energy reference peak");cout<<endl;
                InfoMessage("      Ctrl+n -> Draw Theoritical gated intensities determined in this work by Loading a LvlScheme");
                ErrorMessage("      !!!! CTRL+n  DONT'T WORK  FOR THE MOMENT 12/02/2016");cout<<endl;
                InfoMessage("      Ctrl+x -> Exit");cout<<endl;
                break;
            case kKey_Up:
                cout<<"up arrow"<<endl;
                break;
            case kKey_z:
                Draw_Rect_AND_Name_Z(false);
                break;
            case kKey_d:
                Draw_Rect_AND_Name_A(false);
                break;
            case kKey_s:
                Draw_Rect_AND_Name_Z(true);
                break;
            case kKey_q:
                Draw_Rect_AND_Name_A(true);
                break;
            case kKey_r:
                x=0;y=0;
                break;
//            case   kKey_Enter://faire entrée normal
//                Plot_Current_Spectra(x,y,0);
//                break;

            default :
                    break;
        }
            break;

        }//end kKeyPress

        fLastX = px;
        fLastY = py;

    }//End Switch


    if   (Key == kKey_h  && CTRL==false && LastCTRL==true)//CTRL+h
    {
        cout<<endl;
        ErrorMessage("-------------------------------------------------------------------");cout<<endl;
        ErrorMessage("                   SPECIAL HELP ON SPECTRA ");cout<<endl;
        ErrorMessage("-------------------------------------------------------------------");cout<<endl;
        InfoMessage("      CTRL+ h->Show Special Help for Gated Spectra");cout<<endl;
        InfoMessage("      HELP ON OPTIONS TO PLOT GATED SPECTRA");cout<<endl;
        InfoMessage("      KEYWORDS :");cout<<endl;
        InfoMessage("      Default :N 0 -> No Gates");cout<<endl;
        InfoMessage("      SM-> Put simple Gate on Tree conditioned by Mass");cout<<endl;
        InfoMessage("      SI-> Put simple Gate on Tree conditioned by Mass and Z");cout<<endl;
        InfoMessage("      DM-> Put Double Gate on Tree conditioned by Mass");cout<<endl;
        InfoMessage("      T-> Put Triple Gate ...");cout<<endl;
        InfoMessage("      SA-> Put Simple Gate AGATA ONLY");cout<<endl;
        cout<<endl;
        InfoMessage("      SYNTAX :");cout<<endl;
        InfoMessage("      SM 125+256+265");cout<<endl;
        InfoMessage("      One gate with several 'or' conditions");cout<<endl;
        cout<<endl;
        InfoMessage("      DM 125.3+659.3/254.5+256.9");cout<<endl;
        InfoMessage("      Double Gate with 'or' connections");cout<<endl;
        cout<<endl;
        ErrorMessage("      HELP ON OPTIONS TO PLOT ISOBAR OR ISTONES MATRIX");cout<<endl;
        InfoMessage("      E_Z-> Plot E versus Z Matrix with current M between Current Z-3 and Current Z+3");cout<<endl;
        InfoMessage("      E_M-> Plot E versus M Matrix with current Z between Current M-3 and Current M+3");cout<<endl;
        cout<<endl;
        ErrorMessage("      HELP ON OPTIONS TO PLOT RAW SPECTRA");cout<<endl;
        InfoMessage("      B_M-> Plot Raw Spectra with the current mass");cout<<endl;
        InfoMessage("      Options are :");cout<<endl;
        InfoMessage("      B_M->Raw Spectra with all the availaible PIDs");cout<<endl;
        InfoMessage("      B_M X+/-x-> Raw Spectra for the current mass and PID X+/-x");cout<<endl;
        cout<<endl;
        ErrorMessage("      HELP ON OPTIONS TO Substract And Mutliply/Divide by another Spectra");cout<<endl;
        InfoMessage("      S_S-> KeyWord To substract Current Spectra by Another");cout<<endl;
        InfoMessage("      Options are :");cout<<endl;
        InfoMessage("      S_S X N 0->Add to the Current Spectra another Spectra Corrected by factor X");cout<<endl;
        InfoMessage("      S_S -X SI 369+542->Substract to the Current Spectra the Gated Spectra of the current Nuclei Gated on 369+542");cout<<endl;
        InfoMessage("      S_S -X DM 369+542/586->Substract to the Current Spectra the Double_Gated Spectra of the current Nuclei ");cout<<endl;

    }
    if   (Key == kKey_r  && CTRL==false && LastCTRL==true)//CTRL+r
    {
        Redraw();gPad->Modified();gPad->Update();
    }

    if   (Key == kKey_a && !CTRL && LastCTRL )//CTRL+a
    {
        Display_All_Nuclei();gPad->Modified();gPad->Update();
    }

    if   (Key == kKey_d && !CTRL && LastCTRL )//CTRL+d
    {
        OpenContextMenu("DrawSpectra",this);
    }
    if   (Key == kKey_x && !CTRL && LastCTRL )//CTRL+x
    {
        delete Cube;
        C_Chart->Close();
    }
    if   (Key == kKey_i && !CTRL && LastCTRL )//CTRL+i
    {
        OpenContextMenu("Draw_Serie_Isotopique",this);
    }
    if   (Key == kKey_w && !CTRL && LastCTRL )//CTRL+w
    {
        OpenContextMenu("Plot_Main_Wastes",this);
    }
    if   (Key == kKey_c && !CTRL && LastCTRL )//CTRL+c
    {
        OpenContextMenu("Search_Coinc",this);
    }
    if   (Key == kKey_e && !CTRL && LastCTRL )//CTRL+e
    {
        OpenContextMenu("Add_Gamma",this);
    }
    if   (Key == kKey_b && !CTRL && LastCTRL )//CTRL+b
    {
        OpenContextMenu("Estimate_BackGround",this);
    }
    if   (Key == kKey_n && !CTRL && LastCTRL )//CTRL+n
    {
        OpenContextMenu("Draw_Gated_Intensities",this);
    }
    if   (Key == kKey_m && !CTRL && LastCTRL )//CTRL+m
    {
     OpenContextMenu("Draw_Double_Gated_Intensities",this);
    }
    if(Key == kKey_k && !CTRL &&  LastCTRL)//CTRL+k
    {
    Gw::GSPlayerTUI *pl2 = new Gw::GSPlayerTUI();
    pl2->ConnectCanvas();
    gPad->SetCrosshair(0);
    }


    fLastX = px;
    fLastY = py;
    //cout<<"fLastX "<<fLastX<<"fLastY "<<fLastY<<endl;

}


void Cube_Player::Draw_Rect_AND_Name_Z(Bool_t Mode)

{

    TVirtualPad *pad = gROOT->GetSelectedPad(); if ( pad == NULL ) return;

    pad->cd();

    /*TLatex Tl;*/Tl.SetTextFont(43); Tl.SetTextSize(13);


                  if(Mode==false){x+=1;}else{x-=1;}


                  switch (x) {

                  case 0:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 1:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 2:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 3:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 4:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 5:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 6:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 7:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 8:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 9:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 10:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 11:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 12:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 13:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 14:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 15:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                  case 16:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 17:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                  case 18:
                      Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;


                  }

                  gPad->Modified();gPad->Update();

              }

void Cube_Player::Draw_Rect_AND_Name_A(Bool_t Mode)

{


    TVirtualPad *pad = gROOT->GetSelectedPad(); if ( pad == NULL ) return;

    pad->cd();

    /*TLatex Tl*/;Tl.SetTextFont(43); Tl.SetTextSize(13);

                 if(Mode==false){y+=1;}else{y-=1;}

                 switch (x) {

                 case 0:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 1:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 2:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 3:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 4:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 5:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 6:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 7:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 8:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 9:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 10:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 11:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 12:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 13:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 14:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 15:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 case 16:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 17:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;
                 case 18:
                     Tl.DrawLatex(A_Offset+y,Z_Offset+x,ReturnZWithALatex(x,A_Offset+y));   break;

                 }

                 gPad->Modified();gPad->Update();

             }

TString Cube_Player::ReturnZWithALatex(Int_t Zz,Int_t Aa)

{
    TString ZWithA="#font[22]{^{";
    ZWithA+=Aa;
    ZWithA+="}";
    ZWithA+=Z[Zz];
    ZWithA+="}";

    return ZWithA;
}

void Cube_Player::Plot_Current_Spectra(Int_t z,Int_t a,const char *OPTION,const char* OPTION2,Float_t StrengthMin,Bool_t Same,const char* Quality_Factor,const char *Gate,Float_t Gate_W,Int_t Max_Entries)
{

    Gw::Spectrum_Loader *g = new Gw::Spectrum_Loader("TOTO","TATA");

//Plot_Isotope(Int_t Zz,Int_t M,Float_t Z_ID_R,ULong64_t Max_Read_Entries)

    Float_t *E =new Float_t[Maximum_Gamma_Read];
    Float_t *Eexp =new Float_t[Maximum_Gamma_Read];
    Float_t *EZmoin =new Float_t[Maximum_Gamma_Read];
    Float_t *EZplus =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus3 =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin3 =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus2 =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin2 =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus4 =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin4 =new Float_t[Maximum_Gamma_Read];
    Float_t *EZmoinexp =new Float_t[Maximum_Gamma_Read];
    Float_t *EZplusexp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplusexp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoinexp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus3exp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin3exp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus2exp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin2exp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAplus4exp =new Float_t[Maximum_Gamma_Read];
    Float_t *EAmoin4exp =new Float_t[Maximum_Gamma_Read];

    for(Int_t h=0;h<Maximum_Gamma_Read;h++)
    {
        E[h]=0;
        EZmoin[h] =0;
        EZplus[h] =0;
        EAplus[h] =0;
        EAmoin[h]=0;
        EAplus3[h] =0;
        EAmoin3[h] =0;
        EAplus2[h] =0;
        EAmoin2[h] =0;
        EAplus4[h] =0;
        EAmoin4[h] =0;
        Eexp[h]=0;
        EZmoinexp[h] =0;
        EZplusexp[h] =0;
        EAplusexp[h] =0;
        EAmoinexp[h]=0;
        EAplus3exp[h] =0;
        EAmoin3exp[h] =0;
        EAplus2exp[h] =0;
        EAmoin2exp[h] =0;
        EAplus4exp[h] =0;
        EAmoin4exp[h] =0;

    }

    E=DATABASE->Import_E(z,a,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
    Eexp=LS->GetGammas(z+Z_Offset,a+A_Offset);

    for(Int_t h=0;h<Maximum_Gamma_Read;h++)
    {


  //cout<<"Test tab "<<Eexp[h]<<endl;


    }


    Float_t Max_Position=0,Max_PositionZplus=0,Max_PositionZmoin=0,Max_PositionAplus=0,Max_PositionAmoin=0,Max_PositionAplus3=0,Max_PositionAmoin3=0,Max_PositionExp=0,Max_PositionZplusExp=0,Max_PositionZmoinExp=0,Max_PositionAplusExp=0,Max_PositionAmoinExp=0,Max_PositionAplus3Exp=0,Max_PositionAmoin3Exp=0;
    Float_t Max_PositionAplus2=0,Max_PositionAmoin2=0,Max_PositionAmoin4=0,Max_PositionAplus4=0,Max_PositionAplus2Exp=0,Max_PositionAmoin2Exp=0,Max_PositionAmoin4Exp=0,Max_PositionAplus4Exp=0;

    signed short int *EMode =new signed short int[Number_Of_Option];

    EMode=Cube->WhichMode(OPTION2);


    for(Int_t i=0;i<Number_Of_Option;i++)
    {
       // cout<<"EMode ---------------------------------------------------- "<<EMode[i]<<endl;
        switch(EMode[i])
        {
        case -1:
            EZmoin=DATABASE->Import_E(z-1,a,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EZmoinexp=LS->GetGammas(z+Z_Offset-1,a+A_Offset);
            break;
        case 1:
            EZplus =DATABASE->Import_E(z+1,a,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EZplusexp=LS->GetGammas(z+Z_Offset+1,a+A_Offset);
            break;
        case 2:
            EZmoin =DATABASE->Import_E(z-1,a,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EZplus =DATABASE->Import_E(z+1,a,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EZmoinexp=LS->GetGammas(z+Z_Offset-1,a+A_Offset);
            EZplusexp=LS->GetGammas(z+Z_Offset+1,a+A_Offset);
            break;
        case 3:
            EAplus=DATABASE->Import_E(z,a+1,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplusexp=LS->GetGammas(z+Z_Offset,a+A_Offset+1);
            break;
        case 4:
            EAmoin =DATABASE->Import_E(z,a-1,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoinexp=LS->GetGammas(z+Z_Offset,a+A_Offset-1);
            break;
        case 5:
            EAmoin =DATABASE->Import_E(z,a-1,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplus =DATABASE->Import_E(z,a+1,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplusexp=LS->GetGammas(z+Z_Offset,a+A_Offset+1);
            EAmoinexp=LS->GetGammas(z+Z_Offset,a+A_Offset-1);
            break;
        case 6:
            EAmoin3=DATABASE->Import_E(z,a-3,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin3exp=LS->GetGammas(z+Z_Offset,a+A_Offset-3);
            break;
        case 7:
            EAplus3=DATABASE->Import_E(z,a+3,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplus3exp=LS->GetGammas(z+Z_Offset,a+A_Offset+3);
            break;
        case 8:
            EAplus3 =DATABASE->Import_E(z,a+3,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin3 =DATABASE->Import_E(z,a-3,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin3exp=LS->GetGammas(z+Z_Offset,a+A_Offset-3);
            EAplus3exp=LS->GetGammas(z+Z_Offset,a+A_Offset+3);
            break;
        case 9:
            EAplus2 =DATABASE->Import_E(z,a+2,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplus2exp=LS->GetGammas(z+Z_Offset,a+A_Offset+2);
            break;
        case 10:
            EAmoin2 =DATABASE->Import_E(z,a-2,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin2exp=LS->GetGammas(z+Z_Offset,a+A_Offset-2);
            break;
        case 11:
            EAplus2 =DATABASE->Import_E(z,a+2,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin2 =DATABASE->Import_E(z,a-2,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplus2exp=LS->GetGammas(z+Z_Offset,a+A_Offset+2);
            EAmoin2exp=LS->GetGammas(z+Z_Offset,a+A_Offset-2);
            break;
        case 12:
            EAplus4 =DATABASE->Import_E(z,a+4,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplus4exp=LS->GetGammas(z+Z_Offset,a+A_Offset+4);
            break;
        case 13:
            EAmoin4 =DATABASE->Import_E(z,a-4,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin4exp=LS->GetGammas(z+Z_Offset,a+A_Offset-4);
            break;
        case 14:
            EAplus4 =DATABASE->Import_E(z,a+4,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAmoin4 =DATABASE->Import_E(z,a-4,Cube->IsLsDraw(OPTION),Cube->IsVMode(OPTION),StrengthMin);
            EAplus4exp=LS->GetGammas(z+Z_Offset,a+A_Offset+4);
            EAmoin4exp=LS->GetGammas(z+Z_Offset,a+A_Offset-4);
            break;

        }//end Switch
    }//End for

    if(!gSystem->IsFileInIncludePath(FileName))
    {


        ErrorMessage("*************************************");
        ErrorMessage("***Spectra are not in include PATH***");
        ErrorMessage("*************************************");

    }else
    {

        Current_Spectra=0x0;
        Current_Matrix=0x0;
        Float_t Scale=0;
        if(g->Which_Gate_Mode(Gate)==4)//this means no gate
        {
            Current_Spectra=(TH1F*)g->Plot_Isotope(Z_Offset+z,a+A_Offset,Quality_Factor,Max_Entries);
            Scale=Current_Spectra->GetMaximum();
        }
        if(g->Which_Gate_Mode(Gate)==0)//this means one gate with tree per M
        {
            Current_Spectra=(TH1F*)g->Plot_Gated_Spectra(Z_Offset+z,a+A_Offset,Quality_Factor,Gate,Gate_W,Max_Entries);
            Scale=Current_Spectra->GetMaximum();
        }
        if(g->Which_Gate_Mode(Gate)==1)//this means one gate with tree per (Z,M)
        {
            Current_Spectra=(TH1F*)g->Plot_Isotope_Gated(Z_Offset+z,a+A_Offset,Quality_Factor,Gate,Gate_W,Max_Entries);
            Scale=Current_Spectra->GetMaximum();
        }
        if(g->Which_Gate_Mode(Gate)==2)//this means two gate with tree per M
        {
            Current_Spectra=(TH1F*)g->Plot_Double_Gated_Spectra(Z_Offset+z,a+A_Offset,Quality_Factor,Gate,Gate_W,Max_Entries);
            Scale=Current_Spectra->GetMaximum();
        }
        if(g->Which_Gate_Mode(Gate)==5)//
        {
        Current_Matrix=g->Plot_E_Z(Z_Offset+z,a+A_Offset,Quality_Factor,Z_Offset+z-3,Z_Offset+z+3,Max_Entries);
        }
        if(g->Which_Gate_Mode(Gate)==6)//
        {
            Current_Matrix=g->Plot_E_M(Z_Offset+z,a+A_Offset,Quality_Factor,a+A_Offset-3,a+A_Offset+3,Max_Entries);
        }
        if(g->Which_Gate_Mode(Gate)==7)//
        {
            Current_Spectra=g->Plot_B_M(Z_Offset+z,a+A_Offset,Gate,Max_Entries);
            Scale=Current_Spectra->GetMaximum();
        }
        if(g->Which_Gate_Mode(Gate)==9)//Simple gated spectra with AGATA ONLY
        {
             Current_Spectra=g->Plot_All_Gated_Spectra(Gate,Gate_W,Max_Entries);
             Scale=Current_Spectra->GetMaximum();
        }
        if(g->Which_Gate_Mode(Gate)==8)//this means substract spectra //TO COMPLETE
        {
           TList *TList_Current_Canvas=new TList();
           TList_Current_Canvas=gPad->GetListOfPrimitives();

           TH1F *Old_Spectra =new TH1F ();

           Old_Spectra=(TH1F*)TList_Current_Canvas->At(1);// en position 1 car le spectre est toujours plotté apres la frame

           Float_t Coef=1;
           TString Delimiter_S=" ";
           TString CompareOption_S=TString(Gate);

           TObjArray *d_S = CompareOption_S.Tokenize(TString(Delimiter_S));
          // cout<<"d_S get Entries "<<d_S->GetEntries()<<endl;
           TString SecondOption_S;
           if(d_S->GetEntries()>1)
           {
               //cout<<"SecondOption_S "<<atof(d_S->At(1)->GetName())<<endl;
               Coef=atof(d_S->At(1)->GetName());
           }
           if(d_S->GetEntries()>2){SecondOption_S=d_S->At(2)->GetName();SecondOption_S+=Delimiter_S;}
           if(d_S->GetEntries()>=3)SecondOption_S+=d_S->At(3)->GetName();


            //cout<<"Secon option "<<SecondOption_S<<endl;


            if(g->Which_Gate_Mode(SecondOption_S)==4)//this means no gate
            {
                Current_Spectra=(TH1F*)g->Plot_Isotope(Z_Offset+z,a+A_Offset,Quality_Factor,Max_Entries);
                Scale=Current_Spectra->GetMaximum();
            }
            if(g->Which_Gate_Mode(SecondOption_S)==0)//this means one gate with tree per M
            {
                Current_Spectra=(TH1F*)g->Plot_Gated_Spectra(Z_Offset+z,a+A_Offset,Quality_Factor,SecondOption_S,Gate_W,Max_Entries);
                Scale=Current_Spectra->GetMaximum();
            }
            if(g->Which_Gate_Mode(SecondOption_S)==1)//this means one gate with tree per (Z,M)
            {
                Current_Spectra=(TH1F*)g->Plot_Isotope_Gated(Z_Offset+z,a+A_Offset,Quality_Factor,SecondOption_S,Gate_W,Max_Entries);
                Scale=Current_Spectra->GetMaximum();
            }
            if(g->Which_Gate_Mode(SecondOption_S)==2)//this means two gate with tree per M
            {
                Current_Spectra=(TH1F*)g->Plot_Double_Gated_Spectra(Z_Offset+z,a+A_Offset,Quality_Factor,SecondOption_S,Gate_W,Max_Entries);
                Scale=Current_Spectra->GetMaximum();
            }
            if(g->Which_Gate_Mode(SecondOption_S)==7)//
            {
                Current_Spectra=g->Plot_B_M(Z_Offset+z,a+A_Offset,SecondOption_S,Max_Entries);
                Scale=Current_Spectra->GetMaximum();
            }


             TH1F *New_Current_Spectra=new TH1F ();
             New_Current_Spectra=g->Substract_Two_Spectra(Old_Spectra,Current_Spectra,Coef);

           TCanvas *c1=new TCanvas();
           c1->cd();
           New_Current_Spectra->Draw();

        }

        if(Current_Matrix!=0x0)
        {
            // cout<<SpecName<<endl;

            if(!Same){
                TCanvas *CMat=new TCanvas("Current_Can_Mat","Current_Can_Mat",800,600);
                CMat->cd();
                Current_Matrix->Draw("COLZ");}
            else{
                gPad->cd();
                Current_Matrix->Draw("COLZ");
                gPad->Modified();
                gPad->Update();
            }
        }

        if(Current_Spectra!=0x0)
        {
            // cout<<SpecName<<endl;

            if(!Same){
                TCanvas *Ctest=new TCanvas("Current_Can","Current_Can",800,600);
                Ctest->cd();
                Current_Spectra->Draw();}
            else{
                gPad->cd();
                Current_Spectra->Draw("SAME");
                gPad->Modified();
                gPad->Update();
            }


            Current_Spectra->SetTitle(ReturnZWithALatex(z,a+A_Offset));
            Current_Spectra->GetXaxis()->SetRangeUser(0,Max_RangeUser);
            Current_Spectra->GetXaxis()->SetTitle("Energy");
            Current_Spectra->GetYaxis()->SetTitle("Counts");


            for(Int_t k=0;k<Maximum_Gamma_Read;k++)
            {
                Max_Position=0;Max_PositionExp=0;

                for(Int_t j=0;j<6;j++)
                {

                    float Position = Current_Spectra->GetBinContent((Int_t)E[k]-3+j);
                    float PositionExp = Current_Spectra->GetBinContent((Int_t)Eexp[k]-3+j);

                    if(Position>=Max_Position){Max_Position=Position;}
                    if(PositionExp>=Max_PositionExp){Max_PositionExp=PositionExp;}

                }
               // if(!(E[k]<Min_Energy_Arrow || E[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_Position,E[k],0,z,a+A_Offset);

               if(!(Eexp[k]<Min_Energy_Arrow || Eexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionExp,Eexp[k],11,z,a+A_Offset);// 11 est l'indice pour la couleur Rose

                //cout<<" Eexp[k] "<<Eexp[k]<<endl;
            }

            for(Int_t i=0;i<Number_Of_Option;i++)
            {


                switch(EMode[i])
                {
                case -1:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {
                        Max_PositionZmoin=0;Max_PositionZmoinExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionZmoin = Current_Spectra->GetBinContent((Int_t)EZmoin[k]-3+j);
                            float PositionZmoinExp = Current_Spectra->GetBinContent((Int_t)EZmoinexp[k]-3+j);
                            if(PositionZmoin>=Max_PositionZmoin){Max_PositionZmoin=PositionZmoin;}
                            if(PositionZmoinExp>=Max_PositionZmoinExp){Max_PositionZmoinExp=PositionZmoinExp;}
                        }
                        if(!(EZmoin[k]<Min_Energy_Arrow || EZmoin[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZmoin,EZmoin[k],2,z,a+A_Offset);
                        if(!(EZmoinexp[k]<Min_Energy_Arrow || EZmoinexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZmoinExp,EZmoinexp[k],13,z,a+A_Offset);
                    }

                    break;
                case 1:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {
                        Max_PositionZplus=0;Max_PositionZplusExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionZplus = Current_Spectra->GetBinContent((Int_t)EZplus[k]-3+j);
                            float PositionZplusExp = Current_Spectra->GetBinContent((Int_t)EZplusexp[k]-3+j);

                            if(PositionZplus>=Max_PositionZplus){Max_PositionZplus=PositionZplus;}
                            if(PositionZplusExp>=Max_PositionZplusExp){Max_PositionZplusExp=PositionZplusExp;}
                        }
                        if(!(EZplus[k]<Min_Energy_Arrow || EZplus[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZplus,EZplus[k],1,z,a+A_Offset);
                        if(!(EZplusexp[k]<Min_Energy_Arrow || EZplusexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZplusExp,EZplusexp[k],12,z,a+A_Offset);
                    }
                    break;
                case 2:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {
                        Max_PositionZplus=0;Max_PositionZplusExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionZplus = Current_Spectra->GetBinContent((Int_t)EZplus[k]-3+j);
                            float PositionZplusExp = Current_Spectra->GetBinContent((Int_t)EZplusexp[k]-3+j);
                            if(PositionZplus>=Max_PositionZplus){Max_PositionZplus=PositionZplus;}
                            if(PositionZplusExp>=Max_PositionZplusExp){Max_PositionZplusExp=PositionZplusExp;}
                        }
                        if(!(EZplus[k]<Min_Energy_Arrow || EZplus[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZplus,EZplus[k],1,z,a+A_Offset);
                        if(!(EZplusexp[k]<Min_Energy_Arrow || EZplusexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZplusExp,EZplusexp[k],12,z,a+A_Offset);

                        Max_PositionZmoin=0;Max_PositionZmoinExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionZmoin = Current_Spectra->GetBinContent((Int_t)EZmoin[k]-3+j);
                            if(PositionZmoin>=Max_PositionZmoin){Max_PositionZmoin=PositionZmoin;}
                            float PositionZmoinExp = Current_Spectra->GetBinContent((Int_t)EZmoinexp[k]-3+j);
                            if(PositionZmoinExp>=Max_PositionZmoinExp){Max_PositionZmoinExp=PositionZmoinExp;}
                        }
                        if(!(EZmoin[k]<Min_Energy_Arrow || EZmoin[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZmoin,EZmoin[k],2,z,a+A_Offset);
                        if(!(EZmoinexp[k]<Min_Energy_Arrow || EZmoinexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionZmoinExp,EZmoinexp[k],13,z,a+A_Offset);

                    }
                    break;

                case 3:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus=0;Max_PositionAplusExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus = Current_Spectra->GetBinContent((Int_t)EAplus[k]-3+j);
                            float PositionAplusExp = Current_Spectra->GetBinContent((Int_t)EAplusexp[k]-3+j);
                            if(PositionAplus>=Max_PositionAplus){Max_PositionAplus=PositionAplus;}
                            if(PositionAplusExp>=Max_PositionAplusExp){Max_PositionAplusExp=PositionAplusExp;}
                        }
                        if(!(EAplus[k]<Min_Energy_Arrow || EAplus[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus,EAplus[k],3,z,a+A_Offset);
                        if(!(EAplusexp[k]<Min_Energy_Arrow || EAplusexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplusExp,EAplusexp[k],14,z,a+A_Offset);

                    }
                    break;
                case 4:

                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin=0;Max_PositionAmoinExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin = Current_Spectra->GetBinContent((Int_t)EAmoin[k]-3+j);
                            float PositionAmoinExp = Current_Spectra->GetBinContent((Int_t)EAmoinexp[k]-3+j);
                            if(PositionAmoin>=Max_PositionAmoin){Max_PositionAmoin=PositionAmoin;}
                            if(PositionAmoinExp>=Max_PositionAmoinExp){Max_PositionAmoinExp=PositionAmoinExp;}
                        }
                        if(!(EAmoin[k]<Min_Energy_Arrow || EAmoin[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin,EAmoin[k],4,z,a+A_Offset);
                        if(!(EAmoinexp[k]<Min_Energy_Arrow || EAmoinexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoinExp,EAmoinexp[k],15,z,a+A_Offset);

                    }

                    break;

                case 5:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus=0;Max_PositionAplusExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus = Current_Spectra->GetBinContent((Int_t)EAplus[k]-3+j);
                            if(PositionAplus>=Max_PositionAplus){Max_PositionAplus=PositionAplus;}
                            float PositionAplusExp = Current_Spectra->GetBinContent((Int_t)EAplusexp[k]-3+j);
                            if(PositionAplusExp>=Max_PositionAplusExp){Max_PositionAplusExp=PositionAplusExp;}
                        }

                        if(!(EAplus[k]<Min_Energy_Arrow || EAplus[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus,EAplus[k],3,z,a+A_Offset);
                        if(!(EAplusexp[k]<Min_Energy_Arrow || EAplusexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplusExp,EAplusexp[k],14,z,a+A_Offset);

                    }

                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin=0;Max_PositionAmoinExp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin = Current_Spectra->GetBinContent((Int_t)EAmoin[k]-3+j);
                            if(PositionAmoin>=Max_PositionAmoin){Max_PositionAmoin=PositionAmoin;}
                            float PositionAmoinExp = Current_Spectra->GetBinContent((Int_t)EAmoinexp[k]-3+j);
                            if(PositionAmoinExp>=Max_PositionAmoinExp){Max_PositionAmoinExp=PositionAmoinExp;}
                        }
                        if(!(EAmoin[k]<Min_Energy_Arrow || EAmoin[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin,EAmoin[k],4,z,a+A_Offset);
                        if(!(EAmoinexp[k]<Min_Energy_Arrow || EAmoinexp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoinExp,EAmoinexp[k],15,z,a+A_Offset);

                    }

                    break;

                case 6:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus3=0;Max_PositionAplus3Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus3 = Current_Spectra->GetBinContent((Int_t)EAplus3[k]-3+j);
                            float PositionAplus3Exp = Current_Spectra->GetBinContent((Int_t)EAplus3exp[k]-3+j);
                            if(PositionAplus3>=Max_PositionAplus3){Max_PositionAplus3=PositionAplus3;}
                            if(PositionAplus3Exp>=Max_PositionAplus3Exp){Max_PositionAplus3Exp=PositionAplus3Exp;}
                        }

                        if(!(EAplus3[k]<Min_Energy_Arrow || EAplus3[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus3,EAplus3[k],5,z,a+A_Offset);
                        if(!(EAplus3exp[k]<Min_Energy_Arrow || EAplus3exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus3Exp,EAplus3exp[k],16,z,a+A_Offset);

                    }
                break;
                case 7:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin3=0; Max_PositionAmoin3Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin3 = Current_Spectra->GetBinContent((Int_t)EAmoin3[k]-3+j);
                            float PositionAmoin3Exp = Current_Spectra->GetBinContent((Int_t)EAmoin3exp[k]-3+j);

                            if(PositionAmoin3>=Max_PositionAmoin3){Max_PositionAmoin3=PositionAmoin3;}
                            if(PositionAmoin3Exp>=Max_PositionAmoin3Exp){Max_PositionAmoin3Exp=PositionAmoin3Exp;}
                        }
                        if(!(EAmoin3[k]<Min_Energy_Arrow || EAmoin3[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin3,EAmoin3[k],6,z,a+A_Offset);
                        if(!(EAmoin3exp[k]<Min_Energy_Arrow || EAmoin3exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin3Exp,EAmoin3exp[k],17,z,a+A_Offset);

                    }

                    break;
                case 8:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus3=0;Max_PositionAplus3Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus3 = Current_Spectra->GetBinContent((Int_t)EAplus3[k]-3+j);
                            if(PositionAplus3>=Max_PositionAplus3){Max_PositionAplus3=PositionAplus3;}
                            float PositionAplus3Exp = Current_Spectra->GetBinContent((Int_t)EAplus3exp[k]-3+j);
                            if(PositionAplus3Exp>=Max_PositionAplus3Exp){Max_PositionAplus3Exp=PositionAplus3Exp;}
                        }
                        if(!(EAplus3[k]<Min_Energy_Arrow || EAplus3[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus3,EAplus3[k],5,z,a+A_Offset);
                        if(!(EAplus3exp[k]<Min_Energy_Arrow || EAplus3exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus3Exp,EAplus3exp[k],16,z,a+A_Offset);

                    }
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin3=0; Max_PositionAmoin3Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin3 = Current_Spectra->GetBinContent((Int_t)EAmoin3[k]-3+j);
                            if(PositionAmoin3>=Max_PositionAmoin3){Max_PositionAmoin3=PositionAmoin3;}
                            float PositionAmoin3Exp = Current_Spectra->GetBinContent((Int_t)EAmoin3exp[k]-3+j);
                            if(PositionAmoin3Exp>=Max_PositionAmoin3Exp){Max_PositionAmoin3Exp=PositionAmoin3Exp;}
                        }
                        if(!(EAmoin3[k]<Min_Energy_Arrow || EAmoin3[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin3,EAmoin3[k],6,z,a+A_Offset);
                        if(!(EAmoin3exp[k]<Min_Energy_Arrow || EAmoin3exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin3Exp,EAmoin3exp[k],17,z,a+A_Offset);

                    }
                    break;
                case 9:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus2=0;Max_PositionAplus2Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus2 = Current_Spectra->GetBinContent((Int_t)EAplus2[k]-3+j);
                            float PositionAplus2Exp = Current_Spectra->GetBinContent((Int_t)EAplus2exp[k]-3+j);
                            if(PositionAplus2>=Max_PositionAplus2){Max_PositionAplus2=PositionAplus2;}
                            if(PositionAplus2Exp>=Max_PositionAplus2Exp){Max_PositionAplus2Exp=PositionAplus2Exp;}
                        }
                        if(!(EAplus2[k]<Min_Energy_Arrow || EAplus2[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus2,EAplus2[k],7,z,a+A_Offset);
                        if(!(EAplus2exp[k]<Min_Energy_Arrow || EAplus2exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus2Exp,EAplus2exp[k],18,z,a+A_Offset);

                    }
                    break;
                case 10:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin2=0;Max_PositionAmoin2Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin2 = Current_Spectra->GetBinContent((Int_t)EAmoin2[k]-3+j);
                            float PositionAmoin2Exp = Current_Spectra->GetBinContent((Int_t)EAmoin2exp[k]-3+j);
                            if(PositionAmoin2>=Max_PositionAmoin2){Max_PositionAmoin2=PositionAmoin2;}
                            if(PositionAmoin2Exp>=Max_PositionAmoin2Exp){Max_PositionAmoin2Exp=PositionAmoin2Exp;}
                        }
                        if(!(EAmoin2[k]<Min_Energy_Arrow || EAmoin2[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin2,EAmoin2[k],8,z,a+A_Offset);
                        if(!(EAmoin2exp[k]<Min_Energy_Arrow || EAmoin2exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin2Exp,EAmoin2exp[k],19,z,a+A_Offset);

                    }

                    break;

                case 11:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus2=0;Max_PositionAplus2Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus2 = Current_Spectra->GetBinContent((Int_t)EAplus2[k]-3+j);
                            if(PositionAplus2>=Max_PositionAplus2){Max_PositionAplus2=PositionAplus2;}

                            float PositionAplus2Exp = Current_Spectra->GetBinContent((Int_t)EAplus2exp[k]-3+j);
                            if(PositionAplus2Exp>=Max_PositionAplus2Exp){Max_PositionAplus2Exp=PositionAplus2Exp;}
                        }
                        if(!(EAplus2[k]<Min_Energy_Arrow || EAplus2[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus2,EAplus2[k],7,z,a+A_Offset);
                        if(!(EAplus2exp[k]<Min_Energy_Arrow || EAplus2exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus2Exp,EAplus2exp[k],18,z,a+A_Offset);

                    }
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin2=0;Max_PositionAmoin2Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin2 = Current_Spectra->GetBinContent((Int_t)EAmoin2[k]-3+j);
                            if(PositionAmoin2>=Max_PositionAmoin2){Max_PositionAmoin2=PositionAmoin2;}
                            float PositionAmoin2Exp = Current_Spectra->GetBinContent((Int_t)EAmoin2exp[k]-3+j);
                            if(PositionAmoin2Exp>=Max_PositionAmoin2Exp){Max_PositionAmoin2Exp=PositionAmoin2Exp;}
                        }
                        if(!(EAmoin2[k]<Min_Energy_Arrow || EAmoin2[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin2,EAmoin2[k],8,z,a+A_Offset);
                        if(!(EAmoin2exp[k]<Min_Energy_Arrow || EAmoin2exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin2Exp,EAmoin2exp[k],19,z,a+A_Offset);

                    }
                    break;
                case 12:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus4=0;Max_PositionAplus4Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus4 = Current_Spectra->GetBinContent((Int_t)EAplus4[k]-3+j);
                            if(PositionAplus4>=Max_PositionAplus4){Max_PositionAplus4=PositionAplus4;}
                            float PositionAplus4Exp = Current_Spectra->GetBinContent((Int_t)EAplus4exp[k]-3+j);
                            if(PositionAplus4Exp>=Max_PositionAplus4Exp){Max_PositionAplus4Exp=PositionAplus4Exp;}
                        }
                        if(!(EAplus4[k]<Min_Energy_Arrow || EAplus4[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus4,EAplus4[k],9,z,a+A_Offset);
                        if(!(EAplus4exp[k]<Min_Energy_Arrow || EAplus4exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus4Exp,EAplus4exp[k],20,z,a+A_Offset);

                    }
                    break;
                case 13:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin4=0;Max_PositionAmoin4Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin4 = Current_Spectra->GetBinContent((Int_t)EAmoin4[k]-3+j);
                            if(PositionAmoin4>=Max_PositionAmoin4){Max_PositionAmoin4=PositionAmoin4;}
                            float PositionAmoin4Exp = Current_Spectra->GetBinContent((Int_t)EAmoin4exp[k]-3+j);
                            if(PositionAmoin4Exp>=Max_PositionAmoin4Exp){Max_PositionAmoin4Exp=PositionAmoin4Exp;}
                        }
                        if(!(EAmoin4[k]<Min_Energy_Arrow || EAmoin4[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin4,EAmoin4[k],10,z,a+A_Offset);
                        if(!(EAmoin4exp[k]<Min_Energy_Arrow || EAmoin4exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin4Exp,EAmoin4exp[k],21,z,a+A_Offset);

                    }

                    break;

                case 14:
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAplus4=0;Max_PositionAplus4Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAplus4 = Current_Spectra->GetBinContent((Int_t)EAplus4[k]-3+j);
                            if(PositionAplus4>=Max_PositionAplus4){Max_PositionAplus4=PositionAplus4;}
                            float PositionAplus4Exp = Current_Spectra->GetBinContent((Int_t)EAplus4exp[k]-3+j);
                            if(PositionAplus4Exp>=Max_PositionAplus4Exp){Max_PositionAplus4Exp=PositionAplus4Exp;}

                        }
                        if(!(EAplus4[k]<Min_Energy_Arrow || EAplus4[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus4,EAplus4[k],9,z,a+A_Offset);
                        if(!(EAplus4exp[k]<Min_Energy_Arrow || EAplus4exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAplus4Exp,EAplus4exp[k],20,z,a+A_Offset);
                    }
                    for(Int_t k=0;k<Maximum_Gamma_Read;k++)
                    {

                        Max_PositionAmoin4=0;Max_PositionAmoin4Exp=0;

                        for(Int_t j=0;j<6;j++)
                        {
                            float PositionAmoin4 = Current_Spectra->GetBinContent((Int_t)EAmoin4[k]-3+j);
                            if(PositionAmoin4>=Max_PositionAmoin4){Max_PositionAmoin4=PositionAmoin4;}
                            float PositionAmoin4Exp = Current_Spectra->GetBinContent((Int_t)EAmoin4exp[k]-3+j);
                            if(PositionAmoin4Exp>=Max_PositionAmoin4Exp){Max_PositionAmoin4Exp=PositionAmoin4Exp;}

                        }
                        if(!(EAmoin4[k]<Min_Energy_Arrow || EAmoin4[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin4,EAmoin4[k],10,z,a+A_Offset);
                        if(!(EAmoin4exp[k]<Min_Energy_Arrow || EAmoin4exp[k]>Max_RangeUser))Cube->Draw_Arrow(Scale,Max_PositionAmoin4Exp,EAmoin4exp[k],21,z,a+A_Offset);

                    }
                    break;

                }//end Switch option
            }//End for option

        }
        else
        {
            cout<<endl;WarningMessage("***********************************************************************************");
            WarningMessage("**NOT ENOUGH STAT FOR THIS NUCLEUS OR MATRIX MODE ACTIVATED OR AGATA ONLY *****");
            WarningMessage("************************************************************************************");cout<<endl;
        }



    }
    gPad->SetCrosshair();

    free(E);
    free(EZmoin);
    free(EZplus);
    free(EAplus);
    free(EAmoin);
    free(EAplus3);
    free(EAmoin3);
    free(EAplus2);
    free(EAmoin2);
    free(EAplus4);
    free(EAmoin4);
}

void Cube_Player::Redraw()
{
    Chart_Bidim->Draw("colz");
    cout<<endl; InfoMessage(" Redrawing CHART ");cout<<endl;
}

void Cube_Player::Display_All_Nuclei()
{
    Tl.SetTextFont(43); Tl.SetTextSize(13);

    for(Int_t i=0;i<=18;i++)
    {
        for(Int_t j=-5;j<=36;j++)
        {
            if((Z_Offset+i)<=(0.4*(A_Offset+j)+4) && (i+Z_Offset)>=(0.4*(A_Offset+j)-3))Tl.DrawLatex(A_Offset+j,Z_Offset+i,ReturnZWithALatex(i,A_Offset+j));

        }
    }

}

Bool_t Cube_Player::OpenContextMenu(const char* method, TObject* obj, const char *alt_method_name)
{
    // Open context menu for given method of object *obj.
    // By default title of menu is 'obj->ClassName()::method'
    // You can give an alternative method name in 'alt_method_name'
    // Returns kFALSE if the given method is not defined for the class of object in question.
    //
    // WARNING: even if this method returns kTRUE, this is no guarantee that the method
    // has indeed been executed. The user may have pressed the 'Cancel' button...

    TMethod* m = obj->IsA()->GetMethodAllAny(method);
    if(!m)
    {
        obj->Warning("OpenContextMenu","%s is not a method of %s",method,obj->ClassName());
        return kFALSE;
    }
    TString Method=alt_method_name;
    if(Method=="") Method=method;
    TContextMenu * cm = new TContextMenu(Method, Form("%s::%s",obj->ClassName(),Method.Data()));
    cm->Action(obj,m);
    delete cm;
    return kTRUE;
}


void Cube_Player::DrawSpectra(const char * Nuclei_Name,const char* Option,const char * OPTION2,Float_t StrengthMin,Bool_t Same,const char* Quality_Factor,const char *Gate,Float_t Gate_W,Int_t Max_Entries)
{

    InfoMessage(Nuclei_Name);
    Int_t Zz=0;
    TString B=Nuclei_Name;
    TString A=(B(0,2));
    TString Rest =B(2,2);

    if(A(0,1)=="1"){ A=(B(0,3));Rest =B(3,2);}

    Int_t NNucleiToRead = sizeof(Z)/sizeof(TString);

    for(Int_t i=0;i<=NNucleiToRead;i++)
    {
        if(Z[i]==Rest)break;
        Zz=i;
    }
    Zz+=1;
    Int_t AInt=A.Atoi();

    if(Zz!=NNucleiToRead+1){Plot_Current_Spectra(Zz,AInt-A_Offset,Option,OPTION2,StrengthMin,Same,Quality_Factor,Gate,Gate_W,Max_Entries);}else{ErrorMessage("Wrong name of Nuclei");}

}


void Cube_Player::Draw_Serie_Isotopique(Int_t z,Int_t AMin,Int_t AMax)
{

    Float_t tMax=0,Old_tMax=0;
    Int_t Aref=0;
    Int_t Counter=0;
    TLegend *leg = new TLegend(0.8,0.5,0.9,0.9);
    TString Leg_Name="Isotopes ";Leg_Name+=Z[z-Z_Offset];
    leg->SetHeader(Leg_Name);

    if(!gSystem->IsFileInIncludePath(FileName))
    {


        ErrorMessage("*************************************");
        ErrorMessage("***Spectra are not in include PATH***");
        ErrorMessage("*************************************");

    }else
    {
        new TCanvas();
        fOutputSpectra=TFile::Open(FileName);

        TList *list = (TList*)fOutputSpectra->Get("ListOfHists");

        TString FolderName ="All_ICE/GammaSpectra/";
        TList *list2 = (TList*)list->FindObject("All_ICE");
        TList *list3 = (TList*)list2->FindObject("GammaSpectra");
        TList *list4 = (TList*)list3->FindObject( Form("Z%d",z));


        for(Int_t i=AMin;i<=AMax;i++)
        {

            TString SpecName="Z";SpecName+=z;SpecName+="_A";SpecName+=i;

            Current_Spectra = (TH1F*)list4->FindObject(SpecName);

            if(Current_Spectra!=0x0)
            {
                Current_Spectra->SetLineColor(Color[i-AMin]);


                tMax=Current_Spectra->GetMaximum();
                if(tMax>=Old_tMax){Old_tMax=tMax;Aref=(Int_t)i;}
               // cout<<"Old_tMax"<<Old_tMax<<endl;
            }
        }

        for(Int_t i=AMin-1;i<=AMax;i++)
        {

            if(i==Aref)continue;

            TString SpecName="Z";SpecName+=z;SpecName+="_A";SpecName+=i;

            TString SpecNameFirst="Z";SpecNameFirst+=z;SpecNameFirst+="_A";SpecNameFirst+=Aref;

            if(Counter==0)
            {
                Current_Spectra = (TH1F*)list4->FindObject(SpecNameFirst);
                Current_Spectra->SetLineColor(kBlack);Current_Spectra->SetTitle(Leg_Name);
            }else
            {

                Current_Spectra = (TH1F*)list4->FindObject(SpecName);
                if(Current_Spectra!=0x0){Current_Spectra->SetLineColor(Color[i-AMin]);Current_Spectra->SetTitle(Leg_Name);}
            }

            gStyle->SetOptStat(0);


            if(Current_Spectra!=0x0)
            {

                if(Counter==0)
                {
                    Current_Spectra->Draw();leg->AddEntry(Current_Spectra,ReturnZWithALatex(z-Z_Offset,Aref),"l");
                    Current_Spectra->GetXaxis()->SetTitle("Energy");
                    Current_Spectra->GetYaxis()->SetTitle("Counts");
                }else
                {
                    Current_Spectra->Draw("SAME");
                    leg->AddEntry(Current_Spectra,ReturnZWithALatex(z-Z_Offset,i),"l");
                    Current_Spectra->GetXaxis()->SetTitle("Energy");
                    Current_Spectra->GetYaxis()->SetTitle("Counts");
                }

            }
            else
            {


                cout<<endl;ErrorMessage("**************************************");
                ErrorMessage("***NOT ENOUGH STAT FOR THIS NUCLEUS***");
                ErrorMessage("**************************************");cout<<endl;
            }
            cout<<"Nucleus : "<<i<<endl;
            if(i>=120)break;
            leg->Draw("SAME");
            Counter++;

        }


        Counter=0;
    }
   gPad->SetCrosshair();
}

Bool_t Cube_Player::IsLsDraw(const char *Option)
{

    TString Possibilities[8]={"l","L","lV","LV","Lv","Vl","VL","vL"};
    TString A=Option;

    for(Int_t i=0;i<(Int_t)(sizeof(Possibilities)/sizeof(TString));i++)
    {

        if(Option==Possibilities[i]|| A(0,1)==Possibilities[i] ||A(1,1)==Possibilities[i]){return 1;}

    }

}
Bool_t Cube_Player::IsVMode(const char *Option)
{

    TString Possibilities[8]={"v","V","lV","LV","Lv","Vl","VL","vL"};
    TString A=Option;

    for(Int_t i=0;i<(Int_t)(sizeof(Possibilities)/sizeof(TString));i++)
    {

        if(Option==Possibilities[i] || A(0,1)==Possibilities[i] || A(1,1)==Possibilities[i] ){return 1;}

    }
}

signed short int *Cube_Player::WhichMode(const char *Option)
{

    signed short int *WhichOption =new signed short int[Number_Of_Option];
    WhichOption[0]=0;  WhichOption[1]=0;  WhichOption[2]=0; WhichOption[3]=0;WhichOption[4]=0;
    TString Delimiter=" ";
    TString CompareOption=TString(Option);

    TObjArray *d = CompareOption.Tokenize(TString(Delimiter));

    TObjString *T =(TObjString*)d->At(0);

    const char *True_Option =(const char*)T->GetString();

    //cout<<"TrueOption1 "<<True_Option<<endl;
    WhichOption[0]=Cube->IsZMode(True_Option)+Cube->IsAMode(True_Option)+Cube->IsA3Mode(True_Option)+Cube->IsA4Mode(True_Option)+Cube->IsA2Mode(True_Option);

    if(d->GetEntries()>1)
    {
        T=(TObjString*)d->At(1);
        True_Option =(const char*)T->GetString();
        WhichOption[1]=Cube->IsZMode(True_Option)+Cube->IsAMode(True_Option)+Cube->IsA3Mode(True_Option)+Cube->IsA4Mode(True_Option)+Cube->IsA2Mode(True_Option);
    }

    if(d->GetEntries()>2)
    {
        T=(TObjString*)d->At(2);
        True_Option =(const char*)T->GetString();
        WhichOption[2]=Cube->IsZMode(True_Option)+Cube->IsAMode(True_Option)+Cube->IsA3Mode(True_Option)+Cube->IsA4Mode(True_Option)+Cube->IsA2Mode(True_Option);;
    }
    if(d->GetEntries()>3)
    {
        T=(TObjString*)d->At(3);
        True_Option =(const char*)T->GetString();
        WhichOption[3]=Cube->IsZMode(True_Option)+Cube->IsAMode(True_Option)+Cube->IsA3Mode(True_Option)+Cube->IsA4Mode(True_Option)+Cube->IsA2Mode(True_Option);;
    }
    if(d->GetEntries()>4)
    {
        T=(TObjString*)d->At(4);
        True_Option =(const char*)T->GetString();
        WhichOption[4]=Cube->IsZMode(True_Option)+Cube->IsAMode(True_Option)+Cube->IsA3Mode(True_Option)+Cube->IsA4Mode(True_Option)+Cube->IsA2Mode(True_Option);;
    }
    return WhichOption;

}

signed short int Cube_Player::IsZMode(const char *Option)
{

    TString Possibilities[12]={"Z+1","z+1","Z+","z+","Z-1","z-1","Z-","z-","z+-","Z+-","Z-+","z-+"};
    signed short int Resultat =0;
        if(Option==Possibilities[0]||Option==Possibilities[1]||Option==Possibilities[2]||Option==Possibilities[3])Resultat=1;
        if(Option==Possibilities[4]||Option==Possibilities[5]||Option==Possibilities[6]||Option==Possibilities[7])Resultat= -1;
        if(Option==Possibilities[8]||Option==Possibilities[9]||Option==Possibilities[10]||Option==Possibilities[11])Resultat= 2;

        return Resultat;

}

signed short int Cube_Player::IsAMode(const char *Option)
{

    TString Possibilities[12]={"A+1","a+1","A+","a+","A-1","a-1","A-","a-","A+-","a+-","A-+","a-+"};
    signed short int Resultat =0;
        if(Option==Possibilities[0]||Option==Possibilities[1]||Option==Possibilities[2]||Option==Possibilities[3])Resultat=3;
        if(Option==Possibilities[4]||Option==Possibilities[5]||Option==Possibilities[6]||Option==Possibilities[7])Resultat= 4;
        if(Option==Possibilities[8]||Option==Possibilities[9]||Option==Possibilities[10]||Option==Possibilities[11])Resultat= 5;

        return Resultat;

}

signed short int Cube_Player::IsA3Mode(const char *Option)
{

    TString Possibilities[12]={"A+3","a+3","A+++","a+++","A-3","a-3","A---","a---","A+++---","a++++---","A---+++","a---+++"};
    signed short int Resultat =0;
        if(Option==Possibilities[0]||Option==Possibilities[1]||Option==Possibilities[2]||Option==Possibilities[3])Resultat=6;
        if(Option==Possibilities[4]||Option==Possibilities[5]||Option==Possibilities[6]||Option==Possibilities[7])Resultat= 7;
        if(Option==Possibilities[8]||Option==Possibilities[9]||Option==Possibilities[10]||Option==Possibilities[11])Resultat= 8;

        return Resultat;

}
signed short int Cube_Player::IsA4Mode(const char *Option)
{

    TString Possibilities[12]={"A+4","a+4","A++++","a++++","A-4","a-4","A----","a----","A++++----","a++++----","A----++++","a----++++"};
    signed short int Resultat =0;
        if(Option==Possibilities[0]||Option==Possibilities[1]||Option==Possibilities[2]||Option==Possibilities[3])Resultat=12;
        if(Option==Possibilities[4]||Option==Possibilities[5]||Option==Possibilities[6]||Option==Possibilities[7])Resultat= 13;
        if(Option==Possibilities[8]||Option==Possibilities[9]||Option==Possibilities[10]||Option==Possibilities[11])Resultat= 14;

        return Resultat;

}
signed short int Cube_Player::IsA2Mode(const char *Option)
{

    TString Possibilities[12]={"A+2","a+2","A++","a++","A-2","a-2","A--","a--","A++--","a++--","A--++","a--++"};
    signed short int Resultat =0;
        if(Option==Possibilities[0]||Option==Possibilities[1]||Option==Possibilities[2]||Option==Possibilities[3])Resultat=9;
        if(Option==Possibilities[4]||Option==Possibilities[5]||Option==Possibilities[6]||Option==Possibilities[7])Resultat= 10;
        if(Option==Possibilities[8]||Option==Possibilities[9]||Option==Possibilities[10]||Option==Possibilities[11])Resultat= 11;

        return Resultat;

}




void Cube_Player::Draw_Arrow(Float_t Scale,Float_t Max_Position,Float_t Energy,Int_t Color,Int_t Zz, Int_t A)
{
    TArrow *Gamma_Arrow = new TArrow(Energy,(Max_Position + Scale/100.) ,Energy,(Max_Position +Scale/10.),0.005,"<|");
    Gamma_Arrow->SetAngle(40);
    Gamma_Arrow->SetLineColor(Gamma_Arrow_Color[Color]);
    Gamma_Arrow->SetFillColor(Gamma_Arrow_Color[Color]);
    Gamma_Arrow->SetLineWidth(1);
    Gamma_Arrow->Draw();

    TLatex Tl;
    TString Energy_Name=Form("%0.1f",Energy);
    //Par défaut la couleur rouge est le noyau sélectionné
    //couleur 1 a Z+1
    //Couleur 2 à Z-1
    switch(Color)
    {
    case 1:
        Energy_Name+="   ";Energy_Name+=A;Energy_Name+=Z[Zz+1];break;
    case 2:
        Energy_Name+="   ";Energy_Name+=A;Energy_Name+=Z[Zz+-1];break;
    case 3:
        Energy_Name+="   "; Energy_Name+=A+1;Energy_Name+=Z[Zz];break;
    case 4:
        Energy_Name+="   "; Energy_Name+=A-1;Energy_Name+=Z[Zz];break;
    case 5:
        Energy_Name+="   "; Energy_Name+=A+3;Energy_Name+=Z[Zz];break;
    case 6:
        Energy_Name+="   "; Energy_Name+=A-3;Energy_Name+=Z[Zz];break;
    case 7:
        Energy_Name+="   "; Energy_Name+=A+2;Energy_Name+=Z[Zz];break;
    case 8:
        Energy_Name+="   "; Energy_Name+=A-2;Energy_Name+=Z[Zz];break;
    case 9:
        Energy_Name+="   "; Energy_Name+=A+4;Energy_Name+=Z[Zz];break;
    case 10:
        Energy_Name+="   "; Energy_Name+=A-4;Energy_Name+=Z[Zz];break;
    case 11:
        Energy_Name+="   "; Energy_Name +=A;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 12 :
        Energy_Name+="   "; Energy_Name +=A;Energy_Name+=Z[Zz+1];Energy_Name+=" From E680";break;
    case 13 :
        Energy_Name+="   "; Energy_Name +=A;Energy_Name+=Z[Zz-1];Energy_Name+=" From E680";break;
    case 14 :
        Energy_Name+="   "; Energy_Name +=A+1;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 15 :
        Energy_Name+="   "; Energy_Name +=A-1;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 16 :
        Energy_Name+="   "; Energy_Name +=A+3;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 17:
        Energy_Name+="   "; Energy_Name +=A-3;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 18:
        Energy_Name+="   "; Energy_Name +=A+2;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 19:
        Energy_Name+="   "; Energy_Name +=A-2;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 20 :
        Energy_Name+="   "; Energy_Name +=A+4;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;
    case 21:
        Energy_Name+="   "; Energy_Name +=A-4;Energy_Name+=Z[Zz];Energy_Name+=" From E680";break;

    }
    Tl.SetTextAngle(90);
    Tl.SetTextSize(0.012);
    Tl.SetTextColor(Gamma_Arrow_Color[Color]);
    Tl.DrawLatex(Energy,(Max_Position +1.2*Scale/10.),Energy_Name);

}


void Cube_Player::Plot_Main_Wastes(Int_t MCurrent)
{

    Double_t Parameters[3] = {1.,3.,0.5};

    TF1 *MyFunc = new TF1("MyFunc",GausFunc,1.5,4,3);
    MyFunc->SetParNames("Max","Mean","Sigma");
    MyFunc->SetParameters(Parameters[0],Parameters[1],Parameters[2]);

    TH2F * Q_mQ = new TH2F("QvsM_Q","QvsM_Q",nBinX,BinXmin,BinXmax,nBinY,BinYmin,BinYmax);
    Q_mQ->GetYaxis()->SetTitle("Q");
    Q_mQ->GetXaxis()->SetTitle("M/Q");
    TString Name_Polluants="";Name_Polluants+="M = ";Name_Polluants+=MCurrent;
    TH1F *DistPolluants =new TH1F (Name_Polluants,Name_Polluants,2*MaxADist,MCurrent-MaxADist,MCurrent+MaxADist);

    TLatex Tl;
    Float_t Center=0;

    BinPerUnitX=nBinX/(BinXmax-BinXmin);
    BinPerUnitY=nBinY/(BinYmax-BinYmin);

    Metrik=BinPerUnitX/BinPerUnitY;

    for (Int_t M =Mmin; M<=Mmax; M++)
    {
        for (Int_t j =Qmin; j<=Qmax; j++)
        {

            Q_mQ->Fill((Float_t)M/j,j);
        }
    }

    TCanvas *C_Polluants =new TCanvas ("C_Polluants","C_Polluants",1400,1000);
    C_Polluants->Divide(1,2);
    C_Polluants->cd(1);
    Q_mQ->SetStats(0);

    Q_mQ->Draw("Colz");

    for (Int_t M =Mmin; M<=Mmax; M++)
    {
        for (Int_t j =Qmin; j<=Qmax; j++)
        {
            if(((Float_t)M/j)>4.0)continue;
            Center=Q_mQ->GetXaxis()->GetBinCenter((Int_t)(((((Float_t)M/j)-BinXmin)/(BinXmax-BinXmin))*nBinX));
            TString Mm="";
            Mm+=M;
            Tl.SetTextColor(kBlack);
            Tl.SetTextSize(0.020);
            Tl.DrawLatex(Center,j+0.3,Mm);

        }
    }
    Float_t Mref=MCurrent;


    for (Int_t h =Qmin; h<=Qmax; h++)// boucle sur tous les Q possible d'un M donné
    {

        for (Int_t M =Mmin; M<=Mmax; M++)
        {
            for (Int_t j =Qmin; j<=Qmax; j++)
            {

                Float_t M_Q= (Float_t)M/j;
                Float_t Dist_Current_Nuclei=TMath::Sqrt(((M_Q-(Mref/h))*Metrik)*((M_Q-(Mref/h))*Metrik) + ((j-h))*((j-h)));

                if(TMath::Abs(Dist_Current_Nuclei)<=RayonConfiance)
                {
                    if(M!=MCurrent)DistPolluants->Fill(M,(1/TMath::Abs(Dist_Current_Nuclei))/**MyFunc->Eval(M_Q)*/);
                }

            }
        }


    }

    C_Polluants->cd(2);
    DistPolluants->GetYaxis()->SetTitle("Polluting power");
    DistPolluants->GetXaxis()->SetTitle("A");
    DistPolluants->SetStats(0);
    DistPolluants->Draw();

    Float_t testdist=TMath::Sqrt(((2.95982-3)*Metrik)*((2.95982-3)*Metrik) + ((28-27))*((28-27)));

    Float_t testdist2=TMath::Sqrt(((2.95982-3)*Metrik)*((2.95982-3)*Metrik)) ;

}

void Cube_Player::Search_Coinc()
{

    system("firefox -new-window http://www.nndc.bnl.gov/nudat2/indx_adopted.jsp&");
}

void Cube_Player::Add_Gamma(const char* Name_Nuclei)
{

    Gw::Build_LS *LS =new Gw::Build_LS("","",Exp_Directory);

    LS->Plot_LS(Name_Nuclei);

}

void Cube_Player::Estimate_BackGround(Int_t Z,Int_t A,Float_t Energy_Ref,Float_t Height,Color_t Color)
{

    const int taille_FWHM=6;

    //FWHM ancienne parametrisation
    Double_t x[taille_FWHM]  = {0,125.,158.,350.77,495.9,737};
    Double_t y[taille_FWHM]  = {4.17,4.165,4.067,4.613,5.47,6.9387};
    Double_t ex[taille_FWHM] = {0.05,0.05,0.05,0.05,0.05,0.05};

    //END FWHM ancienne parametrisation

    //FWHM nouvelle parametrisation

//    Double_t x[2]  = {59.6,1332.3};
//    Double_t y[2]  = {1.2,2.1};
//    Double_t ex[6] = {0.05,0.05};

    //End FWHM nouvelle parametrisation

    TGraphErrors *Graph_FWHM= new TGraphErrors(taille_FWHM,x,y,ex,ex);
    Graph_FWHM->SetTitle("TGraphErrors FWHM AGATA");
    Graph_FWHM->SetMarkerColor(4);
    Graph_FWHM->SetMarkerStyle(21);


    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);

    Nuclei_Lev->Get_Raw_Intensities(Z,A,Energy_Ref,Height,Graph_FWHM,Color);


}


void Cube_Player::Draw_Gated_Intensities(const char * Name_Nuclei,Float_t Energy_Ref,Float_t Height,Color_t Color,Float_t Gate,Bool_t SAME)
{

    const int taille_FWHM=6;

    //FWHM ancienne parametrisation
    Double_t x[taille_FWHM]  = {0,125.,158.,350.77,495.9,737};
    Double_t y[taille_FWHM]  = {4.17,4.165,4.067,4.613,5.47,6.9387};
    Double_t ex[taille_FWHM] = {0.05,0.05,0.05,0.05,0.05,0.05};

    //END FWHM ancienne parametrisation

    TGraphErrors *Graph_FWHM= new TGraphErrors(taille_FWHM,x,y,ex,ex);
    Graph_FWHM->SetTitle("TGraphErrors FWHM AGATA");
    Graph_FWHM->SetMarkerColor(4);
    Graph_FWHM->SetMarkerStyle(21);


    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);


   Nuclei_Lev->Get_Multi_Gated_Intensities(Name_Nuclei,Gate,Energy_Ref,Height,Graph_FWHM,Color,SAME);

}


void Cube_Player::Draw_Double_Gated_Intensities(const char * Name_Nuclei,Float_t Energy_Ref,Float_t Height,Color_t Color,Float_t Gate1,Float_t Gate2,Bool_t SAME)
{

    const int taille_FWHM=6;

    //FWHM ancienne parametrisation
    Double_t x[taille_FWHM]  = {0,125.,158.,350.77,495.9,737};
    Double_t y[taille_FWHM]  = {4.17,4.165,4.067,4.613,5.47,6.9387};
    Double_t ex[taille_FWHM] = {0.05,0.05,0.05,0.05,0.05,0.05};

    //END FWHM ancienne parametrisation

    TGraphErrors *Graph_FWHM= new TGraphErrors(taille_FWHM,x,y,ex,ex);
    Graph_FWHM->SetTitle("TGraphErrors FWHM AGATA");
    Graph_FWHM->SetMarkerColor(4);
    Graph_FWHM->SetMarkerStyle(21);


    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);


   Nuclei_Lev->Get_Double_Gated_Intensities(Name_Nuclei,Gate1,Gate2,Energy_Ref,Height,Color,Graph_FWHM,SAME);

}






ClassImp(Cube_Player);
