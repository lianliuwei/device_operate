{
  'variables': {
    'chromium_code': 1,
    'BASE_PATH': '../third_party/chromium/',
  },
    
  'targets': [
    {
      'target_name': 'test_support',
      'type': 'static_library',
      'sources' : [
        'run_all_unittests.cc',
        'test_suite.h',
        'test_suite.cc',
      ],
      'include_dirs': [
        '..',
      ], 
       'dependencies': [
        '<(BASE_PATH)/base/base_bin.gyp:base_bin',
        '<(BASE_PATH)/testing/gtest.gyp:gtest',
       ],
    },

  ],
}