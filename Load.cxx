
#include <TString.h>
#include <TRint.h>
#include <TROOT.h>
#include <TEnv.h>

void Load()
{


        printf("******************************************************************** \n") ;
        printf("                    Messages from VAMOS AGATA e680!! \n") ;
        printf("******************************************************************** \n") ;

        // to add to ROOT the Gw path to look for macros
        TString tmp, macroPATH; const Char_t *CompOpt = "";
        TString OutPutPath = "/agataDAS1/EXILL/ClementWorkingDir/test/";

        macroPATH = gROOT->GetMacroPath(); tmp = Gw::Env::GetPath("MacrosPath");
        if ( macroPATH.Contains(tmp.Data()) == false ) {
                printf("- Add %s to the macros path \n",tmp.Data());
                tmp += ":";
                macroPATH.Prepend(tmp.Data()); gROOT->SetMacroPath(macroPATH.Data());
        }

        // add GammaWare include path to ROOT for compilation
        tmp = Gw::Env::GetPath("IncludePath");  printf("- Add %s to the include path \n",tmp.Data());
        tmp.Prepend(" .include ");
        gROOT->ProcessLine(tmp.Data());

        // load the different Gw libraries needed
        gSystem->Load("libGWCORE.so");
        gSystem->Load("libGWPHYSICS.so");
        gSystem->Load("libEve.so");
        gSystem->Load("libGWTOOLS.so");
        gSystem->Load("libGWADF.so");
        gSystem->Load("libGWADFE.so");
        gSystem->Load("libGWILL.so");

        tmp = "./" ;  printf("- Add %s to the include path \n",tmp.Data());
        tmp.Prepend(" .include ");
        gROOT->ProcessLine(tmp.Data());

               // load the different Gw libraries needed
               gSystem->Load("libGWCORE.so");
               gSystem->Load("libGWPHYSICS.so");
               gSystem->Load("libEve.so");
               gSystem->Load("libGWTOOLS.so");
               gSystem->Load("libGWADF.so");
               gSystem->Load("libGWADFE.so");
               gSystem->Load("libGWILL.so");

               tmp = "./" ;  printf("- Add %s to the include path \n",tmp.Data());
               tmp.Prepend(" .include ");
               gROOT->ProcessLine(tmp.Data());



               tmp = Form(".L Spectrum_Loader.cxx+%s",CompOpt);
               gROOT->ProcessLine(tmp.Data());

               tmp = Form(".L SpreadIntensityMatrix.cxx+%s",CompOpt);
               gROOT->ProcessLine(tmp.Data());

               tmp = Form(".L ENSDF_DATAe680.cxx+%s",CompOpt);
               gROOT->ProcessLine(tmp.Data());

               tmp = Form(".L Build_LS.cxx+%s",CompOpt);
               gROOT->ProcessLine(tmp.Data());

               tmp = Form(".L Cube_Player.cxx+%s",CompOpt);
               gROOT->ProcessLine(tmp.Data());

               tmp = Form(".L Root_Cube.cxx+%s",CompOpt);
               gROOT->ProcessLine(tmp.Data());

               Plot_Chart();
   }
