/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "settings/dialogs/GUIDialogSettingsManualBase.h"

#include <string>
#include <utility>
#include <vector>

class CVariant;
struct IntegerSettingOption;

class CGUIDialogPlayerSettingsOSD : public CGUIDialogSettingsManualBase
{
public:
  CGUIDialogPlayerSettingsOSD();
  ~CGUIDialogPlayerSettingsOSD() override;

protected:
  // implementations of ISettingCallback
  void OnSettingChanged(const std::shared_ptr<const CSetting>& setting) override;
  void OnSettingAction(const std::shared_ptr<const CSetting>& setting) override;

  // specialization of CGUIDialogSettingsBase
  bool AllowResettingSettings() const override { return false; }
  bool Save() override;
  void SetupView() override;

  // specialization of CGUIDialogSettingsManualBase
  void InitializeSettings() override;

  bool m_userMediaCodecSurface = true;
  bool m_userMediaCodec = true;

private:
};
