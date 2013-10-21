{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'depend_calc',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
      ],      
      'include_dirs': [
        '..',
      ],   
    
      'sources': [
        'calc_delegate.h',
        'calc_executer.h',
        'calc_executer.cc',
        'calc_group.h',
        'calc_group.cc',
        'calc_group_walker.h',
        'calc_group_walker.cc',
        'calc_item.h',
        'calc_item.cc',
        'compute_manager.h',
        'calc_data.h',
        'calc_data.cc',
      ],
    },

    {
      'target_name': 'depend_calc_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'calc_group_unittest.cc',
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
        'depend_calc',
      ],
    },
  ],
}