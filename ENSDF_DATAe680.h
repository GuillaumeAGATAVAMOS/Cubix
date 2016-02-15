#include <iostream>
#include <utility>
#include "TH2.h"
#include "TString.h"
#include "TNamed.h"
#include "TObject.h"
#include "Cube_Player.h"


#ifndef ENSDF_DATAe680_H
#define ENSDF_DATAe680_H



namespace Gw {

    class ENSDF_DATAe680: public TNamed
    {

    private:

    protected:

    public:

        ENSDF_DATAe680(const char* name, const char *title);
       // ENSDF_DATAe680();
        virtual ~ENSDF_DATAe680();

        Float_t *Import_E(Int_t Z,Int_t A,Int_t Draw=0,Int_t Verbose=0,Float_t StrenghtMin=0);
        Float_t *Import_C(Int_t Z,Int_t A);
        Float_t  *Import_S(Int_t Zz,Int_t A);
        Float_t  *Import_ILE(Int_t Z,Int_t A);
        Float_t  *Import_FLE(Int_t Z,Int_t A);
        Float_t  *Import_SFL(Int_t Z,Int_t A);
        Float_t  *Import_SIL(Int_t Z,Int_t A);

        Float_t Get_Ground_Gamma(Int_t Z,Int_t A);

        Int_t GetSizeOfGammas();

        ClassDef(ENSDF_DATAe680,1)

            };

}//namespace

#endif
