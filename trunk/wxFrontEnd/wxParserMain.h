/***************************************************************
 * Name:      wxParserMain.h
 * Purpose:   Defines Application Frame
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-13
 * Copyright: todor milev ()
 * License: use however you wish
 **************************************************************/

#ifndef WXPARSERMAIN_H
#define WXPARSERMAIN_H

//(*Headers(wxParserFrame)
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)

//#include "openglXP.h"
#include "wxDrawPanel.h"
#include "wxParserOutput.h"
#include "wxStatus.h"
#include "wxPNGdisplay.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include "wx/stdpaths.h"
#include <wx/image.h>
#include "../vpf.h"




DEFINE_EVENT_TYPE(wxEVT_ComputationOverMakeReport)
DEFINE_EVENT_TYPE(wxEVT_ComputationProgressReport)

class wxParserFrame: public wxFrame
{
    public:
        wxButton* GetButton1(){return this->Button1;}
        wxParserFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~wxParserFrame();
        wxDrawPanel* theDrawPanel;
        wxParserOutput* theParserOutput;
        wxStatus* theStatus;
        wxPNGdisplay* thePNGdisplay;
//        openglXP theOpenGLXP;
        MutexWrapper mutexRuN;
        std::string thePath;
        std::string theSettingsFileName;
        std::string ProgressReportString;
        std::string StatusString;
        wxCommandEvent eventComputationOver;
        wxCommandEvent eventProgressReport;
        wxPaintEvent eventPNGrepaint;
        List<wxBitmap*> theBitmapList;
        List<wxImage> theSlides;
        List<std::string> theSlidesFileNames;
        List<int> theSlideFrameIndices;
        int indexCurrentPng;
        int numSlideFrames;
        double scalePNG;
        int bitmapH, bitmapW;
        bool Quitting;
        bool TimerReady;
        void StartTimer(){this->Timer1.Start();}
        wxTimer& GetTimer(){return this->Timer1;}
        void UpdateChoices();
        friend class AnimationBuffer;
        friend class wxDrawPanel;
        CommandList theCommandList;
    private:

        //(*Handlers(wxParserFrame)
        void OnQuit(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnTimer1Trigger(wxTimerEvent& event);
        void OnChoice1Select(wxCommandEvent& event);
        void OnTextCtrl1Text(wxCommandEvent& event);
        void OnTextCtrl1Text1(wxCommandEvent& event);
        void OnTextCtrl1Text2(wxCommandEvent& event);
        void OnTextCtrl1Text3(wxCommandEvent& event);
        void OnTextCtrl1Text4(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        //*)

        //(*Identifiers(wxParserFrame)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(wxParserFrame)
        wxButton* Button1;
        wxButton* Button2;
        wxTimer Timer1;
        wxTextCtrl* TextCtrl1;
        //*)
        void OnComputationOver(wxCommandEvent& ev);
        void OnProgressReport(wxCommandEvent& ev);
        void RunTheComputation();
        void ReadSettings();
        void WriteSettings();
        DECLARE_EVENT_TABLE()
};


#endif // WXPARSERMAIN_H
