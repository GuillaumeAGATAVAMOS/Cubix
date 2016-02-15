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
#include <GLSPlayer.h>
#include "XGammaLink.h"
#include "BaseGEM.h"
#include "BaseGEM.h"
#include "TSeqCollection.h"
#include <vector>
#include "TMatrixD.h"
#include "SpreadIntensityMatrix.h"


#include "Build_LS.h"
#include "ENSDF_DATAe680.h"
#include "Cube_Player.h"
#include "SpreadIntensityMatrix.h"

#ifndef BUILD_LS_H
#endif

using namespace std;
using namespace Gw;


typedef std::vector<Float_t> Float_vec_t;
typedef std::vector<Int_t> Int_vec_t;

Float_vec_t E_Index;
Float_vec_t S_Index;
Int_vec_t Index_Coinc;
Int_vec_t Index_Coinc_Above;
Float_vec_t Corrected_S;
Float_vec_t Corrected_S_Above;
Int_vec_t Index_Sorted_Above;
Float_t Energy=0;

TObjArray *ListPeaks;
TObjArray *List_Raw_Peaks;
TObjArray *DG_Peaks;

Float_t Examine_Dead_Gammas(LevelScheme *lev,Int_t Index_Coinc_Gamma,Int_vec_t Index_Coinc_Gammas)
{

        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gamma);
        Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S =  ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

        //*********************************************************************
        //Examine dead gammas that are important to corrected the real strength
        //*********************************************************************

        Float_t S_C_Dead=0;
        Float_t S_Dead_and_Notdead=0;
        cout<<"GAMMA AAA "<<E.GetValue()<<endl;
        // 1) calculer l'intensitée brute totale des gammas dead+non dead

        for(Int_t o=0;o<(Int_t)lev->GetLinks().GetSize();o++)
        {

            const GammaLink *Linkdead=(GammaLink*)lev->GetLinks().At(o);
            Measure<Float_t> Sdead =  ((GammaLink*)Linkdead)->GetStrength();
            NuclearLevel *ILdead = (NuclearLevel*)Linkdead->GetIL();
            Measure<Float_t> ILEdead = ((NuclearLevel*)ILdead)->GetEnergy();

            if(FLE.GetValue()==ILEdead.GetValue())
            {
                S_Dead_and_Notdead+=Sdead.GetValue();
            }
        }


        // 2) find the dead and the not dead gammas
        for(Int_t o=0;o<(Int_t)lev->GetLinks().GetSize();o++)
        {

            Int_t Dead_Gamma=0;

            const GammaLink *Linkdead=(GammaLink*)lev->GetLinks().At(o);
            Measure<Float_t> Edead =  ((GammaLink*)Linkdead)->GetEnergy();
            Measure<Float_t> Sdead =  ((GammaLink*)Linkdead)->GetStrength();
            NuclearLevel *ILdead = (NuclearLevel*)Linkdead->GetIL();
            Measure<Float_t> ILEdead = ((NuclearLevel*)ILdead)->GetEnergy();
            NuclearLevel *FLdead = (NuclearLevel*)Linkdead->GetFL();
            Measure<Float_t> FLEdead = ((NuclearLevel*)FLdead)->GetEnergy();

            if(FLE.GetValue()==ILEdead.GetValue())
            {

                for(Int_t u=0;u<(Int_t)Index_Coinc_Gammas.size();u++)
                {

                    //cout<<" maybe E dead values "<<Edead.GetValue()<<endl;
                    const GammaLink *Linktest=(GammaLink*)lev->GetLinks().At((Int_t)Index_Coinc_Gammas.at(u));
                    Measure<Float_t> Etest =  ((GammaLink*)Linktest)->GetEnergy();
                    NuclearLevel *ILtest = (NuclearLevel*)Linktest->GetIL();
                    Measure<Float_t> ILEtest = ((NuclearLevel*)ILtest)->GetEnergy();
                    if(Edead.GetValue()==Etest.GetValue())
                    {
                        Dead_Gamma--;
                       // cout<<"not dead"<<Dead_Gamma<<" Energy "<<Edead.GetValue()<<endl;
                    }
                    else
                    {
                        if(ILEdead.GetValue()==ILEtest.GetValue())
                        {
                            //cout<<"dead "<<Edead.GetValue()<<endl;
                            Dead_Gamma++;
                        }
                    }
                }
                if(Dead_Gamma>=1)
                {
                    S_C_Dead+=(Sdead.GetValue()/S_Dead_and_Notdead);
                   // cout<<" is a dead gamma "<<Dead_Gamma<<" erngy "<<Edead.GetValue()<<endl;
                   // cout<<" S_C_Dead "<<S_C_Dead<<endl;
                }
            }
        }

return (1-S_C_Dead);


}




Int_vec_t Iter_Below_LS(Float_vec_t Gate,LevelScheme *lev,Int_t Counter)
{

    Float_vec_t tEnergy;
    tEnergy.clear();

    if(Counter>lev->GetLinks().GetSize())return Index_Coinc;

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(i);
        Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();

        for (std::vector<Float_t>::const_iterator Egate = Gate.begin(); Egate != Gate.end(); ++Egate)
        {
            if(*Egate==E.GetValue())
            {
                NuclearLevel *FL = (NuclearLevel*)Link->GetFL();

                Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

                //once have get the FLE of the (New) gate(s), compare with ILE of possible new gammas

                for(Int_t l=0;l<lev->GetLinks().GetSize();l++)
                {
                    const GammaLink *Link=(GammaLink*)lev->GetLinks().At(l);
                    Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();
                    Measure<Float_t> S =  ((GammaLink*)Link)->GetStrength();
                    NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
                    Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();

                    if(ILE.GetValue()==FLE.GetValue())
                    {
                        cout<<" S.getvalue "<<S.GetValue()<<endl;
                        cout<<" E.getvalue "<<E.GetValue()<<endl;

                        Energy=E.GetValue();
                        tEnergy.push_back(E.GetValue());

                        S_Index.push_back(S.GetValue());
                        Bool_t Not_Yet=true;

                        for (std::vector<Int_t>::const_iterator I = Index_Coinc.begin(); I != Index_Coinc.end(); ++I)
                        {
                            if(*I==l){Not_Yet=false;}
                        }
                        if(Not_Yet){Index_Coinc.push_back(l);E_Index.push_back(E.GetValue());}
                    }

                }
            }
        }
    }

    Counter++;

    Iter_Below_LS(tEnergy,lev,Counter);
    return Index_Coinc;
}

Int_vec_t Iter_Above_LS(Float_vec_t Gate,LevelScheme *lev,Int_t Counter)
{

    Float_vec_t tEnergy_Above;
    tEnergy_Above.clear();


    if(Counter>lev->GetLinks().GetSize())return Index_Coinc;

    for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(i);
        Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();

        for (std::vector<Float_t>::const_iterator Egate = Gate.begin(); Egate != Gate.end(); ++Egate)
        {
            if(*Egate==E.GetValue())
            {
                NuclearLevel *IL = (NuclearLevel*)Link->GetIL();

                Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();

                //once have get the ILE of the (New) gate(s), compare with FLE of other gammas

                for(Int_t l=0;l<lev->GetLinks().GetSize();l++)
                {
                    const GammaLink *Link=(GammaLink*)lev->GetLinks().At(l);
                    Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();
                    Measure<Float_t> S =  ((GammaLink*)Link)->GetStrength();
                    NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
                    Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

                    if(ILE.GetValue()==FLE.GetValue())
                    {
                        cout<<" S.getvalue "<<S.GetValue()<<endl;
                        cout<<" E.getvalue "<<E.GetValue()<<endl;

                        Energy=E.GetValue();
                        tEnergy_Above.push_back(E.GetValue());

                        S_Index.push_back(S.GetValue());
                        Bool_t Not_Yet=true;

                        for (std::vector<Int_t>::const_iterator I = Index_Coinc_Above.begin(); I != Index_Coinc_Above.end(); ++I)
                        {
                            if(*I==l){Not_Yet=false;}
                        }
                        if(Not_Yet){Index_Coinc_Above.push_back(l);E_Index.push_back(E.GetValue());}
                    }

                }
            }
        }
    }

    Counter++;

    Iter_Above_LS(tEnergy_Above,lev,Counter);
    return Index_Coinc_Above;
}

