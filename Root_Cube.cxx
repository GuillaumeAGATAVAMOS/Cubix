#include "includes_cube.h"
#include "Cube_Player.h"
#include "ENSDF_DATAe680.h"
#include "TImage.h"

void Plot_Chart(Int_t Option)

{



    Cube=new Cube_Player ("toto","tata");
    DATABASE=new ENSDF_DATAe680("toto2","tata");
    LS=new Build_LS("","",Exp_Directory);

    TString FileChart = Form("ZivsNi.root");

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

        Chart_Bidim = (TH2F*)C_Chart->GetPrimitive("hh");//getlist
        Chart_Bidim->SetTitle("E680 Chart");
        Chart_Bidim->GetYaxis()->SetTitle("Z");
        Chart_Bidim->GetXaxis()->SetTitle("A");
        Chart_Bidim->GetYaxis()->SetRangeUser(26,48);
        Chart_Bidim->GetXaxis()->SetRangeUser(38,73);
        Chart_Bidim->SetStats(0);

        cout<<endl;InfoMessage("*************************************");
        InfoMessage("********* CHART WAS FOUND ***********");
        InfoMessage("*************************************");
        cout<<endl;

        C_Chart->Draw();

        if(Option!=1)
        {
            TString ImagePath="Cubix.png";
            TImage* im = TImage::Open(ImagePath);
            TCanvas *C_Picture= new TCanvas("Cubix","Cubix",C_Chart->GetWindowTopX(),C_Chart->GetWindowTopY(),540,760);
            C_Picture->cd();
            C_Picture->SetBit(kNoContextMenu);
            im->Draw("xyz");
        }

        Cube->SetCanvas();

        //Cube->Search_Coinc();
    }

}
