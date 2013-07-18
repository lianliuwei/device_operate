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
        'device/register/soft_diff_register.h',
        'device/register/soft_diff_register.cc',
        'device/canscope_device_constants.h',
      ], 
    },
  ],
}