Int_vec_t Sort_Index_Below_LS(Int_vec_t Index_Coinc_Gammas,LevelScheme *lev)
{
    Float_vec_t vecY;vecY.clear();
    Int_vec_t vecX;vecX.clear();
    Int_vec_t vecXInv;vecXInv.clear();

    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas.size();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(i));
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();

        vecY.push_back(ILE.GetValue());
        vecX .push_back(Index_Coinc_Gammas.at(i));
    }

    for(unsigned int i=1 ; i<vecX.size() ; i++)
        for(unsigned int j=0 ; j<vecX.size()-1 ; j++)
            if(vecY[j]>vecY[j+1])
            {
        swap(vecX[j],vecX[j+1]);
        swap(vecY[j],vecY[j+1]);
    }


    for(int i=(vecY.size()-1) ; i>=0 ; i--)
    {
        cout<<vecX[i]<<" || "<<vecY[i]<<endl;
        vecXInv.push_back(vecX[i]);
    }

    return vecXInv;
}

Int_vec_t Sort_Index_Above_LS(Int_vec_t Index_Coinc_Gammas_Above,LevelScheme *lev)
{
    Float_vec_t vecY;vecY.clear();
    Int_vec_t vecX;vecX.clear();
    Int_vec_t vecXInv;vecXInv.clear();

    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas_Above.size();i++)
    {

        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas_Above.at(i));
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

        cout<<"FLE VEC "<<FLE.GetValue()<<endl;

        vecY.push_back(FLE.GetValue());
        vecX .push_back(Index_Coinc_Gammas_Above.at(i));
    }

    for(unsigned int i=1 ; i<vecX.size() ; i++)
        for(unsigned int j=0 ; j<vecX.size()-1 ; j++)
            if(vecY[j]<vecY[j+1])
            {
        swap(vecX[j],vecX[j+1]);
        swap(vecY[j],vecY[j+1]);
    }


    for(int i=(vecY.size()-1) ; i>=0 ; i--)
    {
        cout<<vecX[i]<<" || "<<vecY[i]<<endl;
        vecXInv.push_back(vecX[i]);
    }

    return vecXInv;
}


Float_vec_t Correct_Strength_Below_LS(LevelScheme *lev,Int_vec_t Index_Coinc_Gammas)
{

    Float_t Branching_First_Lvl=0;
    Int_t Nb_Merge_Gammas=0;
    Int_t Nb_Demerge_Gammas=0;
    Corrected_S.clear();
    Float_t S_C=0;

    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas.size();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(i));
        Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S =  ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

        if(i==0)//means you see the first level below the gate //examine the firstlevel and branching ratios
        {
            for(Int_t l=0;l<(Int_t)Index_Coinc_Gammas.size();l++)
            {

                const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(l));
                Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
                Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
                NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

                if(ILE.GetValue()==ILEbranch.GetValue())
                {
                    Branching_First_Lvl++;Corrected_S.push_back(Sbranch.GetValue());
                }
                else continue;

            }//end for under the branching gammas of the first level
        }//end if first Lvl

        //********************Autres Niveau < niveau peuplé********************************

        else if (i>=Branching_First_Lvl)//if i!= FirstLvls below the gate
        {
            Nb_Demerge_Gammas=0;
            Float_t Strength_Branching=0;
            //***************************************************************************
            //********************Examine dermerge Gammas ********************************
            //***************************************************************************

            for(Int_t l=0;l<(Int_t)Index_Coinc_Gammas.size();l++)//Examine demerge ratios
            {
                const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(l));
                Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
                Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
                NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

                if(ILE.GetValue()==ILEbranch.GetValue())// if ILE == ILEbranch
                {

                    Nb_Demerge_Gammas++;

                }
            }//end for under demerge gammas

            if(Nb_Demerge_Gammas>1)//Calul the strenght corrected
            {
                S_C=0;
                Float_t S_Branch=0;

                for(Int_t m=0;m<(Int_t)Index_Coinc_Gammas.size();m++)//Examine demerge ratios
                {
                    const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(m));
                    Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                    Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                    NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
                    Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
                    NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                    Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

                    if(ILE.GetValue()==ILEbranch.GetValue() && i!=m)// if ILE == ILEbranch and don't correlate same Gammas
                    {
                        S_Branch+=Sbranch.GetValue();
                        S_C=S.GetValue()/(S.GetValue()+S_Branch);

                    }

                }//end for under calcul of strenght demerge gammas
                //cout<<"ttttttest "<<S_C<<endl;

            }//END Correct the stregth demerge gammas

            //*******************************************************************************
            //********************END Examine dermerge Gammas ********************************
            //********************************************************************************

            for(Int_t l=0;l<(Int_t)Index_Coinc_Gammas.size();l++)//Examine merge ratios
            {
                const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(l));
                Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
                Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
                NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

                if(ILE.GetValue()==FLEbranch.GetValue())// if FLE == ILE
                {
                    Strength_Branching+=Corrected_S.at(l);
                    Nb_Merge_Gammas++;
                }
            }//end for under merge gammas

            if(Nb_Demerge_Gammas>1) {Corrected_S.push_back(Strength_Branching*S_C);}
            else{Corrected_S.push_back(Strength_Branching);}

        }

        //*****************END Autres Niveau < niveau peuplé********************************
    }


    return Corrected_S;
}



