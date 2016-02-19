#include <stdio.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <utility>
#include <stdlib.h>
#include "TTree.h"
#include "TMath.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TH1F.h"


#ifndef SPECTRUM_LOADER_H
#define SPECTRUM_LOADER_H


namespace Gw {

    class Spectrum_Loader: public TNamed
    {


    private:

        typedef std::vector<Float_t> Float_vec_t;

        TDirectoryFile *fTree;
        TTree *fCurrentTree;
        TH1F *ffOutputSpectra;//current spectra
        TH1F *f_2OutputSpectra;//current second spectra to compare
        TH2F *f2OutputSpectra;//Current matrix
        Float_t fGate_Width;
        TObjArray *Array_Of_Spectra;
        TFile *fInputRawSpectra; //file of raw spectra for a given Mass
        TH1F *fCurrentRawSpectra;// current raw spectra for a given mass
        TH1F *fCurrent_Substracted_Spectra;


    protected:


    public:
        Spectrum_Loader();
        Spectrum_Loader(const char* name, const char *title);
        virtual ~Spectrum_Loader();

        Int_t Which_Gate_Mode(const char*Gate);

        void Change_Gate_Width(Float_t Gate_Width);
        Float_t Get_Gate_Width();

        TH1F *Plot_Isotope(Int_t Z=34,Int_t M=88,const char *ID_R="0.5",ULong64_t Max_Read_Entries=10000000000);
        TH1F *Plot_Gated_Spectra(Int_t Zz=34,Int_t M=88,const char *ID_R="0.5",const char *Gate="N 0",Float_t Gate_W=2.0,ULong64_t Max_Read_Entries=10000000000);
        TH1F* Plot_Isotope_Gated(Int_t Zz=34,Int_t M=88,const char *ID_R="0.5",const char *Gate="N 0",Float_t Gate_W=2.0,ULong64_t Max_Read_Entries=10000000000);
        TH1F *Plot_Double_Gated_Spectra(Int_t Zz=34,Int_t M=88,const char *ID_R="0.5",const char *Gate="N 0",Float_t Gate_W=2.0,ULong64_t Max_Read_Entries=10000000000);
        TH2F *Plot_E_Z(Int_t Zz,Int_t M,const char *ID_R,Float_t Z_Min,Float_t Z_Max,ULong64_t Max_Read_Entries);
        TH2F *Plot_E_M(Int_t Zz,Int_t M,const char *ID_R,Float_t M_Min,Float_t M_Max,ULong64_t Max_Read_Entries);
        TH1F *Plot_B_M(Int_t Zz,Int_t M,const char *PID,ULong64_t Max_Read_Entries);
        TH1F *Plot_All_Gated_Spectra(const char *Gate,Float_t Gate_W,ULong64_t Max_Read_Entries);


        TH1F *Substract_Two_Spectra(TH1F* Old_Spectra,TH1F* Current_Spectra,Float_t Coef);

        TObjArray *Search_Polluants(Int_t Zz,Int_t M,const char * ID_R,ULong64_t Max_Read_Entries);

        TH2F *Plot_PID_M(Int_t M_Current,const char *Gate,unsigned int Max_Entries);


        Int_t Is_Special_BackSub_Mode(const char* Sub_Mode);

        TH1F * Load_Or_Create_Raw_Spectra(Int_t M,Int_t SubMode,Int_t Max_Read_Entries);

        TH1F * Create_SpectraWithout_BackGround(TH1F * Raw_Spectra,TH1F * Current_Spectra_to_Substract);

        ClassDef(Spectrum_Loader,1)



            };
}

#endif // SPECTRUM_LOADER_H
