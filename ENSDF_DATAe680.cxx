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

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TFile.h"
#include "TString.h"
#include "TObject.h"
# include "TObjArray.h"
#include "TBrowser.h"
#include "TMath.h"
#include "LevelScheme.h"
#include <ENSDF.h>
#include "Link.h"
#include "TList.h"
#include "GammaLink.h"
#include "NuclearLevel.h"
#include "Spin.h"

#include "Cube_Player.h"


#ifndef ENSDF_DATAe680_H
#endif
#include "ENSDF_DATAe680.h"

using namespace std;
using namespace Gw;


ENSDF_DATAe680::~ENSDF_DATAe680()
{

}
ENSDF_DATAe680::ENSDF_DATAe680(const char* name, const char *title) :
        TNamed(name,title)
{
}

Int_t ENSDF_DATAe680::GetSizeOfGammas()
{
    Gw::LevelScheme *lev =new Gw::LevelScheme();

    lev->Import(Raw_Directory,"Dummy");

    Int_t Size= lev->GetLinks().GetSize();

    return Size;

}

Float_t *ENSDF_DATAe680::Import_E(Int_t Zz,Int_t A,Int_t Draw,Int_t Verbose,Float_t StrengthMin)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();

    TString Directory=Raw_Directory;Directory+=Z[Zz];

    Directory+="/";Directory+=A+A_Offset;Directory+=ZMAJ[Zz];Directory+=".ens";

    cout<<"Directory "<<Directory<<endl;

    lev->Import(Directory,ZMAJ[Zz]);

    Float_t *ETab=new Float_t [Maximum_Gamma_Read];
    for(Int_t h=0;h<Maximum_Gamma_Read;h++)
    {
        ETab[h]=0;
    }
    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {

        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);


        Gw::NuclearLevel *IL = (Gw::NuclearLevel*)Link->GetIL();

        Gw::Measure<Float_t> ILE = ((Gw::NuclearLevel*)IL)->GetEnergy();

        Gw::Measure<Float_t> E =  ((Gw::GammaLink*)Link)->GetEnergy();

        Gw::NuclearLevel *FL = (Gw::NuclearLevel*)Link->GetFL();

        Gw::Measure<Float_t> FLE = ((Gw::NuclearLevel*)FL)->GetEnergy();

        IL = (Gw::NuclearLevel*)Link->GetIL();

        Gw::Spin SIL = ((Gw::NuclearLevel*)IL)->GetSpin();

        FL = (Gw::NuclearLevel*)Link->GetFL();

        Gw::Spin SFL = ((Gw::NuclearLevel*)FL)->GetSpin();

        Gw::Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();

        //cout<<"Strength !!!!!!! "<<StrengthMin<<endl;

        if(S.GetValue()>=StrengthMin){ETab[i]=E.GetValue();}else{ETab[i]=0;continue;}

        if(Verbose==1)
        {

            if(i==0)

            {
                cout<<endl;
                InfoMessage("********************************************************************");cout<<endl;
                TString Name_Nuclei="";Name_Nuclei+=(A+A_Offset);Name_Nuclei+=ZMAJ[Zz];
                InfoMessage("*******************************");
                cout<<"           ";InfoMessage(Name_Nuclei);
                InfoMessage("*******************************");cout<<endl;
            }

            //cout<<endl;
            InfoMessage("********************************************************************");cout<<endl;
            InfoMessage(Form("Energy %0.1f Initial Level : %0.1f Final Level : %0.1f",E.GetValue(),ILE.GetValue(),FLE.GetValue()));

            InfoMessage(Form("Spin  Initial Level : %0.1f Final Level : %0.1f",SIL.Get(),SFL.Get()));

            Gw::Measure<Float_t> C = ((GammaLink*)Link)->GetConversion();

            InfoMessage(Form("C.I. %0.00001f",C.GetValue()));

            InfoMessage(Form("Intensity %0.1f",S.GetValue()));

        }


    }


    if(Draw==1)lev->Draw();

    return ETab;

}

