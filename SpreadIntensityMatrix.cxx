#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "TMatrixD.h"
#include "TMath.h"
#include "TMatrix.h"
#include "TArrayD.h"
#include "TMatrixDBase.h"
#include "TVectorT.h"
#include "TMatrixDLazy.h"
#include "TVectorD.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"

#include "SpreadIntensityMatrix.h"

using namespace std;
using namespace Gw;

SpreadIntensityMatrix::SpreadIntensityMatrix()
{
}
SpreadIntensityMatrix::~SpreadIntensityMatrix()
{

}
SpreadIntensityMatrix::SpreadIntensityMatrix(const char* name, const char *title) : TNamed (name,title)
{
    fCorr_Above=1.;
    fSGate=1.;
}

Float_t SpreadIntensityMatrix::Get_Corr_Above()
{
    return fCorr_Above;
}
Float_t SpreadIntensityMatrix::Get_S_Gate()
{
    return fSGate;
}

TMatrixD SpreadIntensityMatrix::Spread_Below(Float_t S_Gate)
{

    fSGate=S_Gate;
    //Set Test Matrix

    cout<<"BEGIN TEST MATRIX"<<endl;

    TMatrixD Mat(5,5);
    TMatrixD TMat(5,5);

    TArrayD data(25);
    TArrayD Firstdata(5);

    Firstdata[0] = 0;Firstdata[1] = 10.;Firstdata[2] =70.;Firstdata[3] = 0;Firstdata[4] = 0;

    data[0] = 0;data[1] = 10.;data[2] =70.;data[3] = 0;data[4] = 0;
    data[5] = 0;data[6] = 0;data[7] = 4.;data[8] = 19.;data[9] = 0;
    data[10] = 0;data[11] = 0;data[12] = 0;data[13] = 0;data[14] = 80.;
    data[15] = 0;data[16] = 0;data[17] = 0;data[18] = 0;data[19] = 20.;
    data[20] = 0;data[21] = 0;data[22] = 0;data[23] = 0;data[24] = 0;

    Mat.SetMatrixArray(data.GetArray());
    TMat.Transpose(Mat);

    TMatrixD Final(Mat.GetNrows(),Mat.GetNcols());
    Final.Zero();

    TMatrixD FData(1,Mat.GetNcols());
    FData.SetMatrixArray(Firstdata.GetArray());

    Final.SetSub(0,0,FData);

    Mat.Print();

    //End Set Matrix

    for(Int_t i=1;i<Mat.GetNrows()-1;i++)
    {


        cout<<"******************** New Iteration**************************"<<endl;
        cout<<"                     I = "<<i<<"                            "<<endl;

        //Define the Vector of the Current Level

        TMatrixD V(Mat.GetNrows(),1);

        TArrayD dataV(Mat.GetNrows());

        for(Int_t j=0;j<Mat.GetNrows();j++){dataV[j]=0;}
        dataV[i]=1;
        V.SetMatrixArray(dataV.GetArray());

        cout<<"-----------VECTOR ID---------"<<endl;
        V.Print();

        //Result Vector for all the intensity arriving on the current level

            TMatrixD C(1,Mat.GetNcols());
            C.Zero();
            if(i<=1)
            {
                C.Mult(Mat,V);
            }else
            {C.Mult(Final,V);}

            //C.Print();

        //Result Vector of the raw intensity leaving the current Level

        TMatrixD V2(1,Mat.GetNcols());
        if(i<=1)
        {
             V2.Mult(TMat,V);
        }else
        {
            V2.Mult(TMat,V);
        }

        //V2.Print();

        double Ibr=0;

        Ibr=V2.Sum();

        //The new intensity leaving the level

        TMatrixD VM(1,Mat.GetNcols());
        VM=(C.Sum()/Ibr)*V2;

        //VM.Print();

        // Add the new vector to the matrix
        Final.SetSub(i,0,VM);


        cout<<"FINAL"<<endl;cout<<endl;

        //Final.Print();

    }

        Final*=S_Gate;
        Final.Print();

        return Final;

}



