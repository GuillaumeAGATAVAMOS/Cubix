#include "includes_cube.h"

#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TFile.h"
#include "TString.h"
#include "TBrowser.h"
#include "TMath.h"
#include "TTree.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TH1F.h"
#include "TMatrix.h"
#include "TArrayD.h"
#include "TMatrixDBase.h"
#include "TVectorT.h"
#include "TMatrixDLazy.h"
#include "TVectorD.h"




#include "Build_LS.h"
#include "ENSDF_DATAe680.h"
#include "Cube_Player.h"
#include "SpreadIntensityMatrix.h"
#include "Spectrum_Loader.h"

#ifndef SPECTRUM_LOADER_H
#endif

using namespace std;
using namespace Gw;

typedef std::vector<Float_t> Float_vec_t;
Float_vec_t Gate_Return;
Float_vec_t Gate_AND_Return;


Spectrum_Loader::Spectrum_Loader()

{
    fGate_Width=0.5;
}

Spectrum_Loader::Spectrum_Loader(const char* name, const char *title) :
        TNamed(name,title)
{
    fGate_Width=2.;

}



Spectrum_Loader::~Spectrum_Loader()
{

}

void Spectrum_Loader::Change_Gate_Width(Float_t Gate_Width)
{
    fGate_Width=Gate_Width;
}
Float_t Spectrum_Loader::Get_Gate_Width()
{
    return fGate_Width;
}


Double_t *Get_Back_PID(const char*Gate)
{

    Double_t * PID = new Double_t [2];

    //Get Back all the gates with "or" connection

    TString Delimiter=" ";
    TString CompareOption=TString(Gate);

    TObjArray *d = CompareOption.Tokenize(Delimiter);

    if(d->GetEntries()>1)
    {
    TString SecondOption = d->At(1)->GetName();

    delete d;

    Delimiter="+/-";

    TObjArray *dS = SecondOption.Tokenize(Delimiter);


    if(dS->GetEntries()!=0)
    {

        TString ZPID =dS->At(0)->GetName();

        TString ZPIDError =dS->At(1)->GetName();

        PID[0]=atof(ZPID);
        PID[1]=atof(ZPIDError);

//        cout<<" PID "<<atof(ZPID)<<" error PID "<<ZPIDError<<endl;

    }
    }
    else
    {
    PID[0]=50;PID[1]=100;
    }
    //cout<<" PID "<<PID[0]<<" error PID "<<PID[1]<<endl;

    return PID;
}



Double_t *Get_Back_IDS(const char*Quality_Factor)
{

    Double_t * QF = new Double_t [2];

    //Get Back QF for Z_ID And M_ID

    TString Delimiter="/";
    TString CompareOption=TString(Quality_Factor);

    TObjArray *d = CompareOption.Tokenize(Delimiter);
    QF[0]=atof(d->At(0)->GetName());
    QF[1]=0.5;//default value
    if(d->GetEntries()>1)
    {
        TString SecondOption = d->At(1)->GetName();
        QF[1]=atof(d->At(1)->GetName());

        delete d;
    }


    return QF;
}

Double_t *Get_Back_IDS_S_P(const char*Quality_Factor)
{

    Double_t * QF = new Double_t [4];
    QF[0]=0.5;QF[1]=0.5;QF[2]=0.5;QF[3]=0.5;//Default values


    TString Delimiter=" ";
    TString CompareOption=TString(Quality_Factor);

    TObjArray *d = CompareOption.Tokenize(Delimiter);

    if(d->GetEntries()>1)
    {
        TString SecondOption = d->At(1)->GetName();

        delete d;

        //Get Back QF for Z_ID And M_ID

        Delimiter="/";
        TString CompareOption_Spectra=TString(SecondOption);
        TObjArray *d_Spectra = CompareOption_Spectra.Tokenize(Delimiter);

        Delimiter="+";

        Int_t k=0;
        for(Int_t i=0;i<d_Spectra->GetEntries();i++)
        {

            TString SecondOption_Spectra_i = d_Spectra->At(i)->GetName();
            TString CompareOption_Spectra=TString(SecondOption_Spectra_i);
            TObjArray *d_Spectra_i = CompareOption_Spectra.Tokenize(Delimiter);
            for(Int_t j=0;j<d_Spectra_i->GetEntries();j++)
            {

                QF[k]=atof(d_Spectra_i->At(j)->GetName());
                //cout<<"QF["<<k<<"]  "<<atof(d_Spectra_i->At(j)->GetName())<<endl;
            k++;
            }


        }


    }

    return QF;
}