Float_vec_t Correct_Strength_Above_LS(LevelScheme *lev,Int_vec_t Index_Coinc_Gammas)
{

    Float_t Branching_First_Lvl=0;
    Int_t Nb_Merge_Gammas=0;
    Int_t Nb_Demerge_Gammas=0;
    Corrected_S_Above.clear();
    Float_vec_t All_Dead;
    All_Dead.clear();
    Float_t S_C=0;

    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas.size();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(i));
        Measure<Float_t> E =  ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S =  ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

        //*********************************************************************
        //Examine dead gammas that are important to corrected the real strength
        //*********************************************************************

        Float_t S_C_Dead=0;
        Float_t S_Dead_and_Notdead=0;
        cout<<endl;
        cout<<"GAMMA *********"<<E.GetValue()<<"*************"<<endl;
        cout<<endl;
        // 1) calculer l'intensitée brute totale des gammas dead+non dead

        for(Int_t o=0;o<(Int_t)lev->GetLinks().GetSize();o++)
        {

            const GammaLink *Linkdead=(GammaLink*)lev->GetLinks().At(o);
            Measure<Float_t> Sdead =  ((GammaLink*)Linkdead)->GetStrength();
            NuclearLevel *ILdead = (NuclearLevel*)Linkdead->GetIL();
            Measure<Float_t> ILEdead = ((NuclearLevel*)ILdead)->GetEnergy();

            if(FLE.GetValue()==ILEdead.GetValue())
            {
                S_Dead_and_Notdead+=Sdead.GetValue();
            }
        }

        // 2) find the dead and the not dead gammas
        for(Int_t o=0;o<(Int_t)lev->GetLinks().GetSize();o++)
        {

            Int_t Dead_Gamma=0;

            const GammaLink *Linkdead=(GammaLink*)lev->GetLinks().At(o);
            Measure<Float_t> Edead =  ((GammaLink*)Linkdead)->GetEnergy();
            Measure<Float_t> Sdead =  ((GammaLink*)Linkdead)->GetStrength();
            NuclearLevel *ILdead = (NuclearLevel*)Linkdead->GetIL();
            Measure<Float_t> ILEdead = ((NuclearLevel*)ILdead)->GetEnergy();
            NuclearLevel *FLdead = (NuclearLevel*)Linkdead->GetFL();
            Measure<Float_t> FLEdead = ((NuclearLevel*)FLdead)->GetEnergy();

            if(FLE.GetValue()==ILEdead.GetValue())
            {
                for(Int_t u=0;u<(Int_t)Index_Coinc_Gammas.size();u++)
                {
                    const GammaLink *Linktest=(GammaLink*)lev->GetLinks().At((Int_t)Index_Coinc_Gammas.at(u));
                    Measure<Float_t> Etest =  ((GammaLink*)Linktest)->GetEnergy();
                    NuclearLevel *ILtest = (NuclearLevel*)Linktest->GetIL();
                    Measure<Float_t> ILEtest = ((NuclearLevel*)ILtest)->GetEnergy();
                    if(Edead.GetValue()==Etest.GetValue())
                    {
                        Dead_Gamma--;
                        // cout<<"not dead"<<Dead_Gamma<<" Energy "<<Edead.GetValue()<<endl;
                    }
                    else
                    {
                        if(ILEdead.GetValue()==ILEtest.GetValue())
                        {
                            //cout<<"dead "<<Edead.GetValue()<<endl;
                            Dead_Gamma++;
                        }
                    }

                }
                if(Dead_Gamma>=1)
                {
                    S_C_Dead+=(Sdead.GetValue()/S_Dead_and_Notdead);
                    cout<<" is a dead gamma "<<Dead_Gamma<<" erngy "<<Edead.GetValue()<<endl;
                   // cout<<" S_C_Dead "<<1-S_C_Dead<<endl;
                }
            }
        }
        //**********************************************************************************
        //*********END Examine dead gammas that are important to corrected the real strength
        //**********************************************************************************


        if(i==0)//means you see the first gammas above the gate //examine the firstgammas and branching ratios
        {
            for(Int_t l=0;l<(Int_t)Index_Coinc_Gammas.size();l++)
            {

                const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(l));
                Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
                Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
                NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

                if(FLE.GetValue()==FLEbranch.GetValue())
                {
                    Branching_First_Lvl++;Corrected_S_Above.push_back(1);//Sbranch.getvalue();
                    cout<<"E viwed "<<E.GetValue()<<" Sbranch "<<Sbranch.GetValue()<<endl;
                    All_Dead.push_back(1.);
                }
                else continue;

            }//end for under the branching gammas of the first level

            cout<<"branching first level"<<Branching_First_Lvl<<endl;
        }//end if first Lvl

        //********************Autres Niveau > niveau peuplé && pas juste au dessus de la gate********************************

        else if (i>=Branching_First_Lvl)//if i!= FirstGammas above the gate
        {
            Nb_Demerge_Gammas=0;
            Float_t Strength_Branching=0;
//            //***************************************************************************
//            //********************Examine dermerge Gammas ********************************
//            //***************************************************************************

//            for(Int_t l=0;l<(Int_t)Index_Coinc_Gammas.size();l++)//Examine demerge ratios
//            {
//                const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(l));
//                Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
//                Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
//                NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
//                Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
//                NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
//                Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

//                if(FLE.GetValue()==FLEbranch.GetValue())// if FLE == FLEbranch
//                {

//                    Nb_Demerge_Gammas++;

//                }
//            }//end for under demerge gammas

            // cout<<"demerge_gammas "<<Nb_Demerge_Gammas<<endl;
            //  cout<<" Gama watched "<<E.GetValue()<<endl;


//            if(Nb_Demerge_Gammas>1)//Calul the strenght corrected
//            {
                S_C=1;
                Float_t S_Branch=0;

                for(Int_t m=0;m<(Int_t)Index_Coinc_Gammas.size();m++)//Examine demerge ratios
                {
                    const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(m));
                    Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                    Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                    NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                    Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();

                    if(FLE.GetValue()==FLEbranch.GetValue() && i!=m)// if FLE == FLEbranch and don't correlate same Gammas
                    {
                        S_Branch+=Sbranch.GetValue();
                        S_C=S.GetValue()/(S.GetValue()+S_Branch);

                    }


                }//end for under calcul of strenght demerge gammas
                //cout<<"ttttttest "<<S_C<<endl;

                S_C=1;
           /* }*///END Correct the stregth demerge gammas

            //*******************************************************************************
            //********************END Examine dermerge Gammas ********************************
            //********************************************************************************

            //********************************************************************************
            //********************Examine merge Gammas ***************************************
            //********************************************************************************
            Float_t Moyenne_Pondere=0;
            Float_t Strength_to_reconstruct=0;
            Float_t Real_SR=0;
            Nb_Merge_Gammas=0;

            for(Int_t l=0;l<(Int_t)Index_Coinc_Gammas.size();l++)//Examine merge ratios
            {
                const GammaLink *Linkbranch=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(l));
                Measure<Float_t> Ebranch =  ((GammaLink*)Linkbranch)->GetEnergy();
                Measure<Float_t> Sbranch =  ((GammaLink*)Linkbranch)->GetStrength();
                NuclearLevel *ILbranch = (NuclearLevel*)Linkbranch->GetIL();
                Measure<Float_t> ILEbranch = ((NuclearLevel*)ILbranch)->GetEnergy();
                NuclearLevel *FLbranch = (NuclearLevel*)Linkbranch->GetFL();
                Measure<Float_t> FLEbranch = ((NuclearLevel*)FLbranch)->GetEnergy();


                if(FLE.GetValue()==ILEbranch.GetValue())// if FLE == ILE
                {
                    Strength_Branching+=Corrected_S_Above.at(l);

                    cout<<"FLE test "<<FLE.GetValue()<<endl;
                    Strength_to_reconstruct=0;
                    //get the Total strength
                    for(Int_t r=0;r<(Int_t)Index_Coinc_Gammas.size();r++)//Examine merge ratios
                    {
                        const GammaLink *Linkr=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(r));
                        Measure<Float_t> Er =  ((GammaLink*)Linkr)->GetEnergy();
                        NuclearLevel *ILr = (NuclearLevel*)Linkr->GetIL();
                        Measure<Float_t> ILEr = ((NuclearLevel*)ILr)->GetEnergy();
                         Measure<Float_t> Sr =  ((GammaLink*)Linkr)->GetStrength();

                        if(ILEr.GetValue()==FLE.GetValue())
                        {
                            cout<<"E regardé "<<Er.GetValue()<<endl;

                            Strength_to_reconstruct+=Sr.GetValue();
                            cout<<"Sr ----------- = "<<Sr.GetValue()/Strength_to_reconstruct<<endl;
                        }

                    }
                    cout<<"E branch "<<Ebranch.GetValue()<<endl;
                    for(Int_t r=0;r<(Int_t)Index_Coinc_Gammas.size();r++)//Examine merge ratios
                    {
                        const GammaLink *Linkr=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(r));
                        Measure<Float_t> Er =  ((GammaLink*)Linkr)->GetEnergy();
                        NuclearLevel *ILr = (NuclearLevel*)Linkr->GetIL();
                        Measure<Float_t> ILEr = ((NuclearLevel*)ILr)->GetEnergy();
                         Measure<Float_t> Sr =  ((GammaLink*)Linkr)->GetStrength();

                        if(ILEr.GetValue()==FLE.GetValue() && Er.GetValue()==Ebranch.GetValue())
                        {
                            Real_SR=Sr.GetValue()/Strength_to_reconstruct;
                            cout<<"Real  Sr = "<<Sr.GetValue()/Strength_to_reconstruct<<endl;
                        }

                    }

                    //cout<<"Total stregth"<<Strength_to_reconstruct<<endl;
                    //cout<<"First pond : "<<Real_SR*Corrected_S_Above.at(l)<<endl;

                    Moyenne_Pondere+=Real_SR*Corrected_S_Above.at(l);


                    Nb_Merge_Gammas++;

                }
            }//end for under merge gammas

                    cout<<"MOYENN PONDDDDDDDD "<<Moyenne_Pondere<<endl;

            // cout<<" Gamma Watched "<<E.GetValue()<<endl;
           //cout<<" S_C "<<S_C<<endl;
              cout<<"Strength branching "<<Strength_Branching<<endl;
              //cout<<"S_C dead "<<(1-S_C_Dead)<<endl;

                    cout<<" Nb_Merge_Gammas !!!!!!!!"<<Nb_Merge_Gammas<<endl;
                    cout<<"S_C_dead "<<(1-S_C_Dead)<<endl;
              cout<<"ALL "<<(1-S_C_Dead)*Strength_Branching<<endl;

            //if(Nb_Demerge_Gammas>1) {Corrected_S_Above.push_back(Strength_Branching*S_C*(1-S_C_Dead));}
            if(Nb_Merge_Gammas>=2 )
              {
                if(Moyenne_Pondere==1)
                {
                     Corrected_S_Above.push_back(1-S_C_Dead);All_Dead.push_back(1-S_C_Dead);
                }
                    else
                    {

                Corrected_S_Above.push_back(Moyenne_Pondere);All_Dead.push_back(Moyenne_Pondere);

            }
                }
            else
            {
                All_Dead.push_back((1-S_C_Dead)*Strength_Branching);
                Corrected_S_Above.push_back(Strength_Branching*(1-S_C_Dead));
            }


        }

        //*****************END Autres Niveau < niveau peuplé********************************
    }

    return All_Dead;
}



