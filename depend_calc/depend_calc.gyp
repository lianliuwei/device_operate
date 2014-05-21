{
  'variables': {
    'chromium_code': 1,
    'BASE_PATH': '../third_party/chromium/',
  },
  'targets': [
    {
      'target_name': 'depend_calc',
      'type': 'static_library',
      'dependencies': [
        '<(BASE_PATH)/base/base_bin.gyp:base_bin',
        '<(BASE_PATH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
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
        '<(BASE_PATH)/base/base_bin.gyp:base_bin',
        '<(BASE_PATH)/base/base_bin.gyp:base_prefs_bin',
        '<(BASE_PATH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(BASE_PATH)/testing/gtest.gyp:gtest',
        '../test/test.gyp:test_support',
        'depend_calc',
      ],
    },
  ],
}