Float_t *ENSDF_DATAe680::Import_C(Int_t Z,Int_t A)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();



    Float_t *CTab=new Float_t [lev->GetLinks().GetSize()];

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {

        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);

        Gw::Measure<Float_t> C = ((GammaLink*)Link)->GetConversion();

        CTab[i]=C.GetValue();

        //cout<<"Conversion "<<C.GetValue()<<endl;


    }

    return CTab;

    //lev->Draw();

}
Float_t *ENSDF_DATAe680::Import_S(Int_t Zz,Int_t A)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();

    TString Directory=Raw_Directory;Directory+=Z[Zz];

    Directory+="/";Directory+=A+A_Offset;Directory+=ZMAJ[Zz];Directory+=".ens";

    cout<<"Directory "<<Directory<<endl;

    lev->Import(Directory,ZMAJ[Zz]);

    Float_t *STab=new Float_t [Maximum_Gamma_Read];

    for(Int_t h=0;h<Maximum_Gamma_Read;h++)
    {
        STab[h]=0;
    }

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {
        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);

        Gw::Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        STab[i]=S.GetValue();

        //cout<<"sctrengthhhhhhh "<<STab[i]<<endl;

    }

    return STab;


}

Float_t ENSDF_DATAe680::Get_Ground_Gamma(Int_t Zz,Int_t A)

{

//to be continued...


}


Float_t *ENSDF_DATAe680::Import_ILE(Int_t Z,Int_t A)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();


    lev->Import(Raw_Directory,"Dummy");

    Float_t *ILE=new Float_t [lev->GetLinks().GetSize()];

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {

        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);

        Gw::NuclearLevel *IL = (Gw::NuclearLevel*)Link->GetIL();

        Gw::Measure<Float_t> ILE = ((Gw::NuclearLevel*)IL)->GetEnergy();

        cout<<"Initial Level Energy "<<ILE.GetValue()<<endl;



    }
    return ILE;
    //lev->Draw();
}

Float_t *ENSDF_DATAe680::Import_FLE(Int_t Z,Int_t A)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();


    lev->Import(Raw_Directory,"Dummy");

    Float_t *FLE=new Float_t [lev->GetLinks().GetSize()];

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {

        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);


        Gw::NuclearLevel *FL = (Gw::NuclearLevel*)Link->GetFL();

        Gw::Measure<Float_t> FLE = ((Gw::NuclearLevel*)FL)->GetEnergy();


        cout<<"Final Level Energy "<<FLE.GetValue()<<endl;


    }

    return FLE;

    //lev->Draw();

}

Float_t *ENSDF_DATAe680::Import_SIL(Int_t Z,Int_t A)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();


    lev->Import(Raw_Directory,"Dummy");

    Float_t *SIL=new Float_t [lev->GetLinks().GetSize()];

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {

        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);


        Gw::NuclearLevel *IL = (Gw::NuclearLevel*)Link->GetIL();

        Gw::Spin SIL = ((Gw::NuclearLevel*)IL)->GetSpin();

        cout<<"Spin Initial Level "<<SIL<<endl;

    }

    return SIL;

    //lev->Draw();

}
Float_t *ENSDF_DATAe680::Import_SFL(Int_t Z,Int_t A)

{

    Gw::LevelScheme *lev =new Gw::LevelScheme();


    lev->Import(Raw_Directory,"Dummy");

    Float_t *SFL=new Float_t [lev->GetLinks().GetSize()];

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {

        const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);


        Gw::NuclearLevel *FL = (Gw::NuclearLevel*)Link->GetFL();

        Gw::Spin SFL = ((Gw::NuclearLevel*)FL)->GetSpin();

        cout<<"Spin Initial Level "<<SFL<<endl;


    }

    return SFL;

    //lev->Draw();

}


