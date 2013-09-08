/* Function to construct and initialize X-windows Window */ 
void xInitWindow(int *argv, char **argc) 
{ 
  XVisualInfo *xvVisualInfo; 
  Colormap    cmColorMap; 
  XSetWindowAttributes winAttributes; 
  GLXContext  glXContext; 

  /* Open the Display */ 
  dpDisplay = XOpenDisplay(NULL); 
  if(dpDisplay == NULL) 
  { 
    printf("Could not open display!\n\r"); 
    exit(0); 
  } 

  /* Check for GLX extension to X-Windows */ 
  if(!glXQueryExtension(dpDisplay, NULL, NULL)) 
  { 
    printf("X server has no GLX extension.\n\r"); 
    exit(0); 
  } 

  /* Grab a doublebuffering RGBA visual as defined in dblBuf */ 
  xvVisualInfo = glXChooseVisual(dpDisplay, DefaultScreen(dpDisplay), dblBuf); 
  if(xvVisualInfo == NULL) 
  { 
    printf("No double buffering RGB visual with depth buffer available.\n\r"); 
    exit(0); 
  } 

  /* Create a window context */ 
  glXContext = glXCreateContext(dpDisplay, xvVisualInfo, None, True); 
  if(glXContext == NULL) 
  { 
    printf("Could not create rendering context.\n\r"); 
    exit(0); 
  } 

  /* Create the color map for the new window */ 
  cmColorMap = XCreateColormap(dpDisplay, RootWindow(dpDisplay, xvVisualInfo->screen), xvVisualInfo->visual, AllocNone); 
  winAttributes.colormap = cmColorMap; 
  winAttributes.border_pixel = 0; 
  winAttributes.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask | 
                             KeyPressMask; 

  /* Create the actual window object */ 
  win = XCreateWindow(dpDisplay, 
                      RootWindow(dpDisplay, xvVisualInfo->screen), 
                      0, 
                      0, 
                      640,                      // Horizontal Size 
                      480,                      // Veritical Size 
                      0, 
                      xvVisualInfo->depth, 
                      InputOutput, 
                      xvVisualInfo->visual, 
                      CWBorderPixel | CWColormap | CWEventMask, 
                      &winAttributes); 

  /* Set the standard properties for the window. */ 
  XSetStandardProperties(dpDisplay, 
                         win, 
                         "Test1", 
                         "Test1", 
                         None, 
                         (char **) argv, 
                         (int) argc, 
                         NULL); 

  /* Establish new event */ 
  wmDeleteWindow = XInternAtom(dpDisplay, "WM_DELETE_WINDOW", False); 
  XSetWMProtocols(dpDisplay, win, &wmDeleteWindow, 1); 

  /* Bind the OpenGL context to the newly created window. */ 
  glXMakeCurrent(dpDisplay, win, glXContext); 

  /* Make the new window the active window. */ 
  XMapWindow(dpDisplay, win); 
}