Float_vec_t Get_Strength_Above_LS(LevelScheme *lev,Int_vec_t Index_Coinc_Gammas)//A modifier pour prend en compte les "dead gammas"
{
    Float_vec_t S_above;
    Float_vec_t S_Corrected_Dead;
    Float_vec_t ILETab;
    Float_t Corrected_Dead=1;
    Float_t Total_Corrected=1;
    Float_vec_t Total;
    Float_vec_t Tab_FLE;
    S_above.clear();
    S_Corrected_Dead.clear();
    ILETab.clear();
    Total.clear();


    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas.size();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(i));
        Measure<Float_t> SS =  ((GammaLink*)Link)->GetStrength();

        S_above.push_back(SS.GetValue());

    }
    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas.size();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(i));
        Measure<Float_t> SS =  ((GammaLink*)Link)->GetStrength();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

        Corrected_Dead=Examine_Dead_Gammas(lev,Index_Coinc_Gammas.at(i),Index_Coinc_Gammas);

       cout<<"Corrected_Dead "<<Corrected_Dead<<endl;

        S_Corrected_Dead.push_back(Corrected_Dead);

    }
    for(std::vector<Float_t>::const_iterator C = Total.begin(); C != Total.end(); ++C)
    {
        cout<<"C "<<*C<<endl;
    }



    return S_above;
}

Float_vec_t Get_E_From_ID(LevelScheme *lev,Int_vec_t Index_Coinc_Gammas)
{
    Float_vec_t E;
    E.clear();
    for(Int_t i=0;i<(Int_t)Index_Coinc_Gammas.size();i++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(Index_Coinc_Gammas.at(i));
        Measure<Float_t> EE =  ((GammaLink*)Link)->GetEnergy();
        E.push_back(EE.GetValue());

    }
    return E;
}


Float_t Get_Corrected_Strength_For_The_Lowest_Gate(LevelScheme *lev,Float_t Gate,Float_vec_t Energy_Gammas_Incoinc,Float_vec_t Good_Strength)
{
    Float_t S_Corrected_ForLowest=0;
    Float_t IL_Gate=0;

    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        if(E.GetValue()==Gate)IL_Gate=ILE.GetValue();
    }

    for(Int_t i=0;i<(Int_t)Energy_Gammas_Incoinc.size();i++)
    {
        for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
        {
            const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
            Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
            Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
            NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
            Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();
            if(FLE.GetValue()==IL_Gate && E.GetValue()==Energy_Gammas_Incoinc.at(i))
            {
                S_Corrected_ForLowest+=Good_Strength.at(i);
            }
        }
    }
    return S_Corrected_ForLowest;
}

Float_t Get_Raw_Strength_For_The_Lowest_Gate(LevelScheme *lev,Float_t Gate,Float_vec_t Energy_Gammas_Incoinc)
{
    Float_t S_Corrected_ForLowest=0;
    Float_t IL_Gate=0;

    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        if(E.GetValue()==Gate)IL_Gate=ILE.GetValue();
    }

    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();
        if(FLE.GetValue()==IL_Gate)
        {
            S_Corrected_ForLowest+=S.GetValue();
        }
    }

    return S_Corrected_ForLowest;
}


Float_t Get_Raw_Strength_For_The_Branching_Gate(LevelScheme *lev,Float_t Gate)
{
    Float_t S_Corrected_Branching=0;
    Float_t IL_Gate=0;
    Float_t Raw_S_Gate=0;

    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        if(E.GetValue()==Gate)
        {
            IL_Gate=ILE.GetValue();Raw_S_Gate=S.GetValue();
        }
    }

    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        if(ILE.GetValue()==IL_Gate)
        {
            S_Corrected_Branching+=S.GetValue();
        }
    }

    return Raw_S_Gate/S_Corrected_Branching;
}


Float_t Get_Strength_For_The_Gate(LevelScheme *lev,Float_t Gate)
{

    Float_t Raw_S_Gate=0;

    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        if(E.GetValue()==Gate)
        {
           Raw_S_Gate=S.GetValue();
        }
    }


    return Raw_S_Gate;
}

Float_t Get_Strength_Below_The_Gate(LevelScheme *lev,Float_t Gate)
{

    Float_t Raw_S_Below_Gate=0;
    Float_t ILE_Below=0;


    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();
        if(E.GetValue()==Gate)
        {
          ILE_Below=FLE.GetValue();
        }
    }
    for(Int_t j=0;j<(Int_t)lev->GetLinks().GetSize();j++)
    {
        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(j);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        Measure<Float_t> S = ((GammaLink*)Link)->GetStrength();
        NuclearLevel *IL = (NuclearLevel*)Link->GetIL();
        Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
        if(ILE_Below==ILE.GetValue())
        {
           Raw_S_Below_Gate+=S.GetValue();
        }
    }



    return Raw_S_Below_Gate*0.01;
}


TMatrixD Fill_TS_Matrix(LevelScheme *lev,const Int_t Size)
{

    TMatrixD Fill_M(Size,Size);

    for(Int_t i=0;i<(Int_t)Size;i++)
    {

        const GammaLink *Link=(GammaLink*)lev->GetLinks().At(i);
        Measure<Float_t> E = ((GammaLink*)Link)->GetEnergy();
        NuclearLevel *FL = (NuclearLevel*)Link->GetFL();
        Measure<Float_t> FLE = ((NuclearLevel*)FL)->GetEnergy();

        Float_t Br=0;

        for(Int_t j=0;j<(Int_t)Size;j++)//Get The branching ratio
        {

            const GammaLink *Link2=(GammaLink*)lev->GetLinks().At(j);
            Measure<Float_t> E2 = ((GammaLink*)Link2)->GetEnergy();
            NuclearLevel *IL = (NuclearLevel*)Link2->GetIL();
            Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
            Measure<Float_t> S = ((GammaLink*)Link2)->GetStrength();

            if(FLE.GetValue()==ILE.GetValue())
            {
            Br+=S.GetValue();
            }
        }

        for(Int_t j=0;j<(Int_t)Size;j++)//Fill the matrix
        {

            const GammaLink *Link2=(GammaLink*)lev->GetLinks().At(j);
            Measure<Float_t> E2 = ((GammaLink*)Link2)->GetEnergy();
            NuclearLevel *IL = (NuclearLevel*)Link2->GetIL();
            Measure<Float_t> ILE = ((NuclearLevel*)IL)->GetEnergy();
            Measure<Float_t> S = ((GammaLink*)Link2)->GetStrength();

            if(FLE.GetValue()==ILE.GetValue())
            {
            Fill_M[i][j]=S.GetValue()/Br;
            }
        }


    }

    return Fill_M;
}


TMatrixD Get_S_Matrix(LevelScheme *lev,const Int_t Size)
{

    TMatrixD Fill_S(Size,1);

        for(Int_t j=0;j<(Int_t)Size;j++)//Fill the matrix with the raw strength
        {

            const GammaLink *Link2=(GammaLink*)lev->GetLinks().At(j);
            Measure<Float_t> S = ((GammaLink*)Link2)->GetStrength();


            Fill_S[j][0]=S.GetValue();

    }

    return Fill_S;
}