void SpreadIntensityMatrix::Spread_Above(Float_t Corr_Intensity)
{

    //Corr_Intensity is egual to the branching ratio of the gate times the total raw intensity below the gate
    // Ex: Bratio of the gate 0.5
    //     Total intensity below the gate :70+10=80 =>0.8
    //Corr_Intensity =0.8*0.5

    fCorr_Above=Corr_Intensity;

    Float_t VISUM=0;
    Float_t VRAWSUM=0;
    Float_t VCSUM=0;

    //Set Test Matrix

    cout<<"BEGIN TEST MATRIX"<<endl;

    TMatrixD Mat(6,6);
    TMatrixD C(6,6);

    TArrayD data(36);
    TArrayD Corr(36);
    TArrayD Firstdata(5);

    Firstdata[0] = 0;Firstdata[1] = 60;Firstdata[2] =0;Firstdata[3] = 0;Firstdata[4] = 0;

    Corr[0]=1;Corr[7]=1;Corr[14]=0.666667;Corr[21]=1;Corr[28]=0.9;Corr[35]=1;

    data[0] = 0;data[1] = 60.;data[2] =0.;data[3] = 0;data[4] = 0;data[5] = 0;
    data[6] = 0;data[7] = 0;data[8] = 10.;data[9] = 40;data[10] = 0;data[11] = 0;
    data[12] = 0;data[13] = 0;data[14] = 0;data[15] = 2;data[16] = 10.;data[17] = 0;
    data[18] = 0;data[19] = 0;data[20] = 0;data[21] = 0;data[22] = 35;data[23] = 0;
    data[24] = 0;data[25] = 0;data[26] = 0;data[27] = 0;data[28] = 0;data[29] = 20;
    data[30] = 0;data[31] = 0;data[32] = 0;data[33] = 0;data[34] = 0;data[35] = 0;

    Mat.SetMatrixArray(data.GetArray());

    TMatrixD Final(Mat.GetNrows(),Mat.GetNcols());
    Final.Zero();

    TMatrixD FData(1,Mat.GetNcols());
    FData.SetMatrixArray(Firstdata.GetArray());


    //Mat.Print("RAW MATRIX");

    C.SetMatrixArray(Corr.GetArray());
    //C.Print();

    //END Set Matrix


    for(Int_t i=1;i<Mat.GetNrows();i++)
    {


        cout<<"******************** New Iteration**************************"<<endl;
        cout<<"                     I = "<<i<<"                            "<<endl;


        VISUM=0;
        VRAWSUM=0;
        VCSUM=0;

        //*******
        //Get the Matrix Corrected by the Current C matrix
        //*******
        TMatrixD HB(Mat.GetNrows(),Mat.GetNcols());

        HB.Mult(C,Mat);

        //HB.Print();


        //*******
        //Define the vector of the current Level
        //*******
        TMatrixD V(Mat.GetNrows(),1);

        TArrayD dataV(Mat.GetNrows());

        for(Int_t j=0;j<Mat.GetNrows();j++){dataV[j]=0;}
        dataV[i]=1;
        V.SetMatrixArray(dataV.GetArray());

        cout<<"-----------VECTOR ID---------"<<endl;
        //V.Print();


        //*******
        //Get the total intensity arriving on the current Level
        //*******

        TMatrixD VI(Mat.GetNrows(),1);
        VI.Zero();
        VI.Mult(HB,V);


        //VI.Print();
        VISUM=VI.Sum();
        cout<<"VI.Sum "<<VISUM<<endl;


        //*******
        //Get the total intensity arriving on the current Level from raw matrix
        //*******

        TMatrixD VRAW(Mat.GetNrows(),1);
        VRAW.Zero();
        VRAW.Mult(Mat,V);


        //VRAW.Print();
        VRAWSUM=VRAW.Sum();
        cout<<"VRAW.Sum "<<VRAWSUM<<endl;


        //*******
        //Get the Correction coef from Craw matrix
        //*******

        TMatrixD VC(Mat.GetNrows(),1);
        VC.Zero();
        VC.Mult(C,V);

        VCSUM=VC.Sum();

        //cout<<" VC Sum "<<VCSUM<<endl;


        //Calcul the new Coef for the C matrix :

        C[i][i]=(VISUM/VRAWSUM)*VCSUM;// to change by i


        //C.Print();

        Final=HB;

    }

    cout<<"------------FINAL MATRIX----------------"<<cout<<endl;cout<<endl;cout<<endl;
    Final*=Corr_Intensity;
    Final.Print();

}




TMatrixD SpreadIntensityMatrix::Get_Total_P(TMatrixD Mat_TS_Brute,const Int_t Size)
{

    TMatrixD I(Size,Size);
    I.UnitMatrix();

    TMatrixD P1(I,TMatrixD::kMinus,Mat_TS_Brute);


    TMatrixD P(P1.Invert(),TMatrixD::kMinus,I);


return P;

}

TMatrixD SpreadIntensityMatrix::Get_Final_M(TMatrixD P,TMatrix Fill_S,const Int_t Size)
{

    TMatrixD Final(Size,Size);


    for(Int_t i=0;i<Size;i++)
    {
        for(Int_t j=0;j<Size;j++)
        {

            Final[i][j]=Fill_S[i][0]*P[i][j];
        }

    }


return Final;

}

TMatrixD SpreadIntensityMatrix::Get_S_And_E(TMatrixD Final_Matrix,TMatrixD E_Matrix,Float_t Gate,const Int_t Size)
{

    TMatrixD Final_S_E(Size,2);


    Int_t k=0;
    for(Int_t i=0;i<Size;i++)
    {
        if(Gate==E_Matrix[i][0])break;
        k++;
    }

    if(k!=Size)

    {

        for(Int_t i=0;i<Size;i++)
        {
            Final_S_E[i][0]=E_Matrix[i][0];//the ernergy
            Final_S_E[i][1]=Final_Matrix[i][k]+Final_Matrix[k][i];//the strength
        }

    }

    return Final_S_E;

}