Float_vec_t Get_Back_Gates(const char*Gate)
{

    Gate_Return.clear();

    //Get Back all the gates with "or" connection

    TString Delimiter=" ";
    TString CompareOption=TString(Gate);

    TObjArray *d = CompareOption.Tokenize(Delimiter);

    TString SecondOption = d->At(1)->GetName();

    delete d;

    Delimiter="+";

    TObjArray *dS = SecondOption.Tokenize(Delimiter);

    if(dS->GetEntries()!=0)
    {

        TString S =dS->At(0)->GetName();

        cout<<"**************************"<<endl;
        cout<<"Maximum 10 'or' Gates"<<endl;
        cout<<"Gate(s) "<<S;
        Gate_Return.push_back(atof(S));

        for(Int_t k=1;k<dS->GetEntries();k++)
        {
            S =dS->At(k)->GetName();

            Gate_Return.push_back(atof(S));

            cout<<" ou "<<S;
        }
        cout<<endl;
        cout<<"**************************"<<endl;
        cout<<endl;

    }

    return Gate_Return;
}


TMatrixD Get_Back_AND_Gates(const char*Gate)
{

    TMatrixD Gate_AND_Return(3,10);

    TString Delimiter=" ";
    TString CompareOption=TString(Gate);

    TObjArray *d = CompareOption.Tokenize(TString(Delimiter));

    TString SecondOption=d->At(1)->GetName();

    delete d;

    //Delimit Gates separated with "AND" -> "∕"

    Delimiter="/";

    TObjArray *dS = SecondOption.Tokenize(TString(Delimiter));

    if(dS->GetEntries()!=0)
    {

        TString S_OR =dS->At(0)->GetName();

        cout<<"**************************"<<endl;
        cout<<"Maximum 10 'or' Gates Separated with AND Connection"<<endl;
        cout<<"Gate(s) "<<S_OR;


        Delimiter="+";

        TObjArray *d_OR = S_OR.Tokenize(TString(Delimiter));


        for(Int_t m=0;m<d_OR->GetEntries();m++)

        {
            Gate_AND_Return[0][m]=(atof(d_OR->At(m)->GetName()));
        }


        for(Int_t k=1;k<dS->GetEntries();k++)
        {
            TString S_OR_2 =dS->At(k)->GetName();

            TObjArray *d_OR_2 = S_OR_2.Tokenize(TString(Delimiter));

            for(Int_t m=0;m<d_OR_2->GetEntries();m++)

            {

                Gate_AND_Return[k][m]=(atof(d_OR_2->At(m)->GetName()));

            }

            cout<<" ET "<<S_OR_2;

            delete d_OR_2;
        }
        cout<<endl;
        cout<<"**************************"<<endl;
        cout<<endl;

    }

    // Gate_AND_Return.Print();

    delete dS;
    return Gate_AND_Return;

}


Int_t Spectrum_Loader::Which_Gate_Mode(const char*Gate)
{

    Int_t Gate_Mode=11;

    TString Delimiter=" ";
    TString CompareOption=TString(Gate);

    TObjArray *d = CompareOption.Tokenize(TString(Delimiter));

    TObjString *T =(TObjString*)d->At(0);

    const char *True_Option =(const char*)T->GetString();

    if((TString)True_Option=="SM"){Gate_Mode=0;}//Simple gated with Tree per M
    if((TString)True_Option=="SI"){Gate_Mode=1;}//Simple gated with Tree per (Z,M)
    if((TString)True_Option=="DM"){Gate_Mode=2;}
    if((TString)True_Option=="T"){Gate_Mode=3;}
    if((TString)True_Option=="N"){Gate_Mode=4;}//no gates
    if((TString)True_Option=="E_Z"){Gate_Mode=5;}//E versus Z mode
    if((TString)True_Option=="E_M"){Gate_Mode=6;}//E versus Z mode
    if((TString)True_Option=="B_M"){Gate_Mode=7;}//Raw spectra for a given mass
    if((TString)True_Option=="S_S"){Gate_Mode=8;}//you want to sbstract the current spectra by another spectra // to complete
    if((TString)True_Option=="SA"){Gate_Mode=9;}//Simple gate just with AGATA
    if((TString)True_Option=="PIDM"){Gate_Mode=10;}//PID versus M Mode (Matrix mode)
     if((TString)True_Option=="S_P"){Gate_Mode=11;}//Search polluant Mode

    //cout<<"Gate_Mode "<<Gate_Mode<<endl;

    return Gate_Mode;

}

