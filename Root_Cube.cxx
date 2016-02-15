#include "includes_cube.h"
#include "Cube_Player.h"
#include "ENSDF_DATAe680.h"

void Plot_Chart()

{

    Cube=new Cube_Player ("toto","tata");
    DATABASE=new ENSDF_DATAe680("toto2","tata");
    LS=new Build_LS("","",Exp_Directory);

    TString FileChart = Form("ZivsMi.root");

    if(!gSystem->IsFileInIncludePath(FileChart))
    {

        ErrorMessage("*************************************");
        ErrorMessage("******Chart is not in include PATH***");
        ErrorMessage("*************************************");
    }
    else
    {

        fInputChart = TFile::Open(FileChart);
        C_Chart = (TCanvas *)gFile->Get(Name);
        C_Chart->SetName("C_Chart");

        Chart_Bidim = (TH2F*)C_Chart->GetPrimitive("ZivsMi");//getlist
        Chart_Bidim->SetTitle("E680 Chart");
        Chart_Bidim->GetYaxis()->SetTitle("Z");
        Chart_Bidim->GetXaxis()->SetTitle("A");
        Chart_Bidim->GetYaxis()->SetRangeUser(28,48);
        Chart_Bidim->GetXaxis()->SetRangeUser(74,122);
        Chart_Bidim->SetStats(0);

        cout<<endl;InfoMessage("*************************************");
        InfoMessage("********* CHART WAS FOUND ***********");
        InfoMessage("*************************************");
        cout<<endl;

        C_Chart->Draw();

        Cube->SetCanvas();

        //Cube->Search_Coinc();
    }

}
