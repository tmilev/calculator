/***************************************************************
 * Name:      wxParserMain.h
 * Purpose:   Defines Application Frame
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-13
 * Copyright: todor milev ()
 * License:
 **************************************************************/

#ifndef WXPARSERMAIN_H
#define WXPARSERMAIN_H

//(*Headers(wxParserFrame)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

#include "wxDrawPanel.h"
#include "wxParserOutput.h"
#include "wxStatus.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include "wx/stdpaths.h"
#include "polyhedra.h"


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
        MutexWrapper mutexRuN;
        std::string thePath;
        std::string theSettingsFileName;
        std::string ProgressReportString;
        std::string StatusString;
        wxCommandEvent eventComputationOver;
        wxCommandEvent eventProgressReport;
        List<wxBitmap*> theBitmapList;
        int bitmapH, bitmapW;
        bool Quitting;
        void StartTimer(){this->Timer1.Start();}
        wxTimer& GetTimer(){return this->Timer1;}
        void UpdateChoices();
    private:

        //(*Handlers(wxParserFrame)
        void OnQuit(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnTimer1Trigger(wxTimerEvent& event);
        void OnChoice1Select(wxCommandEvent& event);
        void OnSpinCtrl1Change(wxSpinEvent& event);
        //*)

        //(*Identifiers(wxParserFrame)
        static const long ID_CHOICE1;
        static const long ID_SPINCTRL1;
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_TIMER1;
        //*)

        //(*Declarations(wxParserFrame)
        wxSpinCtrl* SpinCtrl1;
        wxButton* Button1;
        wxButton* Button2;
        wxTextCtrl* TextCtrl1;
        wxChoice* Choice1;
        wxTimer Timer1;
        //*)
        void OnComputationOver(wxCommandEvent& ev);
        void OnProgressReport(wxCommandEvent& ev);
        void RunTheComputation();
        void ReadSettings();
        void WriteSettings();
        DECLARE_EVENT_TABLE()
};

#endif // WXPARSERMAIN_H