TMatrixD Get_E_Matrix(LevelScheme *lev,const Int_t Size)
{

    TMatrixD Fill_E(Size,1);

        for(Int_t j=0;j<(Int_t)Size;j++)//Fill the matrix with the Energy
        {

            const GammaLink *Link2=(GammaLink*)lev->GetLinks().At(j);
            Measure<Float_t> E = ((GammaLink*)Link2)->GetEnergy();

            Fill_E[j][0]=E.GetValue();
    }

    return Fill_E;
}


Build_LS::Build_LS()

{

}

Build_LS::Build_LS(const char* name, const char *title,const char *Exp_Directory) :
        TNamed(name,title)
{

    TCanvas *C_Player(0x0);
    TF1* fGaus(0x0);
    Float_t *ETab(0x0);
    Float_t *EStrength(0x0);
    Gw::LevelScheme *lev(0x0);
    Gw::GLSPlayer *Plev(0x0);
    Directory=Exp_Directory;
    Raw_Directory=Exp_Directory;
    Float_t *Eexp =(0x0);
    Float_t *Sexp =(0x0);
    Float_t *E =(0x0);
    Float_t *S =(0x0);
    Float_t Format_Intensities=0.01; //0.01 si intensités en pourcentages
    Float_t Delta_Gate=0.5;
    TList* ListPeaks(0x0);
    Float_vec_t Coinc_Gammas;
    Int_t Counter=0;

    Gw::Build_LS *Nuclei_Lev(0x0);

    Gw::ENSDF_DATAe680 *Nuclei_levENSDF(0x0);


}

Build_LS::~Build_LS()
{
    delete C_Player;
    delete ETab;
    delete EStrength;
    delete Eexp;
    delete Sexp;
    delete E;
    delete S;
}

TF1 *Build_LS::GausFunc(Float_t Ampl,Float_t mean, Float_t sigma)
{

    fGaus = new TF1("fa","([0]/([2]*2.206627))*exp(-((x-[1])*(x-[1]))/(2*[2]*[2]))",mean-3*sigma,mean+3*sigma);
    fGaus->SetParameter(0,Ampl);
    fGaus->SetParameter(1,mean);
    fGaus->SetParameter(2,sigma);


    return fGaus;
}


double finter(double *x, double *par)
{
    Double_t xx=0;
    for(Int_t i=0;i<ListPeaks->GetEntries();i++)
    {

        TF1 *f =(TF1*)ListPeaks->At(i);
        xx+=par[0]*(f->Eval(x[0]));

    }
    return xx;
}
double Raw_finter(double *x, double *par)
{
    Double_t xx=0;
    for(Int_t i=0;i<List_Raw_Peaks->GetEntries();i++)
    {

        TF1 *f =(TF1*)List_Raw_Peaks->At(i);
        xx+=par[0]*(f->Eval(x[0]));

    }
    return xx;
}

double DG_finter(double *x, double *par)
{
    Double_t xx=0;
    for(Int_t i=0;i<DG_Peaks->GetEntries();i++)
    {

        TF1 *f =(TF1*)DG_Peaks->At(i);
        xx+=par[0]*(f->Eval(x[0]));

    }
    return xx;
}


void Build_LS::Plot_LS(TString Name_Nuclei)
{


    Directory+=Name_Nuclei;Directory +=".root";

    InfoMessage(Name_Nuclei);
    Int_t Zz=0;
    TString B=Name_Nuclei;
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

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        //Create Level Scheme

        lev =new Gw::LevelScheme();

        Gw::GLSPlayer *Plev = (Gw::GLSPlayer*)lev->GetPlayer();

        Plev->AddGroundLevel(Zz+Z_Offset,AInt);

        Plev->Draw();

        gSystem->cd(Exp_Directory);

    }
    else
    {
        //Read existing LevelScheme

        TFile *FileOut = new TFile(Directory,"UPDATE");
        Plev=(Gw::GLSPlayer*)FileOut->Get(Name_Nuclei);

        TCanvas *C_Player=new TCanvas("C_Player","C_Player",1,1);

        C_Player->cd();

        Plev->Draw();

        gSystem->cd(Raw_Directory);

        FileOut->Close();

        delete FileOut;

    }

}

Float_t *Build_LS::GetGammas(Int_t Zzz, Int_t Aa)
{

    TString Name_Nuclei="";Name_Nuclei+=Aa;Name_Nuclei+=Z[Zzz-Z_Offset];

    Directory = Exp_Directory;
    Directory+=Name_Nuclei;Directory +=".root";

    InfoMessage(Name_Nuclei);
    Int_t Zz=0;
    TString B=Name_Nuclei;
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

    ETab=new Float_t [Maximum_Gamma_Read];

    for(Int_t h=0;h<Maximum_Gamma_Read;h++)
    {
        ETab[h]=0;
    }

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        InfoMessage("*****************************************************");
        InfoMessage("No Level Scheme determined in this work is availaible");
        InfoMessage("*****************************************************");

    }
    else
    {
        TFile *FileOut = new TFile(Directory,"OPEN");
        //Name_Nuclei+=";1";
        Gw::LevelScheme *lev=(Gw::LevelScheme*)FileOut->Get(Name_Nuclei);


        InfoMessage("******************************************");
        InfoMessage("Level Scheme done in this works was found");
        InfoMessage("******************************************");
        InfoMessage("Number of links");
        cout<<lev->GetLinks().GetSize()<<endl;


        for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
        {
            const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);
            Gw::Measure<Float_t> E =  ((Gw::GammaLink*)Link)->GetEnergy();

            ETab[i]=E.GetValue();

            InfoMessage("Energy");
            cout<<ETab[i]<<endl;

        }
    }

    return ETab;

}

Float_t *Build_LS::GetStrength(Int_t Zzz, Int_t Aa)
{

    TString Name_Nuclei="";Name_Nuclei+=Aa;Name_Nuclei+=Z[Zzz-Z_Offset];

    TString Directory = Exp_Directory;
    Directory+=Name_Nuclei;Directory +=".root";

    InfoMessage(Name_Nuclei);

    EStrength=new Float_t [Maximum_Gamma_Read];

    for(Int_t h=0;h<Maximum_Gamma_Read;h++)
    {
        EStrength[h]=0;
    }

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        InfoMessage("*****************************************************");
        InfoMessage("No Level Scheme determined in this work is availaible");
        InfoMessage("*****************************************************");

    }
    else
    {
        TFile *FileOut = new TFile(Directory,"OPEN");
        //Name_Nuclei+=";1";
        Gw::LevelScheme *lev=(Gw::LevelScheme*)FileOut->Get(Name_Nuclei);


        InfoMessage("******************************************");
        InfoMessage("Level Scheme done in this works was found");
        InfoMessage("******************************************");
        InfoMessage("Number of links");
        cout<<lev->GetLinks().GetSize()<<endl;


        for(Int_t i=0;i<lev->GetLinks().GetSize();i++)
        {
            const Gw::GammaLink *Link=(Gw::GammaLink*)lev->GetLinks().At(i);
            Gw::Measure<Float_t> S =  ((Gw::GammaLink*)Link)->GetStrength();

            EStrength[i]=S.GetValue();

            InfoMessage("Strength");
            cout<<EStrength[i]<<endl;

        }
    }

    return EStrength;
    delete EStrength;

}


