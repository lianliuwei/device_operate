{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'osc_chnl_calc',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
        '../third_party/chromium/base/base_bin.gyp:base_prefs_bin',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'osc_chnl_calc/chnl_calc_result.h',
      ], 
    },
  ],
}