TH1F *Spectrum_Loader::Plot_Isotope(Int_t Zz,Int_t M,const char * ID_R,ULong64_t Max_Read_Entries)
{

    TString ZString= Form("Z=%d",Zz);
    TString S_z=Z[Zz-Z_Offset];

    //get back the two quality factors (QF[0] pour Z_ID et QF[1] pour M_ID)
    Double_t *QF = new Double_t[2];
    QF=Get_Back_IDS(ID_R);

    cout<<" QF 0"<<QF[0]<<" QF 1 "<<QF[1]<<endl;


    TString Tree_Name="";Tree_Name+=M;Tree_Name+=S_z;Tree_Name+="_Z_";Tree_Name+=Zz;Tree_Name+="_A";Tree_Name+=M;

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerIsotope");


    TDirectoryFile *Z_Folder =(TDirectoryFile *)ISOtopes->Get(ZString);


    fCurrentTree = (TTree *)Z_Folder->Get(Tree_Name);

    // fCurrentTree->Print();

    Tree_Name+="Z_ID =";Tree_Name+=QF[0];Tree_Name+="M_ID =";Tree_Name+=QF[1];

    ffOutputSpectra = new TH1F(Tree_Name,Tree_Name,2000,0,2000);

    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t Z_ID=0;
    Float_t M_ID=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("Z_ID", &Z_ID);
    fCurrentTree->SetBranchAddress("M_ID", &M_ID);



    //*********Set branch Adress of Tree********************


    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

    cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



    while ( c_in_tree < entries_in_tree )
    {


        if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
        {
            cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
        }


        fCurrentTree->GetEntry(c_in_tree);
        if(Z_ID<QF[0] && M_ID< QF[1])
        {
            for (Int_t i =0; i<MGamma; i++)
            {
                ffOutputSpectra->Fill(EGamma[i]);
            }
        }
        if(c_in_tree>Max_Read_Entries)break;
        c_in_tree++;
    }


    //ffOutputSpectra->Draw();

    return ffOutputSpectra;
    cout<<endl;
}


