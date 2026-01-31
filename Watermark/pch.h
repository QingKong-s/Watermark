#pragma once
#include "eck\PchInc.h"
#include "eck\CCommDlg.h"
#include "eck\CUiAccess.h"
#include "eck\CForm.h"
#include "eck\CButton.h"
#include "eck\CComboBoxNew.h"
#include "eck\CEditExt.h"
#include "eck\CLabel.h"
#include "eck\CFontPicker.h"
#include "eck\CColorPicker.h"
#include "eck\CTableLayout.h"
#include "eck\CLinearLayout.h"
#include "eck\CIniExt.h"
#include "eck\ShellHelper.h"
#include "eck\CRegKey.h"
#include "eck\CTab.h"
#include "eck\CFrameLayout.h"
#include "eck\DwLayoutPath.h"
#include "eck\SystemHelper.h"
#include "eck\CTrayIcon.h"

#define SECURITY_WIN32 1
#include <security.h>

using eck::ComPtr;
using eck::SafeRelease;

using namespace std::literals;