TMatrixD SpreadIntensityMatrix::Get_S_And_E_2(TMatrixD Mat_S,TMatrixD Mat_P,TMatrixD Final_Matrix,TMatrixD E_Matrix,Float_t Gate1,Float_t Gate2,const Int_t Size)
{

    TMatrixD Final_S_E(Size,2);//Final Matrix and below the lowest gate
    TMatrixD IB(Size,2);//matrix between
    TMatrixD IH(Size,2);//Above the higher gate

    Int_t k=0;
    for(Int_t i=0;i<Size;i++)
    {
        if(Gate1==E_Matrix[i][0])break;
        k++;
    }
    Int_t l =0;
    for(Int_t i=0;i<Size;i++)
    {
        if(Gate2==E_Matrix[i][0])break;
        l++;
    }

    //-------------------------------- COMMENTS ABOUT THE COMPUTATION------------------------------//

    //---Mat_S[k][0]->Raw Strength of the highest gate
    //---Mat_S[l][0]->Raw Strength of the Lowest gate
    //--Mat_P[k][l]-> Probility to make a decay Gate1->Gate2 (index of Gate1==k)(index of Gate2==l)
    //--Mat_P[i][l]-> Probability to decay to Gate2
    //--Mat_P[k][i]-> Probability to decay make a decay gate1->i



    if(k!=Size && l!=Size)

    {

        for(Int_t i=0;i<Size;i++)
        {
            Final_S_E[i][0]=E_Matrix[i][0];//the ernergy
            IB[i][0]=E_Matrix[i][0];


//            //*******************************************************OTHER METHOD*************************************************

//            // Below the lowest gate
//            if((Final_Matrix[i][l]+Final_Matrix[l][i])!=0) Final_S_E[i][1]=(Final_Matrix[l][i]);

//            //just between the first and the second gate

//            if(Final_Matrix[i][l] !=0 /*&& Mat_P[k][i]!=0*/){ IB[i][1]=Mat_P[i][l]*((Mat_P[k][i])*Mat_S[l][0])/Mat_P[k][l];}


//            //just above the second gate

//            if(Final_Matrix[i][k] !=0) IH[i][1]=(Final_Matrix[i][k])*Mat_S[l][0]/Mat_S[k][0];

//             //******************************************************* END OTHER METHOD*************************************************


            // Below the lowest gate
            if((Final_Matrix[i][l]+Final_Matrix[l][i])!=0) {Final_S_E[i][1]=(Mat_P[l][i]*Mat_S[l][0]*Mat_S[k][0]);}



            //just between the first and the second gate

            if(Final_Matrix[i][l] !=0 ){ IB[i][1]=(Mat_P[i][l]*Mat_P[k][i]*Mat_S[l][0]*Mat_S[k][0])/Mat_P[k][l];}



            //just above the second gate

            if(Final_Matrix[i][k] !=0) IH[i][1]=(Mat_P[i][k])*Mat_S[l][0]*Mat_S[i][0];


            //Add All

            Final_S_E[i][1]+=IB[i][1]+IH[i][1];

        }

    }

    return Final_S_E;

}



TMatrixD SpreadIntensityMatrix::Get_S_And_E_3(TMatrixD Mat_P,TMatrixD Final_Matrix,TMatrixD E_Matrix,Float_t Gate1,Float_t Gate2,Float_t Gate3,const Int_t Size)
{



    //-------------------------------- TO BE CONTINUED------------------------------//
    //--------------------------------- DO NOT WORK--------------------------------//

    TMatrixD Final_S_E(Size,2);

    Int_t k=0;
    for(Int_t i=0;i<Size;i++)
    {
        if(Gate1==E_Matrix[i][0])break;
        k++;
    }
    Int_t l =0;
    for(Int_t i=0;i<Size;i++)
    {
        if(Gate2==E_Matrix[i][0])break;
        l++;
    }
    Int_t m =0;
    for(Int_t i=0;i<Size;i++)
    {
        if(Gate3==E_Matrix[i][0])break;
        m++;
    }

    if(k!=Size && l!=Size && m!=Size)

    {

        for(Int_t i=0;i<Size;i++)
        {
            Final_S_E[i][0]=E_Matrix[i][0];//the ernergy
           if((Final_Matrix[i][l]+Final_Matrix[l][i])!=0 && (Final_Matrix[i][m]+Final_Matrix[m][i])!=0) Final_S_E[i][1]=(Final_Matrix[i][k]+Final_Matrix[k][i])*Mat_P[k][l]*Mat_P[k][m];//the strength
        }

    }


    return Final_S_E;

}




ClassImp(SpreadIntensityMatrix);
