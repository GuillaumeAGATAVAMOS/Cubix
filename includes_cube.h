#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TCanvas.h"
#include "TObject.h"
#include "TSystem.h"
#include "TH2F.h"
#include "TLatex.h"
#include "TContextMenu.h"
#include "TColor.h"
#include "TLegend.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "GSPlayerTUI.h"
#include "TArrow.h"

#include "Cube_Player.h"
#include "ENSDF_DATAe680.h"
#include "Build_LS.h"

using namespace std;
using namespace Gw;


Int_t Qmin=20,Qmax=40;
Int_t Mmin=70,Mmax=140;

Int_t nBinX=500;
Float_t BinXmin=1.5;
Float_t BinXmax=4;

Int_t nBinY=180;
Float_t BinYmin=20;
Float_t BinYmax=42;

Float_t BinPerUnitY=0;
Float_t BinPerUnitX=0;
Float_t Metrik=0;

Float_t RayonConfiance=2;

Int_t MaxADist=10;

Int_t Z_Offset=27;
Int_t A_Offset=79;

Int_t Min_Energy_Arrow=10;

Cube_Player *Cube;

ENSDF_DATAe680 *DATABASE;

Build_LS *LS;

TFile *fInputChart;

TFile *fOutputSpectra;

Int_t Maximum_Gamma_Read =600;

Int_t Max_RangeUser =2000;

Int_t Number_Of_Option=5;

TH2F *Chart_Bidim=0x0;
TH2F *Current_Matrix=0x0;

TCanvas *C_Chart=0x0;
TCanvas *C_Player=0x0;

TLatex Tl;

Int_t Maximum_Gamma_Exp=100;

Color_t Gamma_Arrow_Color[22]={kRed,kGreen,kBlue,kBlack,kMagenta,kCyan,kGray,kGreen+2,kMagenta-3,kYellow-3,kOrange+8,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10,kPink+10};

//"/agataDAS1/AGATA/Experiments/e680/Analysis/Users/Dudouet/Calibrations/PlotGlobResults/TimeCorr/AllManip_Final_Corr.root"


TString RAW_SPECTRA_PER_M="/agataDAS1/AGATA/Experiments/e680/Analysis/Users/Gui/Calibrations/ROOT_CUBE/Raw_Spectra_Per_M/Raw_Per_M.root";

TString Raw_Directory="/agataDAS1/AGATA/Experiments/e680/Analysis/Users/Gui/Level_Scheme/Import_ENSDF/";

TString Tree_Directory="/agataDAS1/AGATA/Experiments/e680/Analysis/Data/AnalysedTrees/E680_PerIsotope_PerZ_PerM_NoSelection.root";

const char* Exp_Directory ="/agataDAS1/AGATA/Experiments/e680/Analysis/Users/Gui/Level_Scheme/Import_ENSDF/EXP_DATA/";

const char * E_AGATA_DIRECTORY="/agataDAS1/AGATA/Experiments/e680/Analysis/Users/Dudouet/Calibrations/BuildTrees/Trees/Runs.186_188.root";

Color_t Color[27]={1,2,3,4,5,6,7,8,9,kOrange+10,kRed+2,kPink+10,kMagenta-4,kViolet+6,kAzure+10,kCyan+3,kGreen,kYellow,kOrange+8,kRed+4,kPink+8,kMagenta-2,kViolet+8,kAzure+8,kCyan+1,kGreen+2,kPink+10};

TString Name ="Canvas_1";

TString FileName = Form("/agataDAS1/AGATA/Experiments/e680/Analysis/Users/Dudouet/Calibrations/PlotGlobResults/TimeCorr/AllManip_Final_Corr.root");

TString Z[28]={"Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe"};
TString ZMAJ[28]={"CO","NI","CU","ZN","GA","GE","AS","SE","BR","KR","RB","SR","Y","ZR","NB","MO","TC","RU","RH","PD","AG","CD","IN","SN","SB","TE","I","XE"};


double GausFunc(double *xx, double *par)
{
    double x = xx[0];
    double N = par[0];
    double mean = par[1];
    double sigma = par[2];

    double f = N*exp(-0.5*((x-mean)/sigma)*((x-mean)/sigma));

    return f;
}


void ErrorMessage(const char *err)
{
    cout<<"\e[1;91m"<<err<<"\e[0m"<<endl;
}

void InfoMessage(const char *info)
{
    cout<<"\e[1;92m"<<info<<"\e[0m"<<endl;
}

void WarningMessage(const char *warn)
{
    cout<<"\e[1;93m"<<warn<<"\e[0m"<<endl;
}