TObjArray * Build_LS::Get_Raw_Intensities(Int_t Zzz, Int_t A,Float_t Energy_Ref,Float_t Height,TGraphErrors *Graph_FWHM,Color_t Color)
{

    Format_Intensities=0.01;
    Float_t Coef_denormalisation=Height*((Graph_FWHM->Eval(Energy_Ref))/2.355)*2.2066;

    Float_t *Eexp =new Float_t[Maximum_Gamma_Read];
    Float_t *Sexp =new Float_t[Maximum_Gamma_Read];
    Float_t *E =new Float_t[Maximum_Gamma_Read];
    Float_t *S =new Float_t[Maximum_Gamma_Read];

    for(Int_t i=0;i<=Maximum_Gamma_Read;i++)
    {
        Sexp[i]=0;
        Eexp[i]=0;
        S[i]=0;
        E[i]=0;
    }

    List_Raw_Peaks= new TObjArray();

    gPad->cd();

    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);

    Eexp=Nuclei_Lev->GetGammas(Zzz,A);

    Sexp=Nuclei_Lev->GetStrength(Zzz,A);

    Gw::ENSDF_DATAe680 *Nuclei_levENSDF=new Gw::ENSDF_DATAe680("","");

    E=Nuclei_levENSDF->Import_E(Zzz-Z_Offset,A-A_Offset);
    S=Nuclei_levENSDF->Import_S(Zzz-Z_Offset,A-A_Offset);

    //E680 DATABase
    for(Int_t i=0;i<=Maximum_Gamma_Read;i++)
    {

        if(Sexp[i]==0 || Eexp[i] ==0)continue;
        TF1*f=(TF1*)Nuclei_Lev->GausFunc(Format_Intensities*Sexp[i]*Coef_denormalisation,Eexp[i],(Graph_FWHM->Eval(Eexp[i]))/2.355);//to convert FWHM to sigma
        TString Name_Function="";Name_Function+=Eexp[i];
        f->SetLineColor(Color);
        f->SetName(Name_Function);
        List_Raw_Peaks->Add(f);
        // f->Draw("SAME");
        cout<<"test E "<<Eexp[i]<<endl;

    }

    //ENSDF DATABase
//    for(Int_t i=0;i<=Maximum_Gamma_Read;i++)
//    {
//        if(S[i]==0 || E[i] ==0)continue;
//        TF1*f=(TF1*)Nuclei_Lev->GausFunc(Format_Intensities*S[i]*Coef_denormalisation,E[i],(Graph_FWHM->Eval(E[i]))/2.355);//to convert FWHM to sigma
//        TString Name_Function="";Name_Function+=E[i];
//        f->SetName(Name_Function);
//        f->SetLineColor(kRed);
//        List_Raw_Peaks->Add(f);
//        // f->Draw("SAME");
//    }

    List_Raw_Peaks->ls();

    All_Raw_Func_Theo = new TF1("All_Strength",Raw_finter,0,2000,1);
    All_Raw_Func_Theo->SetParameter(0,1.);
    All_Raw_Func_Theo->SetLineColor(Color);
    All_Raw_Func_Theo->SetNpx(10000);

    All_Raw_Func_Theo->Draw("SAME");




    return List_Raw_Peaks;

    delete Nuclei_Lev;
    delete Nuclei_levENSDF;
    delete ListPeaks;
    delete Graph_FWHM;

}

void Build_LS::Draw_Arrow(Float_t Scale,Float_t Max_Position,Float_t Energy,Float_t Strength,Int_t Color)
{

    TArrow *Gamma_Arrow = new TArrow(Energy,(Max_Position + Scale/100.) ,Energy,(Max_Position +Scale/10.),0.005,"<|");
    Gamma_Arrow->SetAngle(40);
    Gamma_Arrow->SetLineColor(Color);
    Gamma_Arrow->SetFillColor(Color);
    Gamma_Arrow->SetLineWidth(1);
    Gamma_Arrow->Draw();
    TLatex Tl;
    TString Intensity=Form("%0.1f",Energy);Intensity+=" (";Intensity+=Form("%3.3f",Strength);Intensity+=")";
    Tl.SetTextAngle(90);
    Tl.SetTextColor(Color);
    Tl.SetTextSize(0.012);
    Tl.DrawLatex(Energy,(Max_Position +1.2*Scale/10.),Intensity);

}

