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

  void CreateGLContext();
  void SwapBuffers();
  
private:
  void OpenXDisplay();
  void CloseXDisplay();

private:
  Display *m_display = nullptr;
  _XScreen *m_screen = nullptr;
  Window m_window = 0;

  static constexpr int screen_number = 0;
};