TH1F *Spectrum_Loader::Plot_Isotope_Gated(Int_t Zz,Int_t M,const char *ID_R,const char *Gate,Float_t Gate_W,ULong64_t Max_Read_Entries)
{

    //Method for plotting gated spectra from a conditioned Z,M TREE.


    fGate_Width=Gate_W;
    TString ZString= Form("Z=%d",Zz);
    TString S_z=Z[Zz-Z_Offset];

    //get back the two quality factors (QF[0] pour Z_ID et QF[1] pour M_ID)
    Double_t *QF = new Double_t[2];
    QF=Get_Back_IDS(ID_R);


    TString Tree_Name="";Tree_Name+=M;Tree_Name+=S_z;Tree_Name+="_Z_";Tree_Name+=Zz;Tree_Name+="_A";Tree_Name+=M;


    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerIsotope");


    TDirectoryFile *Z_Folder =(TDirectoryFile *)ISOtopes->Get(ZString);


    fCurrentTree = (TTree *)Z_Folder->Get(Tree_Name);

    Tree_Name+="Z_ID =";Tree_Name+=QF[0]; Tree_Name+="M_ID =";Tree_Name+=QF[1];

    ffOutputSpectra = new TH1F(Tree_Name,Tree_Name,2000,0,2000);

    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t Z_ID=0;
    Float_t M_ID=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("Z_ID", &Z_ID);
        fCurrentTree->SetBranchAddress("M_ID", &M_ID);

    //*********Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

     cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



    //*****************************************SIMPLE GATED SPECTRA******************************************

    if(Which_Gate_Mode(Gate)==1)//Simple Getd Spectra with Z,M

    {

        Float_vec_t g1=Get_Back_Gates(Gate); //g1 is a combination of several gates with logical link "or"

        while ( c_in_tree < entries_in_tree )
        {


            if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
            {
                cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
            }


            fCurrentTree->GetEntry(c_in_tree);
            if(Z_ID<QF[0] && M_ID< QF[1])
            {

                //**************************One Gate Case*******************************

                Int_t i, nb_in, Flag[200];

                nb_in = 0;
                for (i = 0; i < MGamma; i++ )
                { // count how many gammas in the gate g1

                    Flag[i] = 0;
                    for(Int_t j=0;j<(Int_t)g1.size();j++)
                    {
                        if ( EGamma[i] >= g1.at(j)-fGate_Width &&  EGamma[i]<= g1.at(j)+fGate_Width ) { nb_in++; Flag[i] = 1; }
                    }

                }
                // i
                if ( nb_in == 0 ){c_in_tree++;continue;}// no gammas in the gate
                if ( nb_in == 1 )
                { 	// keep only gammas outside the gate
                    for (i = 0; i < MGamma; i++ )
                    {
                        if ( Flag[i] == 0 ) ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }
                else
                {			// keep all gammas
                    for (i = 0; i < MGamma; i++ )
                    {
                        ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }

                //***************End One Gate Case*****************************************

            }
            if(c_in_tree>Max_Read_Entries)break;
            c_in_tree++;
        }

    }
    //*****************************************END SIMPLE GATED SPECTRA******************************************

    cout<<endl;
    return ffOutputSpectra;


}







TH1F *Spectrum_Loader::Plot_Gated_Spectra(Int_t Zz,Int_t M,const char * ID_R,const char *Gate,Float_t Gate_W,ULong64_t Max_Read_Entries)
{

    Double_t *QF = new Double_t[2];
    QF=Get_Back_IDS(ID_R);

    fGate_Width=Gate_W;

    TString Tree_Name= Form("%d",M);

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerM");

    fCurrentTree = (TTree *)ISOtopes->Get(Tree_Name);

    Tree_Name+="_Gate_";Tree_Name+=Gate;
    Tree_Name+=" M_ID =";Tree_Name+=QF[0];

    ffOutputSpectra = new TH1F(Tree_Name,Tree_Name,2000,0,2000);


    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t M_ID=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("M_ID", &M_ID);

    //*********Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

     cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;




    //*****************************************SIMPLE GATED SPECTRA******************************************

    if(Which_Gate_Mode(Gate)==0)//Simple Gated Spectra

    {

        Float_vec_t g1=Get_Back_Gates(Gate); //g1 is a combination of several gates with logical link "or"

        while ( c_in_tree < entries_in_tree )
        {

            if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
            {
                cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
            }


            fCurrentTree->GetEntry(c_in_tree);
            if(M_ID<QF[0])
            {

                //**************************One Gate Case*******************************

                Int_t i, nb_in, Flag[200];

                nb_in = 0;
                for (i = 0; i < MGamma; i++ )
                { // count how many gammas in the gate g1

                    Flag[i] = 0;
                    for(Int_t j=0;j<(Int_t)g1.size();j++)
                    {
                        if ( EGamma[i] >= g1.at(j)-fGate_Width &&  EGamma[i]<= g1.at(j)+fGate_Width ) { nb_in++; Flag[i] = 1; }
                    }

                }
                // i
                if ( nb_in == 0 ){c_in_tree++;continue;}// no gammas in the gate
                if ( nb_in == 1 )
                { 	// keep only gammas outside the gate
                    for (i = 0; i < MGamma; i++ )
                    {
                        if ( Flag[i] == 0 ) ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }
                else
                {			// keep all gammas
                    for (i = 0; i < MGamma; i++ )
                    {
                        ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }

                //***************End One Gate Case*****************************************

            }
            if(c_in_tree>Max_Read_Entries)break;
            c_in_tree++;
        }

    }
    //*****************************************END SIMPLE GATED SPECTRA******************************************


    cout<<endl;
    return ffOutputSpectra;

}


TH1F *Spectrum_Loader::Plot_Double_Gated_Spectra(Int_t Zz,Int_t M,const char * ID_R,const char *Gate,Float_t Gate_W,ULong64_t Max_Read_Entries)
{


    Double_t *QF = new Double_t[2];
    QF=Get_Back_IDS(ID_R);

    TMatrix Gates(3,10);
    Gates=Get_Back_AND_Gates(Gate);

    fGate_Width=Gate_W;

    TString Tree_Name= Form("%d",M);

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerM");

    fCurrentTree = (TTree *)ISOtopes->Get(Tree_Name);

    TString Spec_Name1 =Tree_Name;
    Spec_Name1 +=Gate;
    Spec_Name1 += Form("M_ID = %4.2f",QF[0]);

    ffOutputSpectra = new TH1F(Tree_Name,Tree_Name,2000,0,2000);


    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t M_ID=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("M_ID", &M_ID);

    //*********Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

    cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



    //*****************************************SIMPLE GATED SPECTRA******************************************

    if(Which_Gate_Mode(Gate)==2)//Double Gated Spectra with TREE per M

    {

        Float_vec_t g1;//g1 is a combination of several gates with logical link "or"
        for(Int_t i=0;i<Gates.GetNcols();i++)
        {
            if(Gates[0][i]!=0)g1.push_back(Gates[0][i]);
        }

        Float_vec_t g2;//g2 is a combination of several gates with logical link "or"

        for(Int_t i=0;i<Gates.GetNcols();i++)
        {
            if(Gates[1][i]!=0)g2.push_back(Gates[1][i]);
        }

        //        cout<<" g1 "<<g1.at(0)<<" "<<g1.at(1)<<endl;

        //        cout<<" g22222 "<<g2.at(0)<<" "<<g2.at(1)<<endl;
        //      cout<<" Size "<<g1.size()<<"  "<<g2.size()<<endl;

        while ( c_in_tree < entries_in_tree )
        {

            if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
            {
                cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
            }


            fCurrentTree->GetEntry(c_in_tree);
            if(M_ID<QF[0])
            {

                //**************************TWO Gate Case*******************************

                Int_t i, nb_in1, Flag1[200],nb_in2, Flag2[200];

                nb_in1 = 0;
                nb_in2=0;

                for (i = 0; i < MGamma; i++ )
                { // count how many gammas in the gate g1

                    Flag1[i] = 0;
                    for(Int_t j=0;j<(Int_t)g1.size();j++)
                    {
                        if ( EGamma[i] >= g1.at(j)-fGate_Width &&  EGamma[i]<= g1.at(j)+fGate_Width ) { nb_in1++; Flag1[i] = 1; }
                    }

                }

                for (i = 0; i < MGamma; i++ )
                { // count how many gammas in the gate g2

                    Flag2[i] = 0;
                    for(Int_t j=0;j<(Int_t)g2.size();j++)
                    {
                        if ( EGamma[i] >= g2.at(j)-fGate_Width &&  EGamma[i]<= g2.at(j)+fGate_Width ) { nb_in2++; Flag2[i] = 1; }
                    }

                }

                if ( nb_in1 == 0 || nb_in2 == 0 ){c_in_tree++;continue;}// no gammas in the gate
                if ( nb_in1 == 1 && nb_in2 ==1 )
                { 	// keep only gammas outside the gate
                    for (i = 0; i < MGamma; i++ )
                    {
                        if ( Flag1[i] == 0  && Flag2[i] ==0) ffOutputSpectra->Fill(EGamma[i]);
                    }
                }
                else
                {			// keep all gammas
                    for (i = 0; i < MGamma; i++ )
                    {
                        ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }

                //***************End TWO Gate Case*****************************************

            }
            if(c_in_tree>Max_Read_Entries)break;
            c_in_tree++;
        }

    }
    //*****************************************END SIMPLE GATED SPECTRA******************************************


    cout<<endl;
    return ffOutputSpectra;

}



TH2F *Spectrum_Loader::Plot_E_Z(Int_t Zz,Int_t M,const char * ID_R,Float_t Z_Min,Float_t Z_Max,ULong64_t Max_Read_Entries)
{

    //Method for plotting matrix e versus Z

    Double_t *QF = new Double_t[2];
    QF=Get_Back_IDS(ID_R);

    TString Tree_Name= Form("%d",M);

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerM");

    fCurrentTree = (TTree *)ISOtopes->Get(Tree_Name);

    TString Spec_Name1 =Tree_Name;
    Spec_Name1 += Form("M_ID = %4.2f",QF[0]);

    f2OutputSpectra = new TH2F(Spec_Name1,Spec_Name1,Int_t(Z_Max-Z_Min)*40,Z_Min,Z_Max,2000,0,2000);


    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t M_ID=0;
    Float_t Z=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("M_ID", &M_ID);
    fCurrentTree->SetBranchAddress("Z", &Z);


    //*********Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

     cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



    while ( c_in_tree < entries_in_tree )
    {

        if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
        {
            cout <<"\e[1;93m"<< "*" <<"\e[0m"<<flush;
            //cout<<" c_in_tree "<<c_in_tree<<endl;
        }


        fCurrentTree->GetEntry(c_in_tree);
        if(M_ID<=QF[0])
        {

            for (Int_t i = 0; i < MGamma; i++ )
            {
                f2OutputSpectra->Fill(Z,EGamma[i]);
            }

            if(c_in_tree>Max_Read_Entries)break;

        }

            c_in_tree++;
    }
    cout<<endl;
    f2OutputSpectra->SetName(Form("ISOBARES %d",M));
    f2OutputSpectra->SetTitle(Form("ISOBARES %d",M));

    return f2OutputSpectra;

}
TH2F *Spectrum_Loader::Plot_E_M(Int_t Zz,Int_t M,const char * ID_R,Float_t M_Min,Float_t M_Max,ULong64_t Max_Read_Entries)
{

    //Method for plotting gated spectra from a conditioned Z,M TREE.

    Double_t *QF = new Double_t[2];
    QF=Get_Back_IDS(ID_R);


    TString Tree_Name= Form("%d",Zz);

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerZ");

    fCurrentTree = (TTree *)ISOtopes->Get(Tree_Name);

    TString Spec_Name1 =Tree_Name;
    Spec_Name1 += Form("Z_ID = %4.2f",QF[0]);

    f2OutputSpectra = new TH2F(Spec_Name1,Spec_Name1,Int_t(M_Max-M_Min)*100,M_Min,M_Max,2000,0,2000);


    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t Z_ID=0;
    Float_t Mm=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("Z_ID", &Z_ID);
    fCurrentTree->SetBranchAddress("M", &Mm);


    //*********Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

    cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;


    while ( c_in_tree < entries_in_tree )
    {

        if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
        {
            cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
        }


        fCurrentTree->GetEntry(c_in_tree);
        if(Z_ID<QF[0])
        {

            for (Int_t i = 0; i < MGamma; i++ )
            {
                f2OutputSpectra->Fill(Mm,EGamma[i]);
            }

            if(c_in_tree>Max_Read_Entries)break;

        }
            c_in_tree++;

    }
    cout<<endl;

    f2OutputSpectra->SetName(Form("ISOTOPES %d",Zz));
    f2OutputSpectra->SetTitle(Form("ISOTOPES %d",Zz));

    return f2OutputSpectra;

}


TH1F *Spectrum_Loader::Plot_B_M(Int_t Zz,Int_t M,const char * PID,ULong64_t Max_Read_Entries)
{

    //Method for plotting RAW spectra from a conditioned M  TREE.


    Double_t *PIDIn =new Double_t[2];

    PIDIn=Get_Back_PID(PID);

    TString Tree_Name= Form("%d",M);

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerM");

    fCurrentTree = (TTree *)ISOtopes->Get(Tree_Name);

    Tree_Name+=" PID =";Tree_Name+=PIDIn[0];

    TString Spec_Name1 =Tree_Name;
    Spec_Name1 += Form("PID = %4.2f ",PIDIn[0]);

    ffOutputSpectra = new TH1F(Spec_Name1,Spec_Name1,2000,0,2000);

    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t PIDTree=0;
    Float_t Z=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("PID", &PIDTree);


    //*********END Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

     cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



    while ( c_in_tree < entries_in_tree )
    {

        if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
        {
            cout <<"\e[1;93m"<< "*" <<"\e[0m"<<flush;
        }

        fCurrentTree->GetEntry(c_in_tree);

        if(PIDTree>=(PIDIn[0]-PIDIn[1]) && PIDTree<=(PIDIn[0]+PIDIn[1]))
        {

            for (Int_t i = 0; i < MGamma; i++ )
            {
                ffOutputSpectra->Fill(EGamma[i]);
            }

            if(c_in_tree>Max_Read_Entries)break;

        }

            c_in_tree++;
    }

    cout<<endl;

            //cout<<" PID "<<PIDIn[0]<<" error PID "<<PIDIn[1]<<endl;
    ffOutputSpectra->SetName(Form("Raw Spectra For Mass = %d, PID = %i +/- %0.01f",M,PIDIn[0],PIDIn[1]));
    ffOutputSpectra->SetTitle(Form("Raw Spectra For Mass = %d, PID = %i +/- %0.01f",M,PIDIn[0],PIDIn[1]));

    //delete PIDIn;

    return ffOutputSpectra;

}




TH1F *Spectrum_Loader::Plot_All_Gated_Spectra(const char *Gate,Float_t Gate_W,ULong64_t Max_Read_Entries)
{

    fGate_Width=Gate_W;

    TString Tree_Name="E680Tree";

    fTree=TFile::Open(E_AGATA_DIRECTORY);

    fCurrentTree = (TTree *)fTree->Get(Tree_Name);

    Tree_Name+="_Gate_";Tree_Name+=Gate;

    ffOutputSpectra = new TH1F(Tree_Name,Tree_Name,2000,0,2000);


    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);

    //*********Set branch Adress of Tree********************



    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

     cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;




    //*****************************************SIMPLE GATED SPECTRA******************************************

    if(Which_Gate_Mode(Gate)==9)//Simple Gated Spectra just with AGATA

    {

        Float_vec_t g1=Get_Back_Gates(Gate); //g1 is a combination of several gates with logical link "or"

        while ( c_in_tree < entries_in_tree )
        {

            if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
            {
                cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
                //cout<<"c_in_tree "<<c_in_tree<<endl;
            }


            fCurrentTree->GetEntry(c_in_tree);


                //**************************One Gate Case*******************************

                Int_t i, nb_in, Flag[200];

                nb_in = 0;
                for (i = 0; i < MGamma; i++ )
                { // count how many gammas in the gate g1

                    Flag[i] = 0;
                    for(Int_t j=0;j<(Int_t)g1.size();j++)
                    {
                        if ( EGamma[i] >= g1.at(j)-fGate_Width &&  EGamma[i]<= g1.at(j)+fGate_Width ) { nb_in++; Flag[i] = 1; }
                    }

                }
                // i
                if ( nb_in == 0 ){c_in_tree++;continue;}// no gammas in the gate
                if ( nb_in == 1 )
                { 	// keep only gammas outside the gate
                    for (i = 0; i < MGamma; i++ )
                    {
                        if ( Flag[i] == 0 ) ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }
                else
                {			// keep all gammas
                    for (i = 0; i < MGamma; i++ )
                    {
                        ffOutputSpectra->Fill(EGamma[i]);
                    } // i
                }

                //***************End One Gate Case*****************************************


                c_in_tree++;
                if(c_in_tree>Max_Read_Entries)break;

        }

    }
    //*****************************************END SIMPLE GATED SPECTRA******************************************


    cout<<endl;
    return ffOutputSpectra;

}


TH1F *Spectrum_Loader::Substract_Two_Spectra(TH1F* Old_Spectra,TH1F* Current_Spectra,Float_t Coef)

{
    TString Name_new_Spectra=Old_Spectra->GetName();Name_new_Spectra+=Form("%0.01f*",Coef);Name_new_Spectra+=Current_Spectra->GetName();

    TH1F * New_Return_Spectra= new TH1F(Name_new_Spectra,Name_new_Spectra,2000,0,2000);
    Double_t yold;
    Double_t ynew;

    for(Int_t i=0;i<Old_Spectra->GetNbinsX();i++)
    {

    ynew=Old_Spectra->GetBinContent(i)+Coef*Current_Spectra->GetBinContent(i);

    New_Return_Spectra->Fill(i,ynew);

    }
    return New_Return_Spectra;
}


TH2F *Spectrum_Loader::Plot_PID_M(Int_t M_Current,const char *Gate,unsigned int Max_Entries)

{

        TString Tree_Name= Form("%d",M_Current);

        fTree=TFile::Open(Tree_Directory);

        TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerM");

        fCurrentTree = (TTree *)ISOtopes->Get(Tree_Name);

        Tree_Name+=" PID vs M =";Tree_Name+=M_Current;

        f2OutputSpectra = new TH2F(Tree_Name,Tree_Name,200,M_Current-0.5,M_Current+0.5,400,25,45);

        //*********Set branch Adress of Tree********************

        Float_t PIDTree=0;
        Float_t M=0;

        fCurrentTree->SetBranchAddress("PID", &PIDTree);
        fCurrentTree->SetBranchAddress("M", &M);


        //*********END Set branch Adress of Tree********************



        ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

         cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
        cout<<endl;

        cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
        cout<<endl;
        cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
        cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



        while ( c_in_tree < entries_in_tree )
        {

            if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
            {
                cout <<"\e[1;93m"<< "*" <<"\e[0m"<<flush;
            }

            fCurrentTree->GetEntry(c_in_tree);


                    f2OutputSpectra->Fill(M,PIDTree);

                if(c_in_tree>Max_Entries)break;


                c_in_tree++;
        }

        cout<<endl;

        f2OutputSpectra->SetName(Form("PID versus M=%d",M_Current));
        f2OutputSpectra->SetTitle(Form("PID versus M=%d",M_Current));


        return f2OutputSpectra;






}


TObjArray *Spectrum_Loader::Search_Polluants(Int_t Zz,Int_t M,const char * ID_R,ULong64_t Max_Read_Entries)
{

    TString ZString= Form("Z=%d",Zz);
    TString S_z=Z[Zz-Z_Offset];

    Array_Of_Spectra= new TObjArray();

    //get back the four quality factors
    //(QF[0] pour Z_ID et QF[1] pour M_ID)//first spectra
    //(QF[2] pour Z_ID et QF[3] pour M_ID)//second spectra
    Double_t *QF = new Double_t[4];
    QF=Get_Back_IDS_S_P(ID_R);

//    cout<<" QF 0 "<<QF[0]<<" QF 1 "<<QF[1]<<endl;

//    cout<<" QF 2 "<<QF[2]<<" QF 3 "<<QF[3]<<endl;


    TString Tree_Name="";Tree_Name+=M;Tree_Name+=S_z;Tree_Name+="_Z_";Tree_Name+=Zz;Tree_Name+="_A";Tree_Name+=M;

    fTree=TFile::Open(Tree_Directory);

    TDirectoryFile *ISOtopes =(TDirectoryFile *)fTree->Get("TreePerIsotope");


    TDirectoryFile *Z_Folder =(TDirectoryFile *)ISOtopes->Get(ZString);


    fCurrentTree = (TTree *)Z_Folder->Get(Tree_Name);

    // fCurrentTree->Print();

    TString Spec_Name1 =Tree_Name;
    Spec_Name1 += Form("Z_ID = %4.2f, M_ID = %4.2f",QF[0],QF[1]);

    TString Spec_Name2 =Tree_Name;
    Spec_Name2 += Form("Z_ID = %4.2f, M_ID = %4.2f",QF[2],QF[3]);


    ffOutputSpectra = new TH1F(Spec_Name1,Spec_Name1,2000,0,2000);
    f_2OutputSpectra = new TH1F(Spec_Name2,Spec_Name2,2000,0,2000);

    ffOutputSpectra->SetLineColor(kRed);
    f_2OutputSpectra->SetLineColor(kBlue);

    //*********Set branch Adress of Tree********************

    Float_t EGamma[20]={0};
    Float_t Z_ID=0;
    Float_t M_ID=0;
    Byte_t MGamma;

    fCurrentTree->SetBranchAddress("MGamma", &MGamma);
    fCurrentTree->SetBranchAddress("EGamma", EGamma);
    fCurrentTree->SetBranchAddress("Z_ID", &Z_ID);
    fCurrentTree->SetBranchAddress("M_ID", &M_ID);



    //*********Set branch Adress of Tree********************


    ULong64_t entries_in_tree = fCurrentTree->GetEntries(), c_in_tree = 0;

    cout<<"\e[1;93m"<<"             ENTRIES IN TREE "<<entries_in_tree<<"\e[0m"<<endl;
    cout<<endl;

    cout<<"\e[1;93m"<<"                      ProGress                        "<<"\e[0m"<<endl;
    cout<<endl;
    cout <<"\e[1;93m"<< "0%   10   20   30   40   50   60   70   80   90 100%"<<"\e[0m"<<endl;
    cout <<"\e[1;93m"<< "|----|----|----|----|----|----|----|----|----|----| "<<"\e[0m"<<endl;



    while ( c_in_tree < entries_in_tree )
    {


        if(c_in_tree%(Int_t)(entries_in_tree/50.)==0)
        {
            cout <<"\e[1;93m"<< "*" <<"\e[0m"<< flush;
        }


        fCurrentTree->GetEntry(c_in_tree);
        if(Z_ID<QF[0] && M_ID< QF[1])
        {
            for (Int_t i =0; i<MGamma; i++)
            {
                ffOutputSpectra->Fill(EGamma[i]);
            }
        }

        if(Z_ID<QF[2] && M_ID< QF[3])
        {
            for (Int_t i =0; i<MGamma; i++)
            {
                f_2OutputSpectra->Fill(EGamma[i]);
            }
        }

        if(c_in_tree>Max_Read_Entries)break;
        c_in_tree++;
    }


    Double_t norm = 1;
    Double_t scale = norm/(ffOutputSpectra->Integral());
    ffOutputSpectra->Scale(scale);

    Double_t scale2 = norm/(f_2OutputSpectra->Integral());
    f_2OutputSpectra->Scale(scale2);

    Array_Of_Spectra->Add(ffOutputSpectra);
    Array_Of_Spectra->Add(f_2OutputSpectra);

    return Array_Of_Spectra;
    cout<<endl;
}




ClassImp(Spectrum_Loader);
