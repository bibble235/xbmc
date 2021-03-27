/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIDialogPlayerSettingsOSD.h"

#include "Application.h"
#include "GUIPassword.h"
#include "ServiceBroker.h"
#include "profiles/ProfileManager.h"
#include "dialogs/GUIDialogYesNo.h"
#include "settings/MediaSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "settings/lib/Setting.h"
#include "utils/log.h"
#include "video/VideoDatabase.h"

#include <string>
#include <vector>

#define SETTING_AUDIO_MAKE_DEFAULT             "mediacodec.makedefault"
#define SETTING_USEMEDIACODECSURFACE           "mediacodec.usermediacodecsurface"
#define SETTING_USEMEDIACODEC                  "mediacodec.usermediacodec"

CGUIDialogPlayerSettingsOSD::CGUIDialogPlayerSettingsOSD()
  : CGUIDialogSettingsManualBase(WINDOW_DIALOG_PLAYER_OSD_SETTINGS, "DialogSettings.xml")
{
}

CGUIDialogPlayerSettingsOSD::~CGUIDialogPlayerSettingsOSD() = default;

void CGUIDialogPlayerSettingsOSD::OnSettingChanged(const std::shared_ptr<const CSetting>& setting)
{
  if (setting == NULL)
    return;

  CGUIDialogSettingsManualBase::OnSettingChanged(setting);

  const std::string &settingId = setting->GetId();
  if (settingId == SETTING_USEMEDIACODECSURFACE)
  {
    m_userMediaCodecSurface = std::static_pointer_cast<const CSettingBool>(setting)->GetValue();
    CServiceBroker::GetSettingsComponent()->GetSettings()->SetBool(CSettings::SETTING_VIDEOPLAYER_USEMEDIACODECSURFACE, m_userMediaCodecSurface);
  }
  else if (settingId == SETTING_USEMEDIACODEC)
  {
    m_userMediaCodec = std::static_pointer_cast<const CSettingBool>(setting)->GetValue();
    CServiceBroker::GetSettingsComponent()->GetSettings()->SetBool(CSettings::SETTING_VIDEOPLAYER_USEMEDIACODEC, m_userMediaCodec);
  }

}

void CGUIDialogPlayerSettingsOSD::OnSettingAction(const std::shared_ptr<const CSetting>& setting)
{
  if (setting == NULL)
    return;

  CGUIDialogSettingsManualBase::OnSettingAction(setting);

  const std::string &settingId = setting->GetId();
  if (settingId == SETTING_AUDIO_MAKE_DEFAULT)
    Save();
}

bool CGUIDialogPlayerSettingsOSD::Save()
{
  const std::shared_ptr<CProfileManager> profileManager = CServiceBroker::GetSettingsComponent()->GetProfileManager();

  if (!g_passwordManager.CheckSettingLevelLock(SettingLevel::Expert) &&
      profileManager->GetMasterProfile().getLockMode() != LOCK_MODE_EVERYONE)
    return true;

  // prompt user if they are sure
  if (!CGUIDialogYesNo::ShowAndGetInput(CVariant{12376}, CVariant{12377}))
    return true;

  // reset the settings
  CVideoDatabase db;
  if (!db.Open())
    return true;

  db.EraseAllVideoSettings();
  db.Close();

  CMediaSettings::GetInstance().GetDefaultVideoSettings() = g_application.GetAppPlayer().GetVideoSettings();
  CMediaSettings::GetInstance().GetDefaultVideoSettings().m_AudioStream = -1;
  CServiceBroker::GetSettingsComponent()->GetSettings()->Save();

  return true;
}

void CGUIDialogPlayerSettingsOSD::SetupView()
{
  CGUIDialogSettingsManualBase::SetupView();

  SetHeading(24186);
  SET_CONTROL_HIDDEN(CONTROL_SETTINGS_OKAY_BUTTON);
  SET_CONTROL_HIDDEN(CONTROL_SETTINGS_CUSTOM_BUTTON);
  SET_CONTROL_LABEL(CONTROL_SETTINGS_CANCEL_BUTTON, 15067);
}

void CGUIDialogPlayerSettingsOSD::InitializeSettings()
{
  CGUIDialogSettingsManualBase::InitializeSettings();

  const std::shared_ptr<CSettingCategory> category = AddCategory("mediacodec", -1);
  if (category == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogPlayerSettingsOSD: unable to setup settings");
    return;
  }

  // get all necessary setting groups
  const std::shared_ptr<CSettingGroup> groupMediaCodec = AddGroup(category);
  if (groupMediaCodec == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogPlayerSettingsOSD: unable to setup settings");
    return;
  }

  const std::shared_ptr<CSettingGroup> groupSaveAsDefault = AddGroup(category);
  if (groupSaveAsDefault == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogPlayerSettingsOSD: unable to setup settings");
    return;
  }

  // Add Controls to View
  m_userMediaCodecSurface = CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_VIDEOPLAYER_USEMEDIACODECSURFACE);
  AddToggle(groupMediaCodec, SETTING_USEMEDIACODECSURFACE, 13440, SettingLevel::Basic, m_userMediaCodecSurface);

  m_userMediaCodec = CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_VIDEOPLAYER_USEMEDIACODEC);
  AddToggle(groupMediaCodec, SETTING_USEMEDIACODEC, 13439, SettingLevel::Basic, m_userMediaCodec);

  AddButton(groupSaveAsDefault, SETTING_AUDIO_MAKE_DEFAULT, 12376, SettingLevel::Basic);
}