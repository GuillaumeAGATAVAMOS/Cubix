#include <iostream>
#include <utility>
#include "TMatrixD.h"
#include "TMath.h"
#include "TMatrix.h"
#include "TArrayD.h"
#include "TMatrixDBase.h"
#include "TMatrixDLazy.h"
#include "TVectorD.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"

#ifndef SPREADINTENSITYMATRIX_H
#define SPREADINTENSITYMATRIX_H


namespace Gw {

class SpreadIntensityMatrix : public TNamed
{


private:
    Float_t fCorr_Above;
    Float_t fSGate;


protected:


public:
    SpreadIntensityMatrix();
    SpreadIntensityMatrix(const char* name, const char *title);
    virtual ~SpreadIntensityMatrix();

    TMatrixD Spread_Below(Float_t S_Gate);
    void Spread_Above(Float_t Corr_Intensity);
    TMatrixD Get_Total_P(TMatrixD Mat_TS_Brute,const Int_t Size);
    TMatrixD Get_Final_M(TMatrixD P,TMatrix Fill_S,const Int_t Size);
    TMatrixD Get_S_And_E(TMatrixD Final_Matrix,TMatrixD E_Matrix,Float_t Gate,const Int_t Size);
    TMatrixD Get_S_And_E_2(TMatrixD Mat_S,TMatrixD Mat_P,TMatrixD Final_Matrix,TMatrixD E_Matrix,Float_t Gate,Float_t Gate2,const Int_t Size);
    TMatrixD Get_S_And_E_3(TMatrixD Mat_P,TMatrixD Final_Matrix,TMatrixD E_Matrix,Float_t Gate1,Float_t Gate2,Float_t Gate3,const Int_t Size);



    Float_t Get_Corr_Above();
    Float_t Get_S_Gate();



    ClassDef(SpreadIntensityMatrix,1)



        };
}

#endif // SPREADINTENSITYMATRIX_H
