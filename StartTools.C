//************************* My aliases ************************************

void StartTools()
{
TBrowser *browser = GetBrowser();

gROOT->GetRootFolder()->AddFolder("Tools","Tools");
TFolder *f = (TFolder*)gROOT->GetRootFolder()->FindObjectAny("Tools");

Gw::GSPlayerTUI *SpectrumPlayer = new
Gw::GSPlayerTUI("SpectrumPlayer","SpectrumPlayer");
f->Add(SpectrumPlayer);

browser->BrowseObject(f);

//if ( browser ) browser->Draw();
}

//______________________________________________________________________________

TBrowser *GetBrowser()
{
TBrowser *b;

   if ( gROOT->GetListOfBrowsers()->GetSize() == 0 ) {
     b = new TBrowser("Browser","ROOT Object Browser"); return b;
   }

   TIter next(gROOT->GetListOfBrowsers());

   while ( (b = (TBrowser *)next()) != NULL ) {
     if ( strcmp("ROOT Object Browser",b->GetTitle()) == 0 ) {
       gROOT->GetListOfBrowsers()->Remove(b); delete b;
       b = new TBrowser("Browser","ROOT Object Browser");
     }
   }
   return b;
}
