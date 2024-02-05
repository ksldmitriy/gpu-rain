#pragma once

struct _XDisplay;
typedef _XDisplay Display;

typedef unsigned long XID;
typedef XID Window;

struct _XScreen;

class XRootWindow {
public:
  XRootWindow();
  XRootWindow(XRootWindow &) = delete;
  XRootWindow &operator=(XRootWindow &) = delete;
  ~XRootWindow();

  void Open();
  void Close();

private:
  void OpenXDisplay();
  void CloseXDisplay();

private:
  Display *m_display = nullptr;
  _XScreen *m_screen;
  Window m_window;
};