TObjArray *Build_LS::Get_Multi_Gated_Intensities(TString Name_Nuclei,Float_t Gate,Float_t Energy_Ref,Float_t Height,TGraphErrors *Graph_FWHM,Color_t Color,Bool_t SAME)
{

    Format_Intensities=0.01;
    Float_t Coef_denormalisation=Height*((Graph_FWHM->Eval(Energy_Ref))/2.355)*2.2066;
    Float_t Raw_Strength_For_The_Branching_Gate=0;

    E_Index.clear();
    S_Index.clear();
    Index_Coinc.clear();
    Index_Coinc_Above.clear();
    Coinc_Gammas.clear();
    Corrected_Strength.clear();
    Counter=0;
    Gate_Tab.clear();
    Gate_Tab.push_back(Gate);
    Float_vec_t Energy;
    Energy.clear();
    Float_t Strength_Of_Gate=0;
    Float_t Raw_Strength_Below_Gate=0;

    Float_vec_t Energy_Above;
    Energy_Above.clear();
    Float_vec_t Strength_Above;
    Strength_Above.clear();
    Int_vec_t Coinc_Gammas_Above;
    Coinc_Gammas_Above.clear();

    Float_vec_t All_Dead;
    All_Dead.clear();


    Index_Sorted_Above.clear();

    Float_vec_t All_Energy;
    All_Energy.clear();
    Float_vec_t All_S;
    All_S.clear();


    ListPeaks= new TObjArray();

    Directory+=Name_Nuclei;Directory +=".root";

    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);

    TString Title_Gate="Gate ";
    Title_Gate+=Gate;

    if(SAME)
    {
        gPad->cd();
        gPad->SetTitle(Title_Gate);
    }
    else
    {
        TCanvas *CStrength=new TCanvas("CStrength","CStrength",600,800);
        CStrength->DrawFrame(0,0,1000,100);
        CStrength->cd();
        CStrength->SetTitle(Title_Gate);
    }

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        InfoMessage("*****************************************************");
        InfoMessage("No Level Scheme determined in this work is availaible");
        InfoMessage("*****************************************************");

    }
    else
    {
        TFile *FileOut = new TFile(Directory,"OPEN");
        Gw::LevelScheme *lev=(Gw::LevelScheme*)FileOut->Get(Name_Nuclei);


        InfoMessage("******************************************");
        InfoMessage("Level Scheme done in this works was found");
        InfoMessage("******************************************");
        InfoMessage("Number of links");
        cout<<lev->GetLinks().GetSize()<<endl;

        InfoMessage("******************************************");
        InfoMessage("   Get Strength from Simple-Gated LS       ");
        InfoMessage("******************************************");


        //Calcul below LS
        Coinc_Gammas=Iter_Below_LS(Gate_Tab,lev,Counter);

        Index_Coinc=Sort_Index_Below_LS(Coinc_Gammas,lev);

        Strength_Of_Gate=Get_Strength_For_The_Gate(lev,Gate);

        Corrected_Strength=Correct_Strength_Below_LS(lev,Index_Coinc);

        Energy=Get_E_From_ID(lev,Index_Coinc);



        //Get the Total Strenght below the gate

        Raw_Strength_Below_Gate=Get_Strength_Below_The_Gate(lev,Gate);

        if(Raw_Strength_Below_Gate==0)//means gate is in the fundamental state
        {
            Raw_Strength_Below_Gate=1;
        }

        //Get the corrected factor from branching ratio to the gate
        Raw_Strength_For_The_Branching_Gate=Get_Raw_Strength_For_The_Branching_Gate(lev,Gate_Tab[0]);



        //Calcul Above_LS

        Coinc_Gammas_Above=Iter_Above_LS(Gate_Tab,lev,Counter);

        Index_Sorted_Above=Sort_Index_Above_LS(Coinc_Gammas_Above,lev);

        Strength_Above=Get_Strength_Above_LS(lev,Index_Sorted_Above);

        All_Dead=Correct_Strength_Above_LS(lev,Index_Sorted_Above);

        Energy_Above=Get_E_From_ID(lev,Index_Sorted_Above);


        Directory=Exp_Directory;
        FileOut->Close();
    }

    //Below
    cout<<"Below--------------"<<endl;
    for (std::vector<Float_t>::const_iterator S = Corrected_Strength.begin(); S != Corrected_Strength.end(); ++S)
    {
        std::cout << *S << ' ';All_S.push_back(*S*(Strength_Of_Gate*0.01));
    }

    cout<<endl;

    Int_t k=0;
    Int_t Index_Energy_Ref=0;//to get the strength of the ref energy

    for (std::vector<Float_t>::const_iterator E = Energy.begin(); E!= Energy.end(); ++E)
    {
        std::cout << *E << ' ';All_Energy.push_back(*E);
        if(*E==Energy_Ref)Index_Energy_Ref=k;
        k++;
    }

    cout<<endl;
    //Above

    cout<<"ABOVE--------------"<<endl;
    cout<<endl;
    Int_t m=0;
    for (std::vector<Float_t>::const_iterator S = Strength_Above.begin(); S != Strength_Above.end(); ++S)
    {

        std::cout <<"S_Above"<< *S << ' '<<endl;All_S.push_back(*S*Raw_Strength_For_The_Branching_Gate*Raw_Strength_Below_Gate*All_Dead.at(m));
        cout<<"All_Dead "<<All_Dead.at(m)<<endl;
        m++;

    }

    cout<<endl;

    for (std::vector<Float_t>::const_iterator E = Energy_Above.begin(); E!= Energy_Above.end(); ++E)
    {
        std::cout <<"E_Above"<< *E << ' '<<endl;All_Energy.push_back(*E);
        if(*E==Energy_Ref)Index_Energy_Ref=k;
        k++;
    }
    cout<<endl;

    // E680 DATABase
    // Build all TF1 functions
    Int_t i=0;Float_t Scale=0;
    for(std::vector<Float_t>::const_iterator S = All_S.begin(); S != All_S.end(); ++S)
    {

        TF1*f=(TF1*)Nuclei_Lev->GausFunc(*S*Format_Intensities*(100/All_S.at(Index_Energy_Ref))*Coef_denormalisation,All_Energy.at(i),(Graph_FWHM->Eval(All_Energy.at(i)))/2.355);//to convert FWHM to sigma
        TString Name_Function="";Name_Function+=All_Energy.at(i);
        f->SetLineColor(Color);
        f->SetName(Name_Function);
        if(f->GetMaximum()>=Scale)Scale=f->GetMaximum();
        ListPeaks->Add(f);
        //f->Draw("SAME");
        i++;
    }


    All_Func_Theo = new TF1("All_Strength",finter,0,1000,1);
    All_Func_Theo->SetParameter(0,1.);
    All_Func_Theo->SetLineColor(Color);
    All_Func_Theo->SetNpx(10000);

    All_Func_Theo->SetTitle(Title_Gate);
    All_Func_Theo->GetYaxis()->SetRangeUser(0,Scale+0.3*Scale);

    if(SAME)
    {
        // All_Func_Theo->Draw("SAME");

    }
    else
    {
        All_Func_Theo->Draw();
    }

    //Draw Name of each peaks

    Int_t j=0;
    TH1F *hgexact = new TH1F("Gated1Exact","Gated1Exact",8192,0,4096);
    const int taille=All_S.size();


    Float_t Total_Scale=0;

    for(std::vector<Float_t>::const_iterator S = All_S.begin(); S != All_S.end(); ++S)
    {
        if(All_Energy.at(j)==Energy_Ref){Total_Scale=Height/(*S);}
        j++;
    }


    j=0;
    for(std::vector<Float_t>::const_iterator S = All_S.begin(); S != All_S.end(); ++S)
    {
        Int_t FWHMInt=(Int_t)((Graph_FWHM->Eval(All_Energy.at(j)))/2.355)*10;
        Float_t MaxPosition=0;

        for(Int_t i=(-2*FWHMInt);i<(2*FWHMInt);i++)//get Max Position Scanning over 2 sigma
        {
            if(MaxPosition<=(All_Func_Theo->Eval(All_Energy.at(j)+i/10)))MaxPosition=All_Func_Theo->Eval(All_Energy.at(j)+i/10);
        }

        hgexact->Fill(All_Energy.at(j),*S*Total_Scale);

        TF1*f=(TF1*)ListPeaks->At(j);
        Draw_Arrow(Scale,MaxPosition,All_Energy.at(j),*S,Color);
        j++;
    }

    hgexact->SetLineColor(kRed);


    Double_t Egr[taille];
    Double_t Sgr[taille];
    Double_t egr[taille];


    j=0;
    for(std::vector<Float_t>::const_iterator S = All_S.begin(); S != All_S.end(); ++S)
    {
        Egr[j]=All_Energy.at(j);
        Sgr[j]=*S*Total_Scale;
        egr[j]=0.02;
        j++;

    }

    TFile *f =new TFile("test.root","recreate");
    TGraphErrors *gr =new TGraphErrors(All_S.size(),Egr,Sgr,egr,egr);

    gr->Write();
    f->Close();
    gPad->Modified();
    gPad->Update();


    return ListPeaks;
    delete ListPeaks;


}

TMatrixD Build_LS::Get_Gated_Intensities_From_TS(TString Name_Nuclei,Float_t Gate,Float_t Energy_Ref,Float_t Height)
{

    //----INITIALISATION Directory

    Directory+=Name_Nuclei;Directory +=".root";

    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);

    Gw::SpreadIntensityMatrix *SIM = new Gw::SpreadIntensityMatrix("","");

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        InfoMessage("*****************************************************");
        InfoMessage("No Level Scheme determined in this work is availaible");
        InfoMessage("*****************************************************");

    }
    else
    {
        TFile *FileOut = new TFile(Directory,"OPEN");
        Gw::LevelScheme *lev=(Gw::LevelScheme*)FileOut->Get(Name_Nuclei);


        InfoMessage("******************************************");
        InfoMessage("Level Scheme done in this works was found");
        InfoMessage("******************************************");
        InfoMessage("Number of links");
        cout<<lev->GetLinks().GetSize()<<endl;

        InfoMessage("******************************************");
        InfoMessage("   Get Strength from Simple-Gated LS       ");
        InfoMessage("******************************************");

        //Perform the calcul of the Total Final Transition-Space Matrix

        const Int_t Dim_LS=lev->GetLinks().GetSize();

        TMatrixD Mat_LS(Dim_LS,Dim_LS);//Raw matrix in TS Space
        TMatrixD Mat_S(Dim_LS,1);// Raw Matrix of stregnth
        TMatrixD Mat_E(Dim_LS,1);// Raw Matrix of Energy
        TMatrixD Mat_P(Dim_LS,Dim_LS);// Total probability matrix
        TMatrixD Mat_Final(Dim_LS,Dim_LS);// Total Final matrix
        TMatrixD S_And_E(Dim_LS,2); // to get the energy and the corresponidng strength


        Mat_LS=Fill_TS_Matrix(lev,Dim_LS);

       // Mat_LS.Print();

        Mat_P=SIM->Get_Total_P(Mat_LS,Dim_LS);
        Mat_P.Print();

        Mat_S=Get_S_Matrix(lev,Dim_LS);

        Mat_E=Get_E_Matrix(lev,Dim_LS);

        //cout<<"Mat_S "<<endl;
       // Mat_S.Print();

       // cout<<"Mat_E "<<endl;
       // Mat_E.Print();

        Mat_Final=SIM->Get_Final_M(Mat_P,Mat_S,Dim_LS);

        Mat_Final.Print();


        //Get the right column of the matrix corresponding to the Gate and the correspond energies

        S_And_E=SIM->Get_S_And_E(Mat_Final,Mat_E,Gate,Dim_LS);

        S_And_E.Print();

        Directory=Exp_Directory;
        FileOut->Close();


        const Int_t taille=Dim_LS;
        Double_t Egr[taille];
        Double_t Sgr[taille];
        Double_t egr[taille];


        Float_t Coef=0;


        for(Int_t i=0;i<Dim_LS;i++)
        {
            if(S_And_E[i][0]==Energy_Ref){


                Coef=Height/(S_And_E[i][1]);
                }


        }
        cout<<"Coef "<<Coef<<endl;
        for(Int_t i=0;i<Dim_LS;i++)
        {
            Egr[i]=S_And_E[i][0];
            Sgr[i]=S_And_E[i][1]*Coef;
            egr[i]=0.01;


        }

        TFile *f =new TFile("TESTM.root","recreate");
        TGraphErrors *gr =new TGraphErrors(Dim_LS,Egr,Sgr,egr,egr);

        gr->Write();
        f->Close();


            return S_And_E;

    }

}


