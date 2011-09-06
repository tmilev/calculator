/***************************************************************
 * Name:      wxParserMain.h
 * Purpose:   Defines Application Frame
 * Author:    todor milev (todor.milev@gmail.com)
 * Created:   2011-09-05
 * Copyright: todor milev ()
 * License:
 **************************************************************/

#ifndef WXPARSERMAIN_H
#define WXPARSERMAIN_H

//(*Headers(wxParserDialog)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "wxParserOutput.h"
#include "wxIndicatorWindow.h"
#include "wxVPDrawCanvas.h"
#include "polyhedra.h"
#include "wx/stdpaths.h"
#include "wx/dcclient.h"

DEFINE_EVENT_TYPE(wxEVT_ComputationOverMakeReport)
DEFINE_EVENT_TYPE(wxEVT_ComputationProgressReport)

class wxParserDialog: public wxDialog
{
    public:
        MutexWrapper mutexRuN;
        wxParserOutput* frameParserOutput;
        wxVPDrawCanvas* frameDrawCanvas;
        wxIndicatorWindow* frameIndicatorWindow;
        std::string thePath;
        std::string theSettingsFileName;
        wxButton* GetButton1(){return this->Button1;}
        wxCommandEvent eventComputationOver;
        wxCommandEvent eventProgressReport;
        wxParserDialog(wxWindow* parent,wxWindowID id = -1);
        std::string ProgressReportString;
        std::string StatusString;
        virtual ~wxParserDialog();

    private:

        //(*Handlers(wxParserDialog)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnInit(wxInitDialogEvent& event);
        void OnButton2Click(wxCommandEvent& event);
        //*)

        //(*Identifiers(wxParserDialog)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        //*)

        //(*Declarations(wxParserDialog)
        wxButton* Button1;
        wxBoxSizer* BoxSizer2;
        wxButton* Button2;
        wxBoxSizer* BoxSizer1;
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
