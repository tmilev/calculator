#include "./fox/include/fx.h"

class FoxTest : public FXMainWindow
{
  FXDECLARE (FoxTest)
  public:
    FoxTest (FXApp* a);
    long onCmdCommand (FXObject*, FXSelector, void*);
    enum
    { ID_COMMAND = FXMainWindow::ID_LAST,
      ID_LAST
    };
    virtual void create ();
  private:
    FXLabel *label;
  protected:
    FoxTest () {}
};

FXDEFMAP (FoxTest) FoxTestMap[] =
{ FXMAPFUNC(SEL_COMMAND,    FoxTest::ID_COMMAND,       FoxTest::onCmdCommand),
};

FXIMPLEMENT (FoxTest, FXMainWindow, FoxTestMap, ARRAYNUMBER (FoxTestMap))

FoxTest::FoxTest (FXApp *a) : FXMainWindow (a, "FoxTest", NULL, NULL, DECOR_ALL, 50, 50, 200, 60)
{ FXVerticalFrame *vertFrame = new FXVerticalFrame (this, LAYOUT_FILL_X|LAYOUT_FILL_Y|PACK_UNIFORM_WIDTH);
  FXHorizontalFrame *horiFrame = new FXHorizontalFrame (vertFrame, LAYOUT_CENTER_X|PACK_UNIFORM_WIDTH|FRAME_RAISED);
  new FXButton (horiFrame, "Drück mich", NULL, this, FoxTest::ID_COMMAND);
  new FXButton (horiFrame, "Exit", NULL, getApp (), FXApp::ID_QUIT);
  label = new FXLabel (vertFrame, "Hallo", NULL, LAYOUT_CENTER_X|LAYOUT_CENTER_Y);
}

void FoxTest::create ()
{ FXMainWindow::create ();
  show ();
}

long FoxTest::onCmdCommand (FXObject*, FXSelector, void*)
{ label->setText ("Danke!");
  return 1;
}

int main (int argc, char *argv[])
{ FXApp* application = new FXApp("FoxyTesting","Test");
  application->init (argc, argv);
  new FoxTest (application);
  application->create();
  application->run();
}