TMatrixD Build_LS::Get_Double_Gated_Intensities_From_TS(TString Name_Nuclei,Float_t Gate1,Float_t Gate2,Float_t Energy_Ref,Float_t Height)
{

    //----INITIALISATION GATES

    Float_t GateMin=0;
    Float_t GateMax=0;

    GateMax=Gate1;
    GateMin=Gate2;

    Directory+=Name_Nuclei;Directory +=".root";

    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);

    Gw::SpreadIntensityMatrix *SIM = new Gw::SpreadIntensityMatrix("","");

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        InfoMessage("*****************************************************");
        InfoMessage("No Level Scheme determined in this work is availaible");
        InfoMessage("*****************************************************");

    }
    else
    {
        TFile *FileOut = new TFile(Directory,"OPEN");
        Gw::LevelScheme *lev=(Gw::LevelScheme*)FileOut->Get(Name_Nuclei);


        InfoMessage("******************************************");
        InfoMessage("Level Scheme done in this works was found");
        InfoMessage("******************************************");
        InfoMessage("Number of links");
        cout<<lev->GetLinks().GetSize()<<endl;

        InfoMessage("******************************************");
        InfoMessage("   Get Strength from Simple-Gated LS       ");
        InfoMessage("******************************************");

        //Perform the calcul of the Total Final Transition-Space Matrix

        const Int_t Dim_LS=lev->GetLinks().GetSize();


        cout<<"DIMMMMMM "<<Dim_LS<<endl;

        TMatrixD Mat_LS(Dim_LS,Dim_LS);//Raw matrix in TS Space
        TMatrixD Mat_S(Dim_LS,1);// Raw Matrix of stregnth
        TMatrixD Mat_E(Dim_LS,1);// Raw Matrix of Energy
        TMatrixD Mat_P(Dim_LS,Dim_LS);// Total probability matrix
        TMatrixD Mat_Final(Dim_LS,Dim_LS);// Total Final matrix
        TMatrixD S_And_E(Dim_LS,2); // to get the energy and the corresponidng strength

        Mat_LS=Fill_TS_Matrix(lev,Dim_LS);

        Mat_P=SIM->Get_Total_P(Mat_LS,Dim_LS);
        Mat_P.Print();

        Mat_S=Get_S_Matrix(lev,Dim_LS);

        Mat_E=Get_E_Matrix(lev,Dim_LS);

        Mat_Final=SIM->Get_Final_M(Mat_P,Mat_S,Dim_LS);

        Mat_Final.Print();

        //Get the right column of the matrix corresponding to the Gate and the correspond energies

        S_And_E=SIM->Get_S_And_E_2(Mat_S,Mat_P,Mat_Final,Mat_E,GateMax,GateMin,Dim_LS);

        S_And_E.Print();

        Directory=Exp_Directory;
        FileOut->Close();


        const Int_t taille=Dim_LS;
        Double_t Egr[taille];
        Double_t Sgr[taille];
        Double_t egr[taille];


        Float_t Coef=0;


        for(Int_t i=0;i<Dim_LS;i++)
        {
            if(S_And_E[i][0]==Energy_Ref){


                Coef=Height/(S_And_E[i][1]);
                }


        }

        for(Int_t i=0;i<Dim_LS;i++)
        {
            Egr[i]=S_And_E[i][0];
            Sgr[i]=S_And_E[i][1]*Coef;
            egr[i]=0.01;

        }

        TFile *f =new TFile("TESTM2G.root","recreate");
        TGraphErrors *gr =new TGraphErrors(Dim_LS,Egr,Sgr,egr,egr);

        gr->Write();
        f->Close();


            return S_And_E;

    }


}

TMatrixD Build_LS::Get_Triple_Gated_Intensities_From_TS(TString Name_Nuclei,Float_t Gate1,Float_t Gate2,Float_t Gate3,Float_t Energy_Ref,Float_t Height)
{

    //----INITIALISATION GATES

    Float_t GateMin=0;
    Float_t GateMax=0;

    GateMax=Gate1;
    GateMin=Gate2;

    Directory+=Name_Nuclei;Directory +=".root";

    Gw::Build_LS *Nuclei_Lev =new Gw::Build_LS("","",Exp_Directory);

    Gw::SpreadIntensityMatrix *SIM = new Gw::SpreadIntensityMatrix("","");

    if(!gSystem->IsFileInIncludePath(Directory))
    {

        InfoMessage("*****************************************************");
        InfoMessage("No Level Scheme determined in this work is availaible");
        InfoMessage("*****************************************************");

    }
    else
    {
        TFile *FileOut = new TFile(Directory,"OPEN");
        Gw::LevelScheme *lev=(Gw::LevelScheme*)FileOut->Get(Name_Nuclei);


        InfoMessage("******************************************");
        InfoMessage("Level Scheme done in this works was found");
        InfoMessage("******************************************");
        InfoMessage("Number of links");
        cout<<lev->GetLinks().GetSize()<<endl;

        InfoMessage("******************************************");
        InfoMessage("   Get Strength from Simple-Gated LS       ");
        InfoMessage("******************************************");

        //Perform the calcul of the Total Final Transition-Space Matrix

        const Int_t Dim_LS=lev->GetLinks().GetSize();

        TMatrixD Mat_LS(Dim_LS,Dim_LS);//Raw matrix in TS Space
        TMatrixD Mat_S(Dim_LS,1);// Raw Matrix of stregnth
        TMatrixD Mat_E(Dim_LS,1);// Raw Matrix of Energy
        TMatrixD Mat_P(Dim_LS,Dim_LS);// Total probability matrix
        TMatrixD Mat_Final(Dim_LS,Dim_LS);// Total Final matrix
        TMatrixD S_And_E(Dim_LS,2); // to get the energy and the corresponidng strength


        Mat_LS=Fill_TS_Matrix(lev,Dim_LS);

       // Mat_LS.Print();

        Mat_P=SIM->Get_Total_P(Mat_LS,Dim_LS);
       // Mat_P.Print();

        Mat_S=Get_S_Matrix(lev,Dim_LS);

        Mat_E=Get_E_Matrix(lev,Dim_LS);

        //cout<<"Mat_S "<<endl;
       // Mat_S.Print();

       // cout<<"Mat_E "<<endl;
        Mat_E.Print();

        Mat_Final=SIM->Get_Final_M(Mat_P,Mat_S,Dim_LS);

        //Mat_Final.Print();


        //Get the right column of the matrix corresponding to the Gate and the correspond energies

        S_And_E=SIM->Get_S_And_E_3(Mat_P,Mat_Final,Mat_E,Gate1,Gate2,Gate3,Dim_LS);

        S_And_E.Print();

        Directory=Exp_Directory;
        FileOut->Close();


        const Int_t taille=Dim_LS;
        Double_t Egr[taille];
        Double_t Sgr[taille];
        Double_t egr[taille];


        Float_t Coef=0;


        for(Int_t i=0;i<Dim_LS;i++)
        {
            if(S_And_E[i][0]==Energy_Ref){


                Coef=Height/(S_And_E[i][1]);
                }


        }
        cout<<"Coef "<<Coef<<endl;
        for(Int_t i=0;i<Dim_LS;i++)
        {
            Egr[i]=S_And_E[i][0];
            Sgr[i]=S_And_E[i][1]*Coef;
            egr[i]=0.01;

        }

        TFile *f =new TFile("TESTM3G.root","recreate");
        TGraphErrors *gr =new TGraphErrors(Dim_LS,Egr,Sgr,egr,egr);

        gr->Write();
        f->Close();


            return S_And_E;

    }


}
















TObjArray *Build_LS::Get_Double_Gated_Intensities(TString Name_Nuclei,Float_t Gate1,Float_t Gate2,Float_t Energy_Ref,Float_t Height,Color_t Color,TGraphErrors *Graph_FWHM,Bool_t SAME)

{


}

ClassImp(Build_LS);




