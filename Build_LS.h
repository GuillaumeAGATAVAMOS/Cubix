#include <iostream>
#include <utility>
#include "TH2.h"
#include "TString.h"
#include "TNamed.h"
#include "TObject.h"
#include "Cube_Player.h"
#include "GLSPlayer.h"
#include "LevelScheme.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "ENSDF_DATAe680.h"
#include "GLSPlayer.h"
#include "TMatrixD.h"
#include "SpreadIntensityMatrix.h"



#ifndef BUILD_LS_H
#define BUILD_LS_H



namespace Gw {
    class Build_LS : public TNamed
    {

    private:

        typedef std::vector<Float_t> Float_vec_t;
        typedef std::vector<Int_t> Int_vec_t;
        TCanvas * C_Player;
        TF1* fGaus;
        TString Directory;
        TString Raw_Directory;
        Float_t *ETab;
        Float_t *EStrength;
        Float_t *Eexp;
        Float_t *Sexp;
        Float_t *E;
        Float_t *S;
        Float_t Format_Intensities;
        Float_t Delta_Gate;
        Int_t Counter;
        Int_vec_t Coinc_Gammas;
        Int_vec_t Coinc_Gammas_Math;
        Int_vec_t Index_Sorted_Math;
        Float_vec_t Gate_Tab;
        Float_vec_t Gate_Tab_Math;
        Int_vec_t Coinc_Gammas_Double_Gated_Above_Gate2;
        Int_vec_t Coinc_Gammas_Double_Gated_Above_Gate1;
        Int_vec_t Coinc_Gammas_Double_Gated_Below_Gate2;
        Int_vec_t Index_Coinc_Double_Gated_Above_Gate2;
        Float_vec_t Strength_Above_Gate2_Double_Gated;
        Float_vec_t Strength_Above_Gate1_Double_Gated;
        Float_vec_t Energy_Above_Gate2_Double_Gated;
        Float_vec_t Energy_Below_Gate2_Double_Gated;
        Float_vec_t Energy_Above_Gate1_Double_Gated;
        Float_vec_t Good_Energy_Double_Gated;
        Float_vec_t Good_Strength_Double_Gated;
        Float_vec_t Corrected_Strength_Below_Gate2_Double_Gated;
        Float_vec_t Corrected_Strength_Below_Gate1_Double_Gated;
        Int_vec_t Coinc_Gammas_Double_Gated_Below_Gate1;
        Int_vec_t Index_Coinc_Double_Gated_Below_Gate1;
        Float_vec_t Energy_Below_Gate1_Double_Gated;
        Float_vec_t All_E_DG;
        Float_vec_t All_S_DG;
        Float_vec_t Corrected_Strength;
        TF1* All_Func_Theo;
        TF1* All_Raw_Func_Theo;
        TF1* All_DG_Func_Theo;


    protected:

        Gw::LevelScheme *lev;
        Gw::GLSPlayer *Plev;
        Gw::Build_LS *Nuclei_Lev;
        Gw::ENSDF_DATAe680 *Nuclei_levENSDF;

        TF1* GausFunc(Float_t Ampl,Float_t mean, Float_t sigma);
        void Draw_Arrow(Float_t Scale,Float_t Max_Position,Float_t Energy,Float_t Strength,Int_t Color);


    public:

        Build_LS(const char* name, const char *title,const char* Exp_Directory);
        Build_LS();
        virtual ~Build_LS();

        void Plot_LS(TString Name_Nuclei);

        Float_t *GetGammas(Int_t Zz, Int_t A);
        Float_t *GetStrength(Int_t Zzz, Int_t A);

        TObjArray *Get_Raw_Intensities(Int_t Zzz=34, Int_t A=88,Float_t Energy_Ref=588.,Float_t Height=1878,TGraphErrors *Graph_FWHM=0x0,Color_t Color=kPink+10);
        TObjArray *Get_Multi_Gated_Intensities(TString Name_Nuclei,Float_t Gate,Float_t Energy_Ref,Float_t Height,TGraphErrors *Graph_FWHM,Color_t Color,Bool_t SAME);
        TObjArray *Get_Double_Gated_Intensities(TString Name_Nuclei,Float_t Gate1,Float_t Gate2,Float_t Energy_Ref,Float_t Height,Color_t Color,TGraphErrors *Graph_FWHM,Bool_t SAME);
        TMatrixD Get_Gated_Intensities_From_TS(TString Name_Nuclei="88Se",Float_t Gate=300.,Float_t Energy_Ref=588.,Float_t Height =100.);
        TMatrixD Get_Double_Gated_Intensities_From_TS(TString Name_Nuclei="88Se",Float_t Gate1=961,Float_t Gate2=588,Float_t Energy_Ref=588.,Float_t Height =100.);
        TMatrixD Get_Triple_Gated_Intensities_From_TS(TString Name_Nuclei,Float_t Gate1,Float_t Gate2,Float_t Gate3,Float_t Energy_Ref,Float_t Height);


        ClassDef(Build_LS,1)

            };
}

#endif // BUILD_LS_H
