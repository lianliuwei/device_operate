{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'device',
      'type': 'executable',
      'variables': {
        'mfc_target': 1,
      },
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'bool_bind.h',
        'canscope_device.h',
        'canscope_device_constants.h',
        'canscope_osc_device.h',
        'canscope_osc_device2.h',
        'device_errors.h',
        'endian.h',
        'int_bind.h',
        'register_group.h',
        'register_memory.h',
        'register_memory.cc',
        'register_property.h',
      ], 
    },
    {
      'target_name': 'device_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'bool_bind.h',
        'bool_bind_unittest.cc',
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
      ],
    },
  ],
}