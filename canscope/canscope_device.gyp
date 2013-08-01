{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'canscope_device',
      'type': 'executable',
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
        'device/register/analog_switch_register.h',
        'device/register/analog_switch_register.cc',
        'device/register/trigger1_register.h',
        'device/register/trigger1_register.cc',
        'device/register/trigger2_register.h',
        'device/register/trigger2_register.cc',
        'device/register/trigger_state_register.h',
        'device/register/trigger_state_register.cc',
        'device/register/eye_register.h',
        'device/osc_device.h',
        'device/osc_device.cc',
        'device/osc_device_handle.h',
        'device/osc_device_handle.cc',
      ], 
    },
    {
      'target_name': 'canscope_usb_port',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
      ],
      'include_dirs': [
        '..',
      ],
      'direct_dependent_settings': {      
        'msvs_settings':{
          'VCLinkerTool': {
            'AdditionalDependencies': [
              'setupapi.lib',
            ],
          },
        },
      },
      'sources': [
        'device/usb/usb_port.h',
        'device/usb/usb_port.cc',
        'device/usb/usb_command.h',
        'device/usb/usb_command.cc',
        'device/usb/device_info.h',
        'device/usb/device_info.cc',
        'device/usb/usb_constants.h',
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
        'device/register/analog_switch_register.h',
        'device/register/analog_switch_register.cc',
        'device/register/analog_switch_register_unittest.cc',
        'device/register/trigger1_register.h',
        'device/register/trigger1_register.cc',
        'device/register/trigger1_register_unittest.cc',
        'device/register/trigger2_register.h',
        'device/register/trigger2_register.cc',
        'device/register/trigger2_register_unittest.cc',
        'device/register/trigger_state_register.h',
        'device/register/trigger_state_register.cc',
        'device/register/trigger_state_register_unittest.cc',
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
    {
      'target_name': 'canscope_usb_port_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'device/usb/usb_port_unittest.cc',
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
        'canscope_usb_port',
        'fpga',
      ],
    },
    {
      'target_name': 'fpga',
      'type': 'none',

      'copies': [
        {
          'destination': '<(PRODUCT_DIR)/../Release/',
          'files': [
            'fpga/Release/CANScope.dll',
          ],
        },
        {
          'destination': '<(PRODUCT_DIR)/../Debug/',
          'files': [
            'fpga/Debug/CANScope.dll',
          ],
        },
      ],   
    },
  ],
}