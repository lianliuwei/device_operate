{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'device',
      'type': 'static_library',
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
        'one_byte_type_bind.h',
        'simple_type_bind.h',
        'uint16_split_bind.h',
        'uint16_split_bind.cc',
        'canscope_device.h',
        'canscope_osc_device.h',
        'canscope_osc_device2.h',
        'device_errors.h',
        'endian.h',
        'int_bind.h',
        'register_group.h',
        'register_memory.h',
        'register_memory.cc',
        'register_property.h',
        'simple_type_bind.h',
      ], 
    },
    {
      'target_name': 'device_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'bool_bind_unittest.cc',
        'uint8_bind_unittest.cc',
        'uint16_split_bind_unittest.cc',
        'register_memory_unittest.cc',
        'simple_type_bind_unittest.cc',
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
        'device',
      ],
    },
  ],
}