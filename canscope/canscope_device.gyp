{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'canscope_device',
      'type': 'executable',
      'variables': {
        'mfc_target': 1,
      },
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
        '../device/device.gyp:device',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'device/canscope_device_constants.h',
        'device/register/soft_diff_register.h',
        'device/register/soft_diff_register.cc',
        'device/register/analog_ctrl_register.h',
        'device/register/analog_ctrl_register.cc',
      ], 
    },
    {
      'target_name': 'canscope_device_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'device/canscope_device_constants.h',
        'device/register/soft_diff_register.h',
        'device/register/soft_diff_register.cc',
        'device/register/soft_diff_register_unittest.cc',
        'device/register/analog_ctrl_register.h',
        'device/register/analog_ctrl_register.cc',
        'device/register/analog_ctrl_register_unittest.cc',
        
      ],
      'include_dirs': [
        '..',
      ], 
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base/base_bin.gyp:base_i18n_bin',
        '../third_party/chromium/base/base_bin.gyp:base_static_bin',
        '../third_party/chromium/base/base_bin.gyp:base_prefs_bin',
        '../third_party/chromium/base/base_bin.gyp:test_support_base_bin',
        '../third_party/chromium/base/base_bin.gyp:run_all_unittests_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
        '../third_party/chromium/testing/gmock.gyp:gmock',
        '../third_party/chromium/testing/gtest.gyp:gtest',
        '../third_party/chromium/third_party/icu_bin.gyp:icui18n_bin',
        '../third_party/chromium/third_party/icu_bin.gyp:icuuc_bin',
        '../device/device.gyp:device',
      ],
    },
  ],
}