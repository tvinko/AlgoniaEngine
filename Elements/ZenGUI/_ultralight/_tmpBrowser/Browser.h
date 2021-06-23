#include <AppCore/AppCore.h>
#include "ZenGUI.h"

using namespace ultralight;

class Browser
{
public:
  Browser();
  virtual ~Browser();

  virtual void Run();

protected:
  RefPtr<App> app_;
  RefPtr<Window> window_;
  std::unique_ptr<UI> ui_